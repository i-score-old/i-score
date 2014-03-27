/*
 * Copyright: LaBRI / SCRIME / L'Arboretum
 *
 * Authors: Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine (as of 16/03/2014)
 *
 *iscore.contact@gmail.com
 *
 * This software is an interactive intermedia sequencer.
 * It allows the precise and flexible scripting of interactive scenarios.
 * In contrast to most sequencers, i-score doesn’t produce any media, 
 * but controls other environments’ parameters, by creating snapshots 
 * and automations, and organizing them in time in a multi-linear way.
 * More about i-score on http://www.i-score.org
 *
 * This software is governed by the CeCILL license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and/ or redistribute the software under the terms of the CeCILL
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL license and that you accept its terms.
 */
#include "MaquetteScene.hpp"

#include <iostream>
#include <QInputDialog>
#include <QtGui>
#include <QColor>
#include <QGraphicsTextItem>
#include <QGraphicsSceneDragDropEvent>
#include <float.h>
#include <limits>
#include "BasicBox.hpp"
#include "ParentBox.hpp"
#include "ChooseTemporalRelation.hpp"
#include "MaquetteView.hpp"
#include "MainWindow.hpp"
#include "Relation.hpp"
#include "ConditionalRelation.hpp"
#include "Comment.hpp"
#include "TriggerPoint.hpp"
#include "ViewRelations.hpp"
#include "AttributesEditor.hpp"
#include "AbstractRelation.hpp"
#include "AbstractComment.hpp"
#include "TextEdit.hpp"
#include "PlayingThread.hpp"
#include "CurvesWidget.hpp"
#include "TimeBarWidget.hpp"
#include <QGraphicsProxyWidget>
#include <QGraphicsLineItem>

#include <sstream>
#include <map>
#include <cmath>

using std::map;
using std::vector;
using std::string;

const string MaquetteScene::DEFAULT_TRIGGER_MSG = "/trigger";
float MaquetteScene::MS_PER_PIXEL = 16;
const float MaquetteScene::MS_PRECISION = 10;
const float LINE_WIDTH = 1.1;

MaquetteScene::MaquetteScene(const QRectF & rect, AttributesEditor *editor)
  : QGraphicsScene(rect)
{
  _editor = editor;
  _clicked = false;
  _modified = false;
  _maxSceneWidth = 360000;

  _relation = new AbstractRelation; /// \todo pourquoi instancier une AbstractRelation ici ? (par jaime Chao)
  _playThread = new PlayingThread(this);
  _timeBar = new TimeBarWidget(0, this);  
  _timeBarProxy = addWidget(_timeBar);/// \todo Vérifier ajout si classe TimeBarWidget hérite de GraphicsProxyWidget ou GraphicsObject. Notamment pour lier avec background. (par jaime Chao)

  _progressLine = new QGraphicsLineItem(QLineF(sceneRect().topLeft().x(), sceneRect().topLeft().y(), sceneRect().bottomLeft().x(), MAX_SCENE_HEIGHT));

  addItem(_progressLine);  
}

MaquetteScene::~MaquetteScene()
{
  delete _tempBox;
  delete _maquette;
}

void
MaquetteScene::init()
{
  _triggersQueueList = new QList<TriggerPoint *>();
  _progressLine->setZValue(2);
  _timeBarProxy->setZValue(3);
  _timeBarProxy->setCacheMode(QGraphicsItem::ItemCoordinateCache);
  _timeBarProxy->setFlag(QGraphicsItem::ItemClipsToShape);

  _currentInteractionMode = SELECTION_MODE;
  setCurrentMode(SELECTION_MODE);
  _currentBoxMode = PB_MODE;
  _savedInteractionMode = _currentInteractionMode;
  _savedBoxMode = _currentBoxMode;
  _resizeMode = NO_RESIZE;
  _tracksView = false;
  _accelerationFactorSave = 1.;
  _accelerationFactor = 1.;

  /// \todo MainWindow appelle init() de MaquetteScene, qui instancie lui même Maquette puis l'init. (par jaime Chao)
  _maquette = Maquette::getInstance();
  _maquette->setScene(this);
  _maquette->init();      

  _tempBox = NULL;
  _resizeBox = NO_ID;

  _relation->setFirstBox(NO_ID);
  _relation->setSecondBox(NO_ID);
  _relationBoxFound = false;

  _mousePos = QPointF(0., 0.);

  connect(_timeBar, SIGNAL(timeOffsetEntered(unsigned int)), this, SLOT(changeTimeOffset(unsigned int)));
  connect(this, SIGNAL(stopPlaying()), this, SLOT(stopOrPause()));
}

void
MaquetteScene::updateProgressBar()
{
  if (_maquette->isExecutionOn()) {      
      _progressLine->setPos(_maquette->getCurrentTime() / MS_PER_PIXEL, sceneRect().topLeft().y());
      invalidate();
    }
  else {      
      _progressLine->setPos(_maquette->getTimeOffset() / MS_PER_PIXEL, sceneRect().topLeft().y());
      invalidate();
    }
}

void
MaquetteScene::zoomChanged(float value)
{
    std::cout<<value<<std::endl;
  setMaxSceneWidth(MaquetteScene::MAX_SCENE_WIDTH*value);
  std::cout<<" > "<<MaquetteScene::MAX_SCENE_WIDTH*value<<std::endl;
  updateProgressBar();
  _timeBar->updateZoom(value);
    
  Maquette::getInstance()->setViewZoom(QPointF(value, 1.));
}

void
MaquetteScene::changeTimeOffset(unsigned int timeOffset)
{
    stopAndGoToTimeOffset(timeOffset);
    
//    _view->repaint();
}

void
MaquetteScene::updateView()
{
  _view = static_cast<MaquetteView*>(views().front());
}

/// \todo Vérifier l'utilité de faire une surcouche d'appels de méthodes de AttributesEditor (_editor). (par jaime Chao)
void
MaquetteScene::updateWidgets()
{
  _editor->updateWidgets(true); /// \todo updateWidgets() est un public slot. mieux vaux faire appel au mécanisme d'auto-connexion des signaux dans Qt (QMetaObject) que de le garder en attribut de classe pour éviter le couplage. (par jaime Chao)
}

void
MaquetteScene::setAttributes(AbstractBox *abstract)
{
  _editor->setAttributes(abstract);
}

map<string, MyDevice>
MaquetteScene::getNetworkDevices()
{
  return _maquette->getNetworkDevices();
}

void
MaquetteScene::changeNetworkDevice(std::string deviceName, std::string pluginName, std::string IP, unsigned int port)
{
  _maquette->changeNetworkDevice(deviceName, pluginName, IP, port);
  setModified(true);
}

void
MaquetteScene::setNetworDeviceConfig(string deviceName, string pluginName, string IP, unsigned int port)
{
  emit(networkConfigChanged(deviceName, pluginName, IP, port));
}

bool
MaquetteScene::sendMessage(const string &message)
{
  return _maquette->sendMessage(message);
}

void
MaquetteScene::displayMessage(const string &message, unsigned int warningLevel)
{
  _view->mainWindow()->displayMessage(QString::fromStdString(message), warningLevel);
}

void
MaquetteScene::selectAll()
{
  QList<QGraphicsItem *> allItems = items();
  QList<QGraphicsItem *>::iterator it;

  for (it = allItems.begin(); it != allItems.end(); it++) {
      (*it)->setSelected(true);
    }
}

void
MaquetteScene::setTracksView(bool view)
{
  _tracksView = view;
  update();
}

bool
MaquetteScene::tracksView()
{
  return _tracksView;
}

unsigned int
MaquetteScene::getCurrentTime()
{
  return _maquette->getCurrentTime();
}

float
MaquetteScene::getProgression(unsigned int boxID)
{
  return _maquette->getProgression(boxID);
}

unsigned int
MaquetteScene::getTimeOffset()
{
    return _maquette->getTimeOffset();
}

void
MaquetteScene::drawForeground(QPainter * painter, const QRectF & rect)
{
  Q_UNUSED(rect);
  if (!_maquette->isExecutionOn()) {

      if (_currentInteractionMode == RELATION_MODE) {
          if (_clicked) {
              if (_relation->firstBox() != NO_ID) {
                  BasicBox *box = getBox(_relation->firstBox());
                  QPointF start;
                  switch (_relation->firstExtremity()) {
                      case BOX_START:
                        start = box->getLeftGripPoint();
                        break;

                      case BOX_END:
                        start = box->getRightGripPoint();
                        break;

                      case NO_EXTREMITY:
                        start = box->getCenter();
                        break;
                    }

                  if (_mousePos != _releasePoint && _mousePos != QPointF(0., 0)) {
                      QPainterPath painterPath;
                      painterPath.moveTo(start);
                      double startX = start.x(), startY = start.y();
                      double endX = 0., endY = 0.;
                      static const double arrowSize = 12.;
                      BasicBox *box = NULL;
                      if (itemAt(_mousePos) != 0) {
                          int type = itemAt(_mousePos)->type();
                          if (type == PARENT_BOX_TYPE) {
                              box = static_cast<BasicBox*>(itemAt(_mousePos));
                              if (_mousePos.x() < (box->mapToScene(box->boundingRect().topLeft()).x()
                                                   + BasicBox::RESIZE_TOLERANCE)) {
                                  endX = box->getLeftGripPoint().x();
                                  endY = box->getLeftGripPoint().y();
                                }
                              else if (_mousePos.x() > (box->mapToScene(box->boundingRect().bottomRight()).x()
                                                        - BasicBox::RESIZE_TOLERANCE)) {
                                  endX = box->getRightGripPoint().x();
                                  endY = box->getRightGripPoint().y();
                                }
                              else {
                                  endX = _mousePos.x();
                                  endY = _mousePos.y();
                                }
                              if (_relationBoxFound) {
                                  painter->drawEllipse(QPointF(endX, endY), arrowSize / 2., arrowSize / 2.);
                                }
                            }
                          else {
                              endX = _mousePos.x();
                              endY = _mousePos.y();
                              painter->drawEllipse(endX, endY - arrowSize / 2., arrowSize, arrowSize);
                              painter->drawLine(QPointF(endX, endY - arrowSize / 2.),
                                                QPointF(endX + arrowSize, endY + arrowSize / 2.));
                              painter->drawLine(QPointF(endX, endY + arrowSize / 2.),
                                                QPointF(endX + arrowSize, endY - arrowSize / 2.));
                            }
                        }
                      else {
                          endX = _mousePos.x();
                          endY = _mousePos.y();
                        }
                      if (fabs(startX - endX) >= 2 * arrowSize) {
                          if (startX <= endX) {
                              painterPath.lineTo(startX + arrowSize, startY);
                            }
                          else {
                              painterPath.lineTo(startX - arrowSize, startY);
                            }
                        }

                      painterPath.quadTo((startX + endX) / 2., startY, (startX + endX) / 2., (startY + endY) / 2.);
                      if (fabs(startX - endX) >= 2 * arrowSize) {
                          if (startX <= endX) {
                              painterPath.quadTo((startX + endX) / 2., endY, endX - arrowSize, endY);
                            }
                          else {
                              painterPath.quadTo((startX + endX) / 2., endY, endX + arrowSize, endY);
                            }
                        }
                      else {
                          painterPath.quadTo((startX + endX) / 2., endY, endX, endY);
                        }

                      //Relation
                      painter->save();
                      QPen localPen;
                      localPen.setWidth(BasicBox::LINE_WIDTH);
                      localPen.setStyle(Qt::DashDotLine);
                      painter->setPen(localPen);
                      painter->drawPath(painterPath);
                      painter->restore();
                      painterPath = QPainterPath();
                      painterPath.moveTo(endX, endY);
                      painterPath.lineTo(endX - arrowSize, endY - (arrowSize / 2.));
                      painterPath.lineTo(endX - arrowSize, endY + (arrowSize / 2.));
                      painterPath.lineTo(endX, endY);

                      painter->fillPath(painterPath, QColor(60, 60, 60));
                    }
                }
              else {
#ifdef DEBUG
                  std::cerr << "MaquetteScene::drawForeground : box with NO_ID found" << std::endl;
#endif
                }
            }
          else {
#ifdef DEBUG
              std::cerr << "MaquetteScene::drawForeground : not clicked" << std::endl;
#endif
            }
        }
      else {
#ifdef DEBUG
          std::cerr << "MaquetteScene::drawForeground : not in relation mode" << std::endl;
#endif
        }
    }
}

void
MaquetteScene::setCurrentMode(int inter, BoxCreationMode box)
{
  _currentInteractionMode = inter;
  if (inter == SELECTION_MODE || inter == RELATION_MODE) {
      _view->setDragMode(QGraphicsView::RubberBandDrag);
    }
  else if ((inter != CREATION_MODE && box == NB_MODE)
           || (inter == CREATION_MODE && box != NB_MODE)) {
      _currentBoxMode = box;
      _view->setDragMode(QGraphicsView::NoDrag);
    }
  else {
      _view->setDragMode(QGraphicsView::NoDrag);
    }
}

int
MaquetteScene::currentMode()
{
  if (_currentInteractionMode == CREATION_MODE && _currentBoxMode == NB_MODE) {
      setCurrentMode(SELECTION_MODE);
    }
  return _currentInteractionMode;
}

int
MaquetteScene::currentBoxMode()
{
  return _currentBoxMode;
}

bool
MaquetteScene::noBoxSelected()
{
  return selectedItems().isEmpty();
}

QGraphicsItem *
MaquetteScene::getSelectedItem()
{
  if (noBoxSelected()) {
      return NULL;
    }
  else if (selectedItems().size() > 1) {
      return NULL;
    }
  else {
      return selectedItems().first();
    }
}

void
MaquetteScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsScene::mouseDoubleClickEvent(event);
}

bool
MaquetteScene::subScenarioMode(QGraphicsSceneMouseEvent *mouseEvent)
{
  if (getSelectedItem() != NULL && itemAt(mouseEvent->scenePos()) != 0) {
      return(getSelectedItem()->type() == PARENT_BOX_TYPE && static_cast<BasicBox*>(getSelectedItem())->currentText() == BasicBox::SUB_SCENARIO_MODE_TEXT && static_cast<BasicBox*>(getSelectedItem())->boxBody().contains(mouseEvent->pos()) && itemAt(mouseEvent->scenePos())->cursor().shape() == Qt::ArrowCursor);
    }
  else {
      return false;
    }
}

void
MaquetteScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  QGraphicsScene::mousePressEvent(mouseEvent);
  _clicked = true;
 
  if (paused())
      stopAndGoToCurrentTime();

  if (_tempBox) {
      removeItem(_tempBox);
      _tempBox = NULL;
    }
  _savedInteractionMode = _currentInteractionMode;
  _savedBoxMode = _currentBoxMode;

  if (mouseEvent->modifiers() == Qt::ShiftModifier) {
      setCurrentMode(SELECTION_MODE);
    }
  else if (noBoxSelected() || subScenarioMode(mouseEvent)) {
      setCurrentMode(CREATION_MODE);
    }  
  else {
      setCurrentMode(SELECTION_MODE);
    }

  if (itemAt(mouseEvent->scenePos()) != 0) {
      if (itemAt(mouseEvent->scenePos())->cursor().shape() == Qt::PointingHandCursor && _currentInteractionMode != TRIGGER_MODE) {
          setCurrentMode(TRIGGER_MODE);
        }
      if (itemAt(mouseEvent->scenePos())->cursor().shape() == Qt::CrossCursor && _currentInteractionMode != RELATION_MODE) {
          setCurrentMode(RELATION_MODE);
        }
    }

  switch (_currentInteractionMode) {
      case RELATION_MODE:
        _mousePos = mouseEvent->scenePos();
        break;

      case SELECTION_MODE:
        break;

      case TEXT_MODE:
        break;

      case TRIGGER_MODE:
        break;

      case CREATION_MODE:
        if (itemAt(mouseEvent->scenePos()) == 0) {
            if (resizeMode() == NO_RESIZE) {
                // Store the first pressed point
                _pressPoint = mouseEvent->scenePos();
                QPen pen(Qt::black);
                QBrush brush(Qt::NoBrush);

                // Add the temporary box to the scene
                _tempBox = addRect(QRectF(_pressPoint.x(), _pressPoint.y(), 0, 0), pen, brush);
              }
          }

        else if (getSelectedItem() != NULL ? getSelectedItem()->type() == PARENT_BOX_TYPE : false && subScenarioMode(mouseEvent)) {
            /// \todo Old TODO updated (by jC) : see why creation is possible in a parent box during resize mode
            if (resizeMode() == NO_RESIZE) {
                // Store the first pressed point
                _pressPoint = mouseEvent->scenePos();
                QPen pen(Qt::black);
                QBrush brush(Qt::NoBrush);

                // Add the temporary box to the scene
                _tempBox = addRect(QRectF(_pressPoint.x(), _pressPoint.y(), 0, 0), pen, brush);
              }
          }
        break;

      case BOX_EDIT_MODE:
        break;
    } 
}

void
MaquetteScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
  if (!_maquette->isExecutionOn()) {
      QGraphicsScene::mouseMoveEvent(mouseEvent);

      switch (_currentInteractionMode) {
          case RELATION_MODE:

            if (_clicked) {
                _mousePos = mouseEvent->scenePos();
                if (_relation->firstBox() != NO_ID) {
                    update();
                  }
                if (itemAt(mouseEvent->scenePos()) != 0) {
                    int type = itemAt(mouseEvent->scenePos())->type();
                    if (type == PARENT_BOX_TYPE) {
                        BasicBox *secondBox = static_cast<BasicBox*>(itemAt(mouseEvent->scenePos()));
                        if (mouseEvent->scenePos().x() < (secondBox->mapToScene(secondBox->boundingRect().topLeft()).x() + BasicBox::RESIZE_TOLERANCE) ||
                            mouseEvent->scenePos().x() > (secondBox->mapToScene(secondBox->boundingRect().bottomRight()).x() - BasicBox::RESIZE_TOLERANCE)) {
                            _relationBoxFound = true;
                            update();
                          }
                        else {
                            _relationBoxFound = false;
                          }
                      }
                    else {
                        _relationBoxFound = false;
                      }
                  }
              }
            break;

          case TEXT_MODE:
            break;

          case TRIGGER_MODE:
            break;

          case SELECTION_MODE:
            break;

          case CREATION_MODE:
            if (noBoxSelected() || subScenarioMode(mouseEvent)) {
                if (resizeMode() == NO_RESIZE && _tempBox) {
                    int upLeftX, upLeftY, width, height;

                    if (_pressPoint.x() < mouseEvent->scenePos().x()) {
                        upLeftX = _pressPoint.x();
                        width = mouseEvent->scenePos().x() - upLeftX;
                      }
                    else {
                        upLeftX = mouseEvent->scenePos().x();
                        width = _pressPoint.x() - upLeftX;
                      }
                    if (_pressPoint.y() < mouseEvent->scenePos().y()) {
                        upLeftY = _pressPoint.y();
                        height = mouseEvent->scenePos().y() - upLeftY;
                      }
                    else {
                        upLeftY = mouseEvent->scenePos().y();
                        height = _pressPoint.y() - upLeftY;
                      }

                    _tempBox->setRect(upLeftX, upLeftY, width, height);
                  }
              }
            break;

          case BOX_EDIT_MODE:
            break;
        }
    }
}

void
MaquetteScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
  QGraphicsScene::mouseReleaseEvent(mouseEvent);

  _releasePoint = mouseEvent->scenePos();
  _clicked = false;
  switch (_currentInteractionMode) {
      case RELATION_MODE:

        if (itemAt(mouseEvent->scenePos()) != 0) {
            int type = itemAt(mouseEvent->scenePos())->type();
            if (type == PARENT_BOX_TYPE) {

                BasicBox *secondBox = static_cast<BasicBox*>(itemAt(mouseEvent->scenePos()));
                BasicBox *firstBox = getBox(_relation->firstBox());

                if(mouseEvent->modifiers() == Qt::AltModifier){ //case conditional relation
                    std::cout<<"condition "<<firstBox->ID()<<" & "<<secondBox->ID()<<std::endl;
                    QList<BasicBox *> boxesToCondition;
                    boxesToCondition<<firstBox;
                    boxesToCondition<<secondBox;
                    conditionBoxes(boxesToCondition);
                }
                else{

                    if (mouseEvent->scenePos().x() < (secondBox->mapToScene(secondBox->boundingRect().topLeft()).x() + BasicBox::RESIZE_TOLERANCE)) {
                        setRelationSecondBox(secondBox->ID(), BOX_START);
                        addPendingRelation();
                        firstBox->setSelected(true);
                    }
                    else if (mouseEvent->scenePos().x() > (secondBox->mapToScene(secondBox->boundingRect().bottomRight()).x() - BasicBox::RESIZE_TOLERANCE)) {
                        setRelationSecondBox(secondBox->ID(), BOX_END);
                        addPendingRelation();
                        firstBox->setSelected(true);
                    }
                    else {
                        if (selectedItems().empty()) {
                        }
                        else {
                            selectionMoved();
                        }
                    }
                }
            }
            else {
                _relationBoxFound = false;
                delete _relation;
                _relation = new AbstractRelation;
              }
          }
        else {
            _relationBoxFound = false;
            delete _relation;
            _relation = new AbstractRelation;
          }
        break;

      case SELECTION_MODE:
        if (selectedItems().isEmpty()) {
            _editor->noBoxEdited(); /// \todo noBoxEdited() est un public slot. mieux vaux faire appel au mécanisme d'auto-connexion des signaux dans Qt (QMetaObject) que de le garder en attribut de classe pour éviter le couplage. (par jaime Chao)
          }

        break;

      case TRIGGER_MODE:
        break;

      case TEXT_MODE:
        if (itemAt(mouseEvent->scenePos()) == 0) {
            addComment(tr("Comment").toStdString(), mouseEvent->scenePos(), NO_ID);
          }
        break;

      case CREATION_MODE:
        if (!selectedItems().empty()) {
            _maquette->updateRelations();
          }
        if (resizeMode() == NO_RESIZE && _tempBox) {
            if (_releasePoint != _pressPoint && (abs(_releasePoint.x() - _pressPoint.x()) > MIN_BOX_WIDTH && abs(_releasePoint.y() - _pressPoint.y()) > MIN_BOX_HEIGHT)) {
                addBox(BoxCreationMode(_currentBoxMode));
              }

            else {
                if (selectedItems().empty()) {
                    _editor->noBoxEdited(); /// \todo noBoxEdited() est un public slot. mieux vaux faire appel au mécanisme d'auto-connexion des signaux dans Qt (QMetaObject) que de le garder en attribut de classe (couplage). (par jaime Chao)
                  }
              }
          }
        if (_tempBox) {
            removeItem(_tempBox);
            _tempBox = NULL;
          }
        break;
    }

  setCurrentMode(_savedInteractionMode, BoxCreationMode(_savedBoxMode));

//  update();
}

void
MaquetteScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
  _tempBox = NULL;
  _resizeBox = NO_ID;
  _relation->setFirstBox(NO_ID);
  _relation->setSecondBox(NO_ID);
  _relationBoxFound = false;

  _mousePos = QPointF(0., 0.);

  _clicked = false;
  _tempBox = NULL;

  QGraphicsScene::contextMenuEvent(event);
}

void
MaquetteScene::setResizeBox(unsigned int box)
{
  _resizeBox = box;
}

int
MaquetteScene::resizeMode() const
{
  return _resizeMode;
}

void
MaquetteScene::setResizeMode(int resizeMode)
{
  _resizeMode = resizeMode;
}

BasicBox*
MaquetteScene::getBox(unsigned int ID)
{
  return _maquette->getBox(ID);
}

Relation*
MaquetteScene::getRelation(unsigned int ID)
{
  return _maquette->getRelation(ID);
}

void
MaquetteScene::cutBoxes()
{
  copyBoxes(true);
}

void
MaquetteScene::copyBoxes(bool erasing)
{
    QPointF topLeft(FLT_MAX, FLT_MAX);
    QPointF bottomRight(FLT_MIN, FLT_MIN);

    _toCopy.clear();
    _boxesToCopy.clear();

    QList<QGraphicsItem *> selected = selectedItems();
    QList<QGraphicsItem *>::iterator it;
    for (it = selected.begin(); it != selected.end(); it++) {
        switch ((*it)->type()) {

        case PARENT_BOX_TYPE:
        {
            AbstractParentBox *curBox = static_cast<AbstractParentBox*>(static_cast<ParentBox*>(*it)->abstract());

            AbstractParentBox *boxToCopy =new AbstractParentBox(*curBox);

            _boxesToCopy[static_cast<ParentBox*>(*it)->ID()] = boxToCopy;
            if (erasing) {
                removeBox(static_cast<ParentBox*>(*it)->ID());
            }
            break;
        }

        case RELATION_TYPE:
            _toCopy.push_back(new AbstractRelation(*static_cast<AbstractRelation*>(static_cast<Relation*>(*it)->abstract())));
            if (erasing) {
                removeRelation(static_cast<Relation*>(*it)->ID());
            }
            break;

        case COMMENT_TYPE:
            _toCopy.push_back(new AbstractComment(*static_cast<AbstractComment*>(static_cast<Comment*>(*it)->abstract())));
            if (erasing) {
                removeComment(static_cast<Comment*>(*it));
            }
            break;

        case TRIGGER_POINT_TYPE:
            _toCopy.push_back(new AbstractTriggerPoint(*static_cast<AbstractTriggerPoint*>(static_cast<TriggerPoint*>(*it)->abstract())));
            if (erasing) {
                removeTriggerPoint(static_cast<TriggerPoint*>(*it)->ID());
            }
            break;

        default:
            std::cerr << "MaquetteScene::copyBoxes : Unhandled item during copy" << std::endl;
            break;
        }
        if (!erasing) {
            topLeft = QPointF(std::min((*it)->mapToScene((*it)->boundingRect().topLeft()).x(), topLeft.x()),
                              std::min((*it)->mapToScene((*it)->boundingRect().topLeft()).y(), topLeft.y()));
            bottomRight = QPointF(std::max((*it)->mapToScene((*it)->boundingRect().bottomRight()).x(), bottomRight.x()),
                                  std::max((*it)->mapToScene((*it)->boundingRect().bottomRight()).y(), bottomRight.y()));
        }
    }
    if (topLeft.x() != FLT_MAX && topLeft.y() != FLT_MAX &&
            bottomRight.x() != FLT_MIN && bottomRight.y() != FLT_MIN) {
        _copySize = bottomRight - topLeft;
    }
    else {
        if (!erasing) {
            std::cerr << "MaquetteScene::copyBoxes : the size of the copy could not be determined" << std::endl;
        }
        _copySize = QPointF(0., 0.);
    }

}

void
MaquetteScene::pasteBoxes(const QPointF &position)
{
  _copyPos = position;
  pasteBoxes();

  setModified(true);
}

void
MaquetteScene::pasteBoxes()
{
  BasicBox* newBox;
  string name = "";

  map<unsigned int, unsigned int> IDMap;

  map<unsigned int, AbstractBox*>::iterator boxIt;

  for (boxIt = _boxesToCopy.begin(); boxIt != _boxesToCopy.end(); ++boxIt) {
      int type = boxIt->second->type();
      AbstractBox *boxToCopy = boxIt->second;
      unsigned int newID = NO_ID;
      AbstractBox *absCopyBox = NULL;
      switch (type) {
          case ABSTRACT_PARENT_BOX_TYPE:
          {
            AbstractParentBox *abParentBox = static_cast<AbstractParentBox*>(boxToCopy);
            abParentBox->setTopLeft(QPointF(abParentBox->topLeft().x(), abParentBox->topLeft().y()) + _copySize);            

            if (abParentBox->mother() != ROOT_BOX_ID) {
                map<unsigned int, unsigned int>::iterator it;
                if ((it = IDMap.find(abParentBox->mother())) != IDMap.end()) {
                    abParentBox->setMother(it->second);
                  }
              }
            newID = addParentBox(*abParentBox);
            absCopyBox = abParentBox;
            break;
          }

          default:
          {
            std::cerr << "MaquetteScene::pasteBoxes : Unrecognized abstract box type" << std::endl;
            break;
          }
        }
      if (newID != NO_ID && absCopyBox != NULL) {
          IDMap[absCopyBox->ID()] = newID;
#ifdef DEBUG
          std::cerr << "MaquetteScene::pasteBoxes : assigning ID " << newID << " to copy of box " << absCopyBox->ID() << std::endl;
#endif
          newBox = getBox(newID);
          newBox->setSelected(false);
          name = QString(newBox->name()).toStdString();
          if ((name.find(tr("- Copy").toStdString())) == std::string::npos) {
              name += tr(" - Copy").toStdString();
            }
          newBox->setName(QString::fromStdString(name));          
          newBox->setPos(newBox->getCenter());

          newBox->setStartMessages(absCopyBox->startMessages());
          newBox->setEndMessages(absCopyBox->endMessages());

          newBox->setSelected(true);
          newBox->centerWidget();
          newBox->update();
        }
      else {
          std::cerr << "MaquetteScene::pasteBoxes : Unvalid assigned box ID" << std::endl;
        }
    }

  vector<Abstract*>::iterator it;
  for (it = _toCopy.begin(); it != _toCopy.end(); it++) {
      if ((*it)->type() == ABSTRACT_RELATION_TYPE) {
          AbstractRelation *relation = static_cast<AbstractRelation*>(*it);
          if (IDMap.find(relation->firstBox()) != IDMap.end() && IDMap.find(relation->secondBox()) != IDMap.end()) {
#ifdef DEBUG
              std::cerr << "MaquetteScene::pasteBoxes : relation was between " << relation->firstBox() << " and " << relation->secondBox() << std::endl;
              std::cerr << "MaquetteScene::pasteBoxes : relation is now between " << IDMap[relation->firstBox()] << " and " << IDMap[relation->secondBox()] << std::endl;
#endif
              relation->setFirstBox(IDMap[relation->firstBox()]);
              relation->setSecondBox(IDMap[relation->secondBox()]);
              relation->setID(NO_ID);
              addRelation(*relation);
            }
        }
      else if ((*it)->type() == ABSTRACT_COMMENT_TYPE) {
          AbstractComment *comment = static_cast<AbstractComment*>(*it);
          std::cerr << "Comment found while pasting : " << IDMap[comment->ID()] << std::endl;
          if (comment->ID() != NO_ID) {
              if (IDMap.find(comment->ID()) != IDMap.end()) {
                  std::cerr << "New ID for Comment : " << IDMap[comment->ID()] << std::endl;
                  comment->setID(IDMap[comment->ID()]);
                  BasicBox *comBox = NULL;
                  if ((comBox = getBox(IDMap[comment->ID()])) != NULL) {
                      comBox->addComment(*comment);
                    }
                  else {
                      std::cerr << "MaquetteScene::pasteBoxes : no box found for comment" << std::endl;
                    }
                }
            }
          else {
              addComment(comment->text(), comment->pos(), NO_ID);
            }
          delete comment;
        }
      else if ((*it)->type() == ABSTRACT_TRIGGER_POINT_TYPE) {
          AbstractTriggerPoint *triggerPoint = static_cast<AbstractTriggerPoint*>(*it);
          if (triggerPoint->boxID() != NO_ID) {
              if (IDMap.find(triggerPoint->boxID()) != IDMap.end()) {
                  getBox(IDMap[triggerPoint->boxID()])->addTriggerPoint(*triggerPoint);
                }
            }
          delete triggerPoint;
        }
      else {
          std::cerr << "MaquetteScene::pasteBox::Unrecognized type while pasting" << std::endl;
        }
    }
  selectionMoved();
  _toCopy.clear();
  _copySize = QPointF(0, 0);
  setModified(true);
}

void
MaquetteScene::clear()
{
  selectAll();
  removeSelectedItems();
  changeTimeOffset(0);
  setModified(true);
}

bool
MaquetteScene::areRelated(unsigned int ID1, unsigned int ID2)
{
  return _maquette->areRelated(ID1, ID2);
}

vector<unsigned int>
MaquetteScene::getRelationsIDs(unsigned int entityID)
{
  return _maquette->getRelationsIDs(entityID);
}

std::string
MaquetteScene::sequentialName(const string &name)
{
  const unsigned int curID = _maquette->sequentialID() + 1;

  std::stringstream tmp;
  tmp << curID;

  return name + "." + tmp.str();
}

QString
MaquetteScene::getStringFromRelation(unsigned int relID)
{
  Relation *rel = getRelation(relID);
  return rel->toString();
}

Comment *
MaquetteScene::addComment(const std::string &comment, const QPointF &position, unsigned int boxID)
{
  Comment *textItem = new Comment(comment, boxID, this);
  textItem->setPos(position);

  setModified(true);

  return textItem;
}

void
MaquetteScene::removeComment(Comment *comment)
{
  unsigned int ID = static_cast<AbstractComment*>(comment->abstract())->ID();
  if (ID != NO_ID) {
      if (getBox(ID) != NULL) {
          getBox(ID)->removeComment();
        }
    }

  removeItem(comment);
  delete comment;
}

int
MaquetteScene::addTriggerPoint(unsigned int boxID, BoxExtremity extremity, const string &message)
{
  int trgID = NO_ID;

  if ((trgID = _maquette->addTriggerPoint(boxID, extremity, message)) > NO_ID) {
      displayMessage(tr("New trigger point added.").toStdString(), INDICATION_LEVEL);
      setModified(true);

      return trgID;
    }
  else {
      std::stringstream ss;
      ss << tr("Cannot add trigger point for box ").toStdString() << getBox(boxID)->name().toStdString() << tr(" at ").toStdString() << extremity;
      std::cerr << tr("MaquetteScene::Cannot add trigger point for box ").toStdString() << getBox(boxID)->name().toStdString() << tr(" at ").toStdString() << extremity << std::endl;
      displayMessage(ss.str(), INDICATION_LEVEL);
      return trgID;
    }

  return RETURN_ERROR;
}

TriggerPoint *
MaquetteScene::getTriggerPoint(unsigned int trgID)
{
  return _maquette->getTriggerPoint(trgID);
}

void
MaquetteScene::removeTriggerPoint(unsigned int trgID)
{
  TriggerPoint *trgPnt = getTriggerPoint(trgID);
  BasicBox *box;
  if (trgPnt != NULL) {
      box = getBox(trgPnt->boxID());
      if (box != NULL) {
          box->removeTriggerPoint(trgPnt->boxExtremity());
        }
      _triggersQueueList->removeAll(trgPnt);
//      removeItem(trgPnt);
      _maquette->removeTriggerPoint(trgID);      
    }
}

void
MaquetteScene::trigger(TriggerPoint *triggerPoint)
{
  _maquette->trigger(triggerPoint);
  removeFromTriggerQueue(triggerPoint);
  triggerPoint->setSelected(false);
}

void
MaquetteScene::triggerNext()
{
   TriggerPoint *triggerPoint = triggersQueueList()->first();
  _maquette->trigger(triggerPoint);
  removeFromTriggerQueue(triggerPoint);
  triggerPoint->setSelected(false);
}

bool
MaquetteScene::setTriggerPointMessage(unsigned int trgID, const string &message)
{
  return _maquette->setTriggerPointMessage(trgID, message);
}

unsigned int
MaquetteScene::findMother(const QPointF &topLeft, const QPointF &size)
{
#ifdef DEBUG
  std::cerr << "MaquetteScene::findMother : child coords : [" << topLeft.x() << ";" << topLeft.y()
            << "] / [" << size.x() << ";" << size.y() << "]" << std::endl;
#endif
  map<unsigned int, ParentBox*> parentBoxes = _maquette->parentBoxes(); /// \todo Mieux vaut parcourir les ParentBoxes contenus dans la scene, pas aller les chercher dans Maquette car on perd l'intérêt d'utiliser le Graphics View Framework. (par jaime Chao)
#ifdef DEBUG
  std::cerr << "MaquetteScene::findMother : parentBoxes size : " << parentBoxes.size() << std::endl;
#endif
  map<unsigned int, ParentBox*>::iterator it;
  unsigned int motherID = ROOT_BOX_ID;
  float motherZValue = std::numeric_limits<float>::min();
  QRectF childRect = QRectF(topLeft, QSize(size.x(), size.y())); /// \todo old TODO updated (by jC)
  for (it = parentBoxes.begin(); it != parentBoxes.end(); ++it) {
      QRectF mRect = QRectF(it->second->getTopLeft(), QSize(it->second->getSize().x(), it->second->getSize().y()));
#ifdef DEBUG
      std::cerr << "MaquetteScene::findMother : possible mother coords : [" << mRect.topLeft().x() << ";" << mRect.topLeft().y()
                << "] / [" << mRect.size().width() << ";" << mRect.size().height() << "]" << std::endl;
#endif
      if (mRect.contains(childRect) && !childRect.contains(mRect)) {
#ifdef DEBUG
          std::cerr << "MaquetteScene::findMother : newMother : " << it->first << std::endl;
#endif
          motherID = it->first;
          motherZValue = it->second->zValue();
        }
    }
  return motherID;
}

void
MaquetteScene::addBox(BoxCreationMode mode)
{
  unsigned int boxID = NO_ID;
  if (abs(_pressPoint.x() - _releasePoint.x()) > (MS_PRECISION / MS_PER_PIXEL)) {
      switch (mode) {
          case PB_MODE:
            boxID = addParentBox();
            update();
            break;

          default:
            std::cerr << "MaquetteScene :: Unknown Box Creation Mode" << std::endl;

            /// \todo Old TODO updated (by jC) : check this
            boxID = addParentBox();
            update();
            break;
        }
    }
  if (boxID != NO_ID) {
      setAttributes(static_cast<AbstractBox*>(getBox(boxID)->abstract()));
    }
}

unsigned int
MaquetteScene::addParentBox(unsigned int ID)
{
  if (ID != NO_ID) {
      if (_maquette->getBox(ID)->type() == PARENT_BOX_TYPE) { /// \todo Le type des GraphicsItem est mal utilisé !! Voir http://qt-project.org/doc/qt-4.8/qgraphicsitem.html#UserType-var (par jaime Chao)
          ParentBox *parentBox = static_cast<ParentBox*>(_maquette->getBox(ID));
          parentBox->setPos(parentBox->getCenter());
          parentBox->update();
          addItem(parentBox);

          _currentZValue++;

          setModified(true);

          return ID;
        }
    }
  return NO_ID;
}

unsigned int
MaquetteScene::addParentBox(const QPointF &topLeft, const QPointF &bottomRight, const string &name)
{
 /// \todo verify if GraphicsScene provide this method. (par jaime Chao)
  unsigned int motherID = findMother(topLeft, QPointF(std::fabs(bottomRight.x() - topLeft.x()),
                                                      std::fabs(bottomRight.y() - topLeft.y())));

  /// Searching the mother of parentBox
  ParentBox *parentBox = NULL;
  if (motherID != ROOT_BOX_ID && motherID != NO_ID) {
      parentBox = static_cast<ParentBox*>(getBox(motherID));
    }
  else {
      motherID = ROOT_BOX_ID;
    }
  unsigned int newBoxID = _maquette->addParentBox(topLeft, bottomRight, name, motherID);

  ParentBox *newBox = static_cast<ParentBox*>(getBox(newBoxID));

  if (newBox != NULL && parentBox != NULL) {
      if (newBox->mother() != motherID) {           // Not yet assigned
          parentBox->addChild(newBoxID);
          newBox->setMother(motherID);
        }
    }
  if (newBoxID != NO_ID) {
      newBox->setPos(newBox->getCenter());
      newBox->update();
      addItem(newBox);

      _currentZValue++;
    }

  setModified(true);

  return newBoxID;
}

unsigned int
MaquetteScene::addParentBox(const AbstractParentBox &box)
{
  return addParentBox(box.topLeft(), box.bottomRight(), box.name());
}

unsigned int
MaquetteScene::addParentBox()
{
  bool ok;
  QString name;
  while (name.isEmpty()) {
      name = QString::fromStdString(sequentialName(tr("Box").toStdString()));
      ok = true;
      if (!ok) {
          return NO_ID;
        }
      if (name.isEmpty()) {
          QMessageBox::warning(_view, tr("Warning"), tr("Please Enter a Name"));
        }
    }

  return addParentBox(_pressPoint, _releasePoint, name.toStdString());
}

int
MaquetteScene::addRelation(const AbstractRelation &abstractRel)
{
  int ret = _maquette->addRelation(abstractRel);
  if (ret > NO_ID) {
      unsigned int relationID = (unsigned int)ret;
      Relation *newRel = _maquette->getRelation(relationID);
      if (newRel != NULL) {
          newRel->setID(relationID);
          newRel->setPos(newRel->getCenter());
          newRel->update();

//			addItem(newRel);
          BasicBox *box = NULL;
          if ((box = getBox(abstractRel.firstBox())) != NULL) {
              box->addRelation(abstractRel.firstExtremity(), newRel);
            }
          if ((box = getBox(abstractRel.secondBox())) != NULL) {
              box->addRelation(abstractRel.secondExtremity(), newRel);
            }
          newRel->updateFlexibility();

          ret = SUCCESS;
        }
    }

#ifdef DEBUG
  std::cerr << "MaquetteScene::addRelation : returns " << ret << std::endl;
#endif

  switch (ret) {
      case SUCCESS:
        displayMessage(tr("New relation created").toStdString(), INDICATION_LEVEL);
        break;

      case NO_MODIFICATION:
        displayMessage(tr("A relation between these two points already exists").toStdString(), INDICATION_LEVEL);
        break;

      case RETURN_ERROR:
        displayMessage(tr("Relation is not compatible with the system").toStdString(), INDICATION_LEVEL);
        break;

      case ARGS_ERROR:
        displayMessage(tr("Boxes can not be identified").toStdString(), WARNING_LEVEL);
        break;

      default:
        displayMessage(tr("Unknown relation creation error").toStdString(), WARNING_LEVEL);
        break;
    }

  setModified(true);
  update();

  return ret;
}

int
MaquetteScene::addPendingRelation()
{
  int ret = addRelation(*_relation);

  _relationBoxFound = false;
  _relation = new AbstractRelation();
  return ret;
}

void
MaquetteScene::changeRelationBounds(unsigned int relID, const float &length, const float &minBound, const float &maxBound)
{
  Relation *rel = getRelation(relID);
  if (rel != NULL) {      
      _maquette->changeRelationBounds(relID, minBound, maxBound);
      rel->changeBounds(minBound, maxBound);
      if (length != NO_LENGTH) {
          AbstractRelation *abRel = static_cast<AbstractRelation*>(rel->abstract());
          float oldLength = abRel->length();
          BasicBox *secondBox = getBox(abRel->secondBox());
          if (secondBox != NULL) {              
              secondBox->moveBy(length - oldLength, 0.);
              vector<unsigned int> boxMoved;
              boxMoved.push_back(secondBox->ID());
              boxesMoved(boxMoved);
            }
        }
    }
}

bool
MaquetteScene::addInterval(unsigned int ID1, unsigned int ID2, int value, int tolerance)
{
  setModified(true);

  return _maquette->addInterval(ID1, ID2, value, tolerance);
}

void
MaquetteScene::removeRelation(unsigned int relID)
{
  Relation *rel = getRelation(relID);
  if (rel != NULL) {
      AbstractRelation *abstract = static_cast<AbstractRelation*>(rel->abstract());

      BasicBox *box = getBox(abstract->firstBox());
      if (box != NULL) {
          box->removeRelation(abstract->firstExtremity(), abstract->ID());
        }
      box = getBox(abstract->secondBox());
      if (box != NULL) {
          box->removeRelation(abstract->secondExtremity(), abstract->ID());
        }

      removeItem(getRelation(relID));
      _maquette->removeRelation(relID);

      setModified(true);
    }
}

void
MaquetteScene::removeConditionalRelation(ConditionalRelation *condRel)
{
    if(condRel != NULL)
    {
        QList<BasicBox *>::iterator     it;
        QList<BasicBox *>               boxes = condRel->getBoxes();
        BasicBox                        *curBox;

        for(it = boxes.begin() ; it!=boxes.end() ; it++)
        {
            curBox = *it;
            curBox->removeConditionalRelation(condRel);
        }

        removeItem(condRel);
        setModified(true);
    }
}


void
MaquetteScene::setRelationFirstBox(unsigned int ID, BoxExtremity extremumType)
{
  _relation->setFirstBox(ID);
  _relation->setFirstExtremity(extremumType);
}

void
MaquetteScene::relationBoxFound()
{
  _relationBoxFound = true;
}

void
MaquetteScene::setRelationSecondBox(unsigned int ID, BoxExtremity extremumType)
{
  _relation->setSecondBox(ID);
  _relation->setSecondExtremity(extremumType);
  _relationBoxFound = true;
}

void
MaquetteScene::boxResized()
{
  BasicBox* resizeBox = getBox(_resizeBox);
  Coords coord;
  coord.topLeftX = resizeBox->relativeBeginPos();
  coord.topLeftY = resizeBox->getTopLeft().y();
  coord.sizeX = std::max((float)10., resizeBox->width());

  coord.sizeY = std::max((float)10., resizeBox->height());
  if (_maquette->updateBox(resizeBox->ID(), coord)) {
      update();
      setModified(true);
    }
}

void
MaquetteScene::selectionMoved()
{
  for (int i = 0; i < selectedItems().size(); i++) {
      QGraphicsItem *curItem = selectedItems().at(i);
      int type = curItem->type();
      if (type == PARENT_BOX_TYPE) {
          BasicBox *curBox = static_cast<BasicBox*>(curItem);
          boxMoved(curBox->ID());
        }
    }
}

bool
MaquetteScene::boxMoved(unsigned int boxID)
{
//  std::cout<<"--- boxMoved ---"<<boxID<<std::endl;
  Coords coord;
  BasicBox * box = _maquette->getBox(boxID);
  if (box != NULL) {
      if (!box->hasMother()) {
          coord.topLeftX = box->mapToScene(box->boxRect().topLeft()).x();
//          std::cout<<"X = "<<coord.topLeftX* MaquetteScene::MS_PER_PIXEL<<std::endl;
        }
      else {
          coord.topLeftX = box->mapToScene(box->boxRect().topLeft()).x()
                           - getBox(box->mother())->beginPos();
        }
      coord.topLeftY = box->mapToScene(box->boxRect().topLeft()).y();
      coord.sizeX = box->boxRect().size().width();
//      std::cout<<"Y = "<<coord.sizeX* MaquetteScene::MS_PER_PIXEL<<std::endl;
      coord.sizeY = box->boxRect().size().height();
    }
  bool ret = _maquette->updateBox(boxID, coord);

  if (ret) {
      update();
      setModified(true);

/*		std::cerr << "Box top left coordinates : " << box->mapToScene(box->boundingRect().topLeft()).x() + box->boundingRect().size().width() << std::endl;
 *              std::cerr << "View right max coordinates :" << _view->sceneRect().topLeft().x() + _view->sceneRect().width() << std::endl;*/

      //if (box->mapToScene(box->boundingRect().topLeft()).x() + box->boundingRect().size().width() >= (_view->sceneRect().topLeft().x() + _view->sceneRect().width() - 100)) {
      //_view->fitInView(box,Qt::KeepAspectRatio);
      //}
    }

  return ret;
}

void
MaquetteScene::boxesMoved(const vector<unsigned int> &moved)
{
  for (unsigned int i = 0; i < moved.size(); i++) {
      boxMoved(moved[i]);
    }
}

void
MaquetteScene::removeBox(unsigned int boxID)
{
  BasicBox *box = getBox(boxID);
  if (box != NULL) {
      if (boxID == _editor->currentBox()) { /// \todo Uniquement changer cette méthode publique en slot pour pouvoir découpler MaquetteScene et AttributesEditor. (c'est l'unique appel utile de _editor dans MaquetteScene). (par jaime Chao)
          _editor->noBoxEdited(); /// \todo noBoxEdited() est un public slot. mieux vaux faire appel au mécanisme d'auto-connexion des signaux dans Qt (QMetaObject) que de le garder en attribut de classe (couplage). (par jaime Chao)
        }
      removeItem(box);

      if (box->type() == PARENT_BOX_TYPE) {
          ParentBox *pBox = static_cast<ParentBox*>(box);
          if (pBox != NULL) {
              map<unsigned int, BasicBox*> children = pBox->children();
              map<unsigned int, BasicBox*>::iterator child;
              for (child = children.begin(); child != children.end(); ++child) {
                  pBox->removeChild(child->first);
                  removeBox(child->first);
                }
            }
        }
      if (box->hasMother()) {
          BasicBox *mother = NULL;
          if ((mother = getBox(box->mother())) != NULL) {
              if (mother->type() == PARENT_BOX_TYPE) {
                  static_cast<ParentBox*>(mother)->removeChild(boxID);
                }
            }
        }

      box->removeComment();
      box->removeTriggerPoint(BOX_START);
      box->removeTriggerPoint(BOX_END);
      
      vector<unsigned int> removedRelations = _maquette->removeBox(boxID);
      for (vector<unsigned int>::iterator it = removedRelations.begin(); it != removedRelations.end(); it++) {
          removeRelation(*it);
        }
      
      delete box;
      setModified(true);
    }

  update();
}

bool
MaquetteScene::playing()
{    
  return _maquette->isExecutionOn() && !_maquette->isExecutionPaused();
}

bool
MaquetteScene::paused()
{
  return _maquette->isExecutionPaused();
}

void
MaquetteScene::updateStartingTime(int value)
{
  _startingValue = value;
}

void
MaquetteScene::setPlaying(unsigned int boxID, bool playing)
{
  BasicBox *box = getBox(boxID); /// \todo Besoin d'un cast qt explicite ! (par jaime Chao)
  map<unsigned int, BasicBox*>::iterator it;
  if ((it = _playingBoxes.find(boxID)) != _playingBoxes.end()) {
      it->second = box;
      it->second->update();
      if (!playing) {           // Playing ended, removing box from set
          _playingBoxes.erase(it);
        }
      else {
          std::cerr << "MaquetteScene::setPlaying : trying to start playing on playing box" << std::endl;
        }
    }
  else {
      if (playing) {
          if (box != NULL) {
              _playingBoxes[boxID] = box;
              box->update();
            }
        }
    }
}

void
MaquetteScene::updatePlayingBoxes()
{
  map<unsigned int, BasicBox*>::iterator it;

  for (it = _playingBoxes.begin(); it != _playingBoxes.end(); ++it) {
      it->second->update();

      //Recording curves
//      if(it->second->recording()){
//          QList<std::string> recMsgs = static_cast<AbstractBox *>(it->second->abstract())->messagesToRecord();
//          for(int i=0 ; i<recMsgs.size() ; i++)
//              it->second->updateCurve(recMsgs.at(i),true);
//      }
    }
}

void
MaquetteScene::playOrResume()
{
    displayMessage(tr("Playing ...").toStdString(), INDICATION_LEVEL);
    
    _maquette->setAccelerationFactor(_accelerationFactor);

    if (_maquette->isExecutionPaused())        
        _maquette->resumeExecution();    
    else
        _maquette->turnExecutionOn();

    _playThread->start();
    _startingValue = _maquette->getTimeOffset();
    
    emit(playModeChanged());
}

void
MaquetteScene::stopOrPause()
{
    if (!_maquette->isExecutionPaused()){
        _maquette->pauseExecution();
        emit(playModeChanged());
    }
    else {
        _playThread->quit();
        _maquette->turnExecutionOff();
        emit(playModeChanged());
        _playingBoxes.clear();        
    }        
}

void
MaquetteScene::stopAndGoToTimeOffset(unsigned int timeOffset)
{
    displayMessage(tr("Stopped").toStdString(), INDICATION_LEVEL);

    _playThread->quit();
    _maquette->stopPlayingAndGoToTimeOffset(timeOffset);    
    emit(playModeChanged());
    _playingBoxes.clear();            
}

void
MaquetteScene::stopAndGoToCurrentTime()
{
    displayMessage(tr("Stopped").toStdString(), INDICATION_LEVEL);
    
    _maquette->stopPlayingAndGoToCurrentTime();
    _playThread->quit();
    emit(playModeChanged());
    _playingBoxes.clear();           
}

void
MaquetteScene::setAccelerationFactor(double value)
{
  _maquette->setAccelerationFactor(value);
  _accelerationFactor = value;
}

void
MaquetteScene::speedChanged(double value)
{
  setAccelerationFactor(value);
  emit(accelerationValueChanged(value));
}

void
MaquetteScene::stopAndGoToStart()
{
    displayMessage(tr("Stopped and go to start").toStdString(), INDICATION_LEVEL);
    
    _maquette->setAccelerationFactor(1.);
    emit(accelerationValueChanged(1.));
    _playThread->quit();
    _playingBoxes.clear();
    _maquette->stopPlayingAndGoToStart();

    //send root box start messages
    std::vector<std::string> startCue = _maquette->getBox(ROOT_BOX_ID)->getStartMessages();
    for(int i=0; i<startCue.size(); i++)
        sendMessage(startCue.at(i));

    update();
    
    emit(playModeChanged());
}

void
MaquetteScene::removeFromTriggerQueue(TriggerPoint *trigger)
{
  _triggersQueueList->removeAll(trigger);
}

void
MaquetteScene::removeSelectedItems()
{
  QList<QGraphicsItem *> toRemove = selectedItems();
  map<unsigned int, BasicBox*> boxesToRemove;
  for (QList<QGraphicsItem *>::iterator it = toRemove.begin(); it != toRemove.end(); it++) {
      int type = (*it)->type();
      if (type == PARENT_BOX_TYPE) {
          boxesToRemove[static_cast<BasicBox*>(*it)->ID()] = static_cast<BasicBox*>(*it);
        }
      else if ((*it)->type() == RELATION_TYPE) {
          removeRelation(static_cast<Relation*>(*it)->ID());
        }
      else if ((*it)->type() == COMMENT_TYPE) {
          removeComment(static_cast<Comment*>(*it));
        }
      else if ((*it)->type() == TRIGGER_POINT_TYPE) {
          removeTriggerPoint(static_cast<TriggerPoint*>(*it)->ID());
        }
      else if ((*it)->type() == CONDITIONAL_RELATION_TYPE) {
          removeConditionalRelation(static_cast<ConditionalRelation*>(*it));
        }
    }
  map<unsigned int, BasicBox*>::iterator boxIt;
  for (boxIt = boxesToRemove.begin(); boxIt != boxesToRemove.end(); ++boxIt) {
      removeBox(boxIt->first);
    }
  setModified(true);
}

void
MaquetteScene::timeEndReached()
{    
    _maquette->stopPlayingAndGoToStart();
    
    update();
    
    emit(playModeChanged());
}

bool
MaquetteScene::documentModified() const
{
  return _modified;
}

void
MaquetteScene::setModified(bool modified)
{
  _modified = modified;
}

void
MaquetteScene::save(const string &fileName)
{
  _maquette->save(fileName);
  setModified(false);
}

void
MaquetteScene::load(const string &fileName)
{
  _maquette->load(fileName);
  setModified(false);
  updateBoxesWidgets();  
}

void
MaquetteScene::updateBoxesWidgets()
{
  std::map<unsigned int, BasicBox*>::iterator it;
  std::map<unsigned int, BasicBox*> boxes = _maquette->getBoxes();
  for (it = boxes.begin(); it != boxes.end(); it++) {
      unsigned int boxID = it->first;
      static_cast<AbstractBox*>(getBox(boxID)->abstract())->clearMessages();
      if (boxID != NO_ID) {
          setAttributes(static_cast<AbstractBox*>(getBox(boxID)->abstract()));
        }
    }
}

void
MaquetteScene::addToTriggerQueue(TriggerPoint *trigger)
{
  if (!_triggersQueueList->contains(trigger)) {
      int i;
      qreal trgPosition = trigger->pos().x();

      for (i = 0; i < _triggersQueueList->size(); i++) {
          qreal itPosition = _triggersQueueList->at(i)->pos().x();
          if (trgPosition <= itPosition) {
              break;
            }
        }
      _triggersQueueList->insert(i, trigger);
    }
}

void
MaquetteScene::verticalScroll(int value)
{
  _timeBar->move(0, value);
  _view->repaint();
}

void
MaquetteScene::setMaxSceneWidth(float maxSceneWidth){
  _maxSceneWidth = maxSceneWidth;
}

float
MaquetteScene::getMaxSceneWidth(){
  return _maxSceneWidth;
}

void
MaquetteScene::conditionBoxes(QList<BasicBox *> boxesToCondition)
{
    QList<BasicBox *>::iterator     it = boxesToCondition.begin();
    BasicBox                        *box,
                                    *earliestBox = *it;
    unsigned int                    earliestDate = earliestBox->date();
    bool                            conditionalRelationFound = false;
    ConditionalRelation             *condRel;


    //Check if all boxes have a trigger point on start and force to move to the same date.
    //Check if boxes have to be simply attached to an existing conditional relation, else create a new one.
    for(it ; it!=boxesToCondition.end() ; it++)
    {
        box = *it;

        if(!box->hasTriggerPoint(BOX_START)) //Force trigger point creation
            box->addTriggerPoint(BOX_START);

        if(box->attachedToConditionalRelation()) // Check if a conditional relation is already attached to a box, to create or not a new one.
        {
            conditionalRelationFound = true;
            condRel = box->getConditionalRelations().first();
        }

        //Find the earliest box
        if(box->date()<earliestDate){
            earliestBox = box;
            earliestDate = box->date();
        }
    }

    //Force boxes to move to the earliest box date
    for(it=boxesToCondition.begin() ; it!=boxesToCondition.end() ; it++)
    {
        box = *it;
        box->moveBy((qreal)(earliestDate/MS_PER_PIXEL) -(qreal)(box->date() / MS_PER_PIXEL), 0.);
        boxMoved(box->ID());
    }

    if(conditionalRelationFound) //just attach boxes to existing relation
    {
        std::cout<<"MaquetteScene: existing relation -> ATTACH"<<std::endl;
        condRel->attachBoxes(boxesToCondition);
    }
    else //create a new one
    {
        std::cout<<"MaquetteScene: no existing relation -> CREATE"<<std::endl;
        condRel = new ConditionalRelation(boxesToCondition, this);
    }

    //add conditional relation to each box
    for(it = boxesToCondition.begin() ; it!=boxesToCondition.end() ; it++)
        (*it)->addConditionalRelation(condRel);
}

void
MaquetteScene::unselectAll(){
    QList<QGraphicsItem *> items = selectedItems();
    QList<QGraphicsItem *>::iterator it;
    for(it = items.begin(); it != items.end() ; it++)
        (*it)->setSelected(false);
}
