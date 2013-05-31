/*
 * Copyright: LaBRI / SCRIME
 *
 * Authors: Luc Vercellin and Bruno Valeze (08/03/2010)
 *
 * luc.vercellin@labri.fr
 *
 * This software is a computer program whose purpose is to provide
 * notation/composition combining synthesized as well as recorded
 * sounds, providing answers to the problem of notation and, drawing,
 * from its very design, on benefits from state of the art research
 * in musicology and sound/music computing.
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
#include "BasicBox.hpp"
#include "MaquetteScene.hpp"
#include "Maquette.hpp"
#include "AbstractBox.hpp"
#include "AbstractComment.hpp"
#include "AbstractCurve.hpp"
#include "AbstractTriggerPoint.hpp"
#include "Comment.hpp"
#include "TriggerPoint.hpp"
#include "TextEdit.hpp"
#include "MainWindow.hpp"
#include "Relation.hpp"
#include "CurveWidget.hpp"
#include "BoxCurveEdit.hpp"

#include <algorithm>
#include <iostream>

#include <QGraphicsView>
#include <QPainter>
#include <QCursor>
#include <QMenu>
#include <QBrush>
#include <QColor>
#include <QEvent>
#include <QGraphicsSceneHoverEvent>
#include <QColorDialog>
#include <QGraphicsTextItem>
#include <QKeyEvent>
#include <QString>
#include <QTranslator>
#include <QGraphicsWidget>
#include <QGraphicsLayout>
#include <QGridLayout>
#include <QToolTip>
#include <QAbstractItemView>
#include <QStyleOptionViewItem>

using std::string;
using std::vector;
using std::map;

/// \todo On pourrait les instancier directement dans le header avec leurs définitions.
const int BasicBox::COMBOBOX_HEIGHT = 25;
const int BasicBox::COMBOBOX_WIDTH = 120;
const float BasicBox::TRIGGER_ZONE_WIDTH = 18.;
const float BasicBox::TRIGGER_ZONE_HEIGHT = 20.;
const float BasicBox::TRIGGER_EXPANSION_FACTOR = 2.5;
const float BasicBox::RAIL_HEIGHT = 20.;
const float BasicBox::MSGS_INDICATOR_WIDTH = 50;
const float BasicBox::EAR_WIDTH = 9;
const float BasicBox::EAR_HEIGHT = 30;
const float BasicBox::GRIP_CIRCLE_SIZE = 5;
unsigned int BasicBox::BOX_MARGIN = 25;
const QString BasicBox::SUB_SCENARIO_MODE_TEXT = tr("Scenario");

BasicBox::BasicBox(const QPointF &press, const QPointF &release, MaquetteScene *parent)
  : QGraphicsItem()
{
  _scene = parent;
  _startMenu = NULL;
  _endMenu = NULL;
  _startMenuButton = NULL;
  _endMenuButton = NULL;

  int xmin = 0, xmax = 0, ymin = 0, ymax = 0;

  xmin = (int)(std::min(press.x(), release.x()));
  ymin = (int)(std::min(press.y(), release.y()));
  xmax = (int)(std::max(press.x(), release.x()));
  ymax = (int)(std::max(press.y(), release.y()));

  _abstract = new AbstractBox();

  _abstract->setTopLeft(QPointF(xmin, ymin));
  _abstract->setWidth(xmax - xmin);
  _abstract->setHeight(ymax - ymin);

  init();

  createWidget();
  createActions();
  createMenus();

  update();
  connect(_comboBox, SIGNAL(currentIndexChanged(const QString &)), _boxContentWidget, SLOT(updateDisplay(const QString &)));
}

void
BasicBox::centerWidget()
{
  _boxWidget->move(-(width()) / 2 + LINE_WIDTH, -(height()) / 2 + (1.2 * RESIZE_TOLERANCE));
  _boxWidget->resize(width() - 2 * LINE_WIDTH, height() - 1.5 * RESIZE_TOLERANCE);

  _comboBox->move(0, -(height() / 2 + LINE_WIDTH));
  _comboBox->resize((width() - 4 * LINE_WIDTH - BOX_MARGIN) / 2, COMBOBOX_HEIGHT);

  _startMenuButton->move(-(width()) / 2 + LINE_WIDTH, -(height()) / 2);
  _endMenuButton->move((width()) / 2 + 2 * LINE_WIDTH - BOX_MARGIN, -(height()) / 2 + LINE_WIDTH);
}

void
BasicBox::createActions()
{
  _jumpToStartCue = new QAction("Jump to cue", this);
  _jumpToEndCue = new QAction("Jump to cue", this);
  _updateStartCue = new QAction("Update cue", this);
  _updateEndCue = new QAction("Update cue", this);

  connect(_jumpToStartCue, SIGNAL(triggered()), _boxContentWidget, SLOT(jumpToStartCue()));
  connect(_jumpToEndCue, SIGNAL(triggered()), _boxContentWidget, SLOT(jumpToEndCue()));
  connect(_updateStartCue, SIGNAL(triggered()), _boxContentWidget, SLOT(updateStartCue()));
  connect(_updateEndCue, SIGNAL(triggered()), _boxContentWidget, SLOT(updateEndCue()));
}

void
BasicBox::createMenus()
{
  _startMenu = new QMenu(tr("&StartMenu"));
  _startMenu->setParent(_boxWidget);
  _startMenu->setWindowModality(Qt::ApplicationModal);
  _startMenu->addAction(_jumpToStartCue);
  _startMenu->addAction(_updateStartCue);

  _endMenu = new QMenu(tr("&EndMenu"));
  _endMenu->setParent(_boxWidget);
  _endMenu->setWindowModality(Qt::ApplicationModal);
  _endMenu->addAction(_jumpToEndCue);
  _endMenu->addAction(_updateEndCue);

  //--- start button ---
  QIcon startMenuIcon(":/images/boxStartMenu.svg");
  _startMenuButton = new QPushButton();
  _startMenuButton->setIcon(startMenuIcon);
  _startMenuButton->setContextMenuPolicy(Qt::CustomContextMenu);

  _startMenuButton->setStyleSheet(
    "QPushButton {"
    "border: none;"
    "border-radius: none;"
    "background-color: transparent;"
    "};"
    );
  _boxContentWidget->setStartMenu(_startMenu);

  //--- end button ---
  QIcon endMenuIcon(":/images/boxEndMenu.svg");
  _endMenuButton = new QPushButton();
  _endMenuButton->setIcon(endMenuIcon);
  _endMenuButton->setShortcutEnabled(1, false);
  _endMenuButton->setStyleSheet(
    "QPushButton {"
    "border: none;"
    "border-radius: none;"
    "background-color: transparent;"
    "}"
    );
  _boxContentWidget->setEndMenu(_endMenu);

  QGraphicsProxyWidget *startMenuProxy = new QGraphicsProxyWidget(this);
  startMenuProxy->setWidget(_startMenuButton);
  QGraphicsProxyWidget *endMenuProxy = new QGraphicsProxyWidget(this);
  endMenuProxy->setWidget(_endMenuButton);

  connect(_startMenuButton, SIGNAL(clicked()), _boxContentWidget, SLOT(execStartAction()));
  connect(_endMenuButton, SIGNAL(clicked()), _boxContentWidget, SLOT(execEndAction()));
  connect(_startMenuButton, SIGNAL(customContextMenuRequested(QPoint)), _boxContentWidget, SLOT(displayStartMenu(QPoint)));
  connect(_endMenuButton, SIGNAL(customContextMenuRequested(QPoint)), _boxContentWidget, SLOT(displayEndMenu(QPoint)));
}

void
BasicBox::updateWidgets()
{
  centerWidget();
}

void
BasicBox::updateDisplay(QString displayMode)
{
  Q_UNUSED(displayMode)
}

void
BasicBox::createWidget()
{
  QBrush brush;
  QPixmap pix(200, 70);
  pix.fill(Qt::transparent);
  brush.setTexture(pix);
  QPalette palette;
  palette.setBrush(QPalette::Background, brush);

  //---------------------- Curve widget ----------------------//
  _boxWidget = new QWidget();
  _boxContentWidget = new BoxWidget(_boxWidget, this);
  QGridLayout *layout = new QGridLayout;
  layout->addWidget(_boxContentWidget);
  layout->setMargin(0);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setAlignment(_boxWidget, Qt::AlignLeft);
  _boxWidget->setLayout(layout);
  _boxContentWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  _curveProxy = new QGraphicsProxyWidget(this);

  _curveProxy->setCacheMode(QGraphicsItem::ItemCoordinateCache);
  _curveProxy->setAcceptedMouseButtons(Qt::LeftButton);
  _curveProxy->setFlag(QGraphicsItem::ItemIsMovable, false);
  _curveProxy->setFlag(QGraphicsItem::ItemIsFocusable, true);
  _curveProxy->setVisible(true);
  _curveProxy->setAcceptsHoverEvents(true);
  _curveProxy->setWidget(_boxWidget);
  _curveProxy->setPalette(palette);

  //---------------- ComboBox (curves list) ------------------//
  _comboBox = new QComboBox;
  _comboBox->view()->setTextElideMode(Qt::ElideMiddle);
  _comboBox->setInsertPolicy(QComboBox::InsertAtTop);
  _comboBox->setBaseSize(_comboBox->width(), COMBOBOX_HEIGHT);
  QFont font;
  font.setPointSize(10);
  _comboBox->setFont(font);

  _comboBoxProxy = new QGraphicsProxyWidget(this);
  _comboBoxProxy->setWidget(_comboBox);
  _comboBoxProxy->setPalette(palette);
  _boxContentWidget->setComboBox(_comboBox);
}

BasicBox::BasicBox(AbstractBox *abstract, MaquetteScene *parent)
  : QGraphicsItem()
{
  _scene = parent;

  _abstract = new AbstractBox(*abstract); /// \todo Pourquoi recevoir un argument *abstract et le ré-instancier ????

  init(); /// \todo Un constructeur !

  update();
}

BasicBox::~BasicBox()
{
  if (_abstract) {
      removeRelations(BOX_START);
      removeRelations(BOX_END);
      delete static_cast<AbstractBox*>(_abstract);
    }
}

QString
BasicBox::currentText()
{
  return _comboBox->currentText();
}

void
BasicBox::updateFlexibility()
{
  _flexible = hasTriggerPoint(BOX_END);
}

/// \todo Une méthode init() ne devrait pas être utilisée. surtout en public !!! Elle peut laisser l'invariant de classe instable à tout moment.
void
BasicBox::init()
{
  _hasContextMenu = false;
  _shift = false;
  _playing = false;
  _low = false;
  _triggerPoints = new QMap<BoxExtremity, TriggerPoint*>();
  _comment = NULL;
  _color = QColor(Qt::white);
  _colorUnselected = QColor(Qt::white);
  _hover = false;

  updateBoxSize();

  setCacheMode(QGraphicsItem::ItemCoordinateCache);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(ItemSendsGeometryChanges, true);
  setVisible(true);
  setAcceptsHoverEvents(true);
  _currentZvalue = 0;
  setZValue(_currentZvalue);
  updateFlexibility();
}

void
BasicBox::changeColor(QColor color)
{
  _color = color;
  _colorUnselected = color;
  update();
}

void
BasicBox::addToComboBox(QString address)
{
  _boxContentWidget->addToComboBox(address);
}

void
BasicBox::updateCurves()
{
  _boxContentWidget->updateMessages(_abstract->ID(), true);
  update();
}

void
BasicBox::updateCurve(string address, bool forceUpdate)
{
  _boxContentWidget->updateCurve(address, forceUpdate);
  update();
}

int
BasicBox::type() const
{
  return BASIC_BOX_TYPE;
}

Abstract*
BasicBox::abstract() const
{
  return _abstract;
}

QPointF
BasicBox::getTopLeft() const
{
  return QPointF(_abstract->topLeft().x(), _abstract->topLeft().y());
}

QPointF
BasicBox::getBottomRight() const
{
  return QPointF(_abstract->topLeft().x() + _abstract->width(), _abstract->topLeft().y() + _abstract->height());
}

QPointF
BasicBox::getCenter() const
{
  return QPointF(_abstract->topLeft().x() + _abstract->width() / 2., _abstract->topLeft().y() + _abstract->height() / 2.);
}

QPointF
BasicBox::getRelativeCenter() const
{
  return QPointF(relativeBeginPos() + _abstract->width() / 2., _abstract->topLeft().y() + _abstract->height() / 2.);
}

QPointF
BasicBox::getSize() const
{
  return QPointF(_abstract->width(), _abstract->height());
}

QPointF
BasicBox::getLeftGripPoint()
{
  return QPointF(_abstract->topLeft().x(), _abstract->topLeft().y() + _abstract->height() / 2.);
}

QPointF
BasicBox::getRightGripPoint()
{
  try{
      return QPointF(_abstract->topLeft().x() + _abstract->width(), _abstract->topLeft().y() + _abstract->height() / 2.);
    }
  catch (const std::exception & e) {
      std::cerr << "BasicBox::getRightGripPoint() : " << e.what();
    }
}

QPointF
BasicBox::getMiddleRight() const
{
  return QPointF(_abstract->topLeft().x() + _abstract->width(), _abstract->topLeft().y() + _abstract->height() / 2.);
}

QPointF
BasicBox::getMiddleLeft() const
{
  return QPointF(_abstract->topLeft().x(), _abstract->topLeft().y() + _abstract->height() / 2.);
}

QPointF
BasicBox::getShapeTopLeft() const
{
  return _abstract->topLeft();
}

QPointF
BasicBox::getShapeTopRight() const
{
  return QPointF(_abstract->topLeft().x() + _abstract->width(),
                 _abstract->topLeft().y());
}

void
BasicBox::setTopLeft(const QPointF & topLeft)
{
  _abstract->setTopLeft(topLeft);

  updateStuff();
}

void
BasicBox::setRelativeTopLeft(const QPointF & rTopLeft)
{
  QPointF relTopLeft(rTopLeft);
  int relBeginPos = relTopLeft.x();
  if (_abstract->mother() != NO_ID && _abstract->mother() != ROOT_BOX_ID) {
      BasicBox *motherBox = _scene->getBox(_abstract->mother());
      if (motherBox != NULL) {
          relBeginPos += motherBox->beginPos();
          relTopLeft.setX(relBeginPos);
        }
      else {
          _abstract->setMother(ROOT_BOX_ID);
        }
    }
  else {
      _abstract->setMother(ROOT_BOX_ID);
    }
  setTopLeft(relTopLeft);
}

void
BasicBox::setSize(const QPointF & size)
{
  _abstract->setWidth(std::max((float)size.x(), MaquetteScene::MS_PRECISION / MaquetteScene::MS_PER_PIXEL));
  _abstract->setHeight(size.y());
  updateStuff();
}

float
BasicBox::beginPos() const
{
  return _abstract->topLeft().x();
}

float
BasicBox::relativeBeginPos() const
{
  float relBeginPos = _abstract->topLeft().x();
  if (_abstract->mother() != NO_ID && _abstract->mother() != ROOT_BOX_ID) {
      BasicBox *motherBox = _scene->getBox(_abstract->mother());
      if (motherBox != NULL) {
          relBeginPos -= _scene->getBox(_abstract->mother())->beginPos();
        }
      else {
          _abstract->setMother(ROOT_BOX_ID);
        }
    }
  else {
      _abstract->setMother(ROOT_BOX_ID);
    }
  return relBeginPos;
}

float
BasicBox::width() const
{
  return _abstract->width();
}

float
BasicBox::height() const
{
  return _abstract->height();
}

QMap<QString, QPair<QString, unsigned int> >
BasicBox::getFinalState()
{
  //QMap <Address, QPair<MessageValue, MessageDate> >

  QMap<QString, QString> startMsgs, endMsgs;
  QMap<QString, QPair<QString, unsigned int> > finalMessages;
  startMsgs = _abstract->startMessages()->toMapAddressValue();
  endMsgs = _abstract->endMessages()->toMapAddressValue();

  //End values
  QList<QString> endAddresses = endMsgs.keys();
  for (QList<QString>::iterator it = endAddresses.begin(); it != endAddresses.end(); it++) {
      finalMessages.insert(*it, QPair<QString, unsigned int>(endMsgs.value(*it), date() + duration()));
    }

  //Start values
  QList<QString> startAddresses = startMsgs.keys();
  for (QList<QString>::iterator it = startAddresses.begin(); it != startAddresses.end(); it++) {
      if (!finalMessages.contains(*it)) {
          finalMessages.insert(*it, QPair<QString, unsigned int>(startMsgs.value(*it), date()));
        }
    }

  return finalMessages;
}

QMap<QString, QPair<QString, unsigned int> >
BasicBox::getStartState()
{
  //QMap <Address, QPair<MessageValue, MessageDate> >

  QMap<QString, QString> startMsgs;
  QMap<QString, QPair<QString, unsigned int> > finalMessages;
  startMsgs = _abstract->startMessages()->toMapAddressValue();

  //Start values
  QList<QString> startAddresses = startMsgs.keys();
  for (QList<QString>::iterator it = startAddresses.begin(); it != startAddresses.end(); it++) {
      finalMessages.insert(*it, QPair<QString, unsigned int>(startMsgs.value(*it), date()));
    }

  return finalMessages;
}


void
BasicBox::resizeWidthEdition(float width)
{
  float newWidth = std::max(width, MaquetteScene::MS_PRECISION / MaquetteScene::MS_PER_PIXEL);

  if (hasMother()) {
      BasicBox *motherBox = _scene->getBox(_abstract->mother());
      if (motherBox != NULL) {
          if ((motherBox->getBottomRight().x() - width) <= _abstract->topLeft().x()) {
              if (_scene->resizeMode() == HORIZONTAL_RESIZE || _scene->resizeMode() == DIAGONAL_RESIZE) {   // Trying to escape by a resize to the right
                  newWidth = motherBox->getBottomRight().x() - _abstract->topLeft().x();
                }
            }
        }
    }
  _abstract->setWidth(newWidth);
  centerWidget();
}

void
BasicBox::resizeHeightEdition(float height)
{
  _abstract->setHeight(height);

  if (_comment != NULL) {
      _comment->updatePos();
    }
  centerWidget();
}

void
BasicBox::resizeAllEdition(float width, float height)
{
  resizeWidthEdition(width);
  resizeHeightEdition(height);
}

QString
BasicBox::name() const
{
  return QString::fromStdString(_abstract->name());
}

void
BasicBox::setName(const QString & name)
{
  _abstract->setName(name.toStdString());
  update();
}

QColor
BasicBox::color() const
{
//    return _abstract->color();
  return _color;
}

void
BasicBox::setColor(const QColor & color)
{
  _abstract->setColor(color);
  _colorUnselected = color;
  _color = color;
}

void
BasicBox::updateRelations(BoxExtremity extremity)
{
  std::map< BoxExtremity, std::map < unsigned int, Relation* > >::iterator it;
  std::map< unsigned int, Relation* >::iterator it2;
  std::map< unsigned int, Relation* >cur;

  Relation *curRel;

  it = _relations.find(extremity);
  if (it != _relations.end()) {
      cur = it->second;
      for (it2 = cur.begin(); it2 != cur.end(); ++it2) {
          curRel = it2->second;
          curRel->lower(_low);
          curRel->updateFlexibility();
        }
    }
}

void
BasicBox::updateStuff()
{
  updateBoxSize();
  if (_comment != NULL) {
      _comment->updatePos();
    }

  map<BoxExtremity, map<unsigned int, Relation*> >::iterator extIt;
  map<unsigned int, Relation*>::iterator relIt;
  for (extIt = _relations.begin(); extIt != _relations.end(); ++extIt) {
      for (relIt = extIt->second.begin(); relIt != extIt->second.end(); ++relIt) {
          relIt->second->updateCoordinates();
        }
    }
  QList<BoxExtremity> list = _triggerPoints->keys();
  QList<BoxExtremity>::iterator it2;
  for (it2 = list.begin(); it2 != list.end(); it2++) {
      _triggerPoints->value(*it2)->updatePosition();
    }
  setFlag(QGraphicsItem::ItemIsMovable, true);
}




void
BasicBox::addRelation(BoxExtremity extremity, Relation *rel)
{
  map<BoxExtremity, Relation*>::iterator it;
  _relations[extremity][rel->ID()] = rel;
  _relations[extremity][rel->ID()]->updateCoordinates();
}

void
BasicBox::removeRelation(BoxExtremity extremity, unsigned int relID)
{
  map<BoxExtremity, map<unsigned int, Relation*> >::iterator it;
  if ((it = _relations.find(extremity)) != _relations.end()) {
      map<unsigned int, Relation*>::iterator it2;
      if ((it2 = it->second.find(relID)) != it->second.end()) {
          it->second.erase(it2);
          if (it->second.empty()) {
              _relations.erase(it);
            }
        }
    }
}

QList<Relation *>
BasicBox::getStartBoxRelations()
{
  return getRelations(BOX_START);
}

QList<Relation *>
BasicBox::getEndBoxRelations()
{
  return getRelations(BOX_END);
}

QList<Relation *>
BasicBox::getRelations(BoxExtremity extremity)
{
  QList<Relation *>relations;
  map<BoxExtremity, map<unsigned int, Relation*> >::iterator it;
  map<unsigned int, Relation*>::iterator itMap;

  if ((it = _relations.find(extremity)) != _relations.end()) {
      for(itMap=(it->second).begin() ; itMap!=(it->second).end() ; ++itMap){
        relations<<itMap->second;
      }
    }
  return relations;
}

void
BasicBox::removeRelations(BoxExtremity extremity)
{
  map<BoxExtremity, map<unsigned int, Relation*> >::iterator it;
  if ((it = _relations.find(extremity)) != _relations.end()) {
      it->second.clear();
      _relations.erase(it);
    }
}

void
BasicBox::addComment()
{
  addComment(AbstractComment(QObject::tr("Comment").toStdString(), _abstract->ID()));
}

void
BasicBox::addComment(const AbstractComment &abstract)
{
  _comment = new Comment(abstract.text(), abstract.ID(), _scene);
  _comment->updatePos();
}

bool
BasicBox::hasComment() const
{
  return(_comment != NULL);
}

Comment*
BasicBox::comment() const
{
  return _comment;
}

void
BasicBox::removeComment()
{
  if (_comment != NULL) {
      _comment = NULL;
    }
}

bool
BasicBox::playing() const
{
  return _playing;
}

void
BasicBox::setCrossedExtremity(BoxExtremity extremity)
{
  if (extremity == BOX_START) {
      _playing = true;
    }
  else if (extremity == BOX_END) {
      _playing = false;
      setCrossedTriggerPoint(false, BOX_START);
      setCrossedTriggerPoint(false, BOX_END);
    }
  _scene->setPlaying(_abstract->ID(), _playing);
}

void
BasicBox::setCrossedTriggerPoint(bool waiting, BoxExtremity extremity)
{
  QMap<BoxExtremity, TriggerPoint*>::iterator it;

//	if ((it = _triggerPoints.find(extremity)) != _triggerPoints.end()) {
  if (_triggerPoints->contains(extremity)) {
      _triggerPoints->value(extremity)->setWaiting(waiting);
    }
  update();
}


bool
BasicBox::hasTriggerPoint(BoxExtremity extremity)
{
  bool result = _triggerPoints->contains(extremity);
  return result;
}

bool
BasicBox::addTriggerPoint(BoxExtremity extremity)
{
  std::string trgName;

  switch (extremity) {
      case BOX_START:
        trgName = "/" + _abstract->name() + "/start";
        break;

      case BOX_END:
        trgName = "/" + _abstract->name() + "/end";
        break;

      default:
        trgName = "/" + _abstract->name() + MaquetteScene::DEFAULT_TRIGGER_MSG;
        break;
    }

  bool ret = false;
  if (!_triggerPoints->contains(extremity)) {
      int trgID = _scene->addTriggerPoint(_abstract->ID(), extremity, trgName);
      if (trgID > NO_ID) {
          _triggerPoints->insert(extremity, _scene->getTriggerPoint(trgID));
          _triggerPoints->value(extremity)->updatePosition();
        }
      ret = true;
      updateRelations(extremity);
      updateFlexibility();
    }


  unlock();

  return ret;
}

void
BasicBox::addTriggerPoint(const AbstractTriggerPoint &abstractTP)
{
  if (!_triggerPoints->contains(abstractTP.boxExtremity())) {
      int trgID = abstractTP.ID();
      if (trgID == NO_ID) {
          trgID = _scene->addTriggerPoint(_abstract->ID(), abstractTP.boxExtremity(), abstractTP.message());
        }
      if (trgID != NO_ID) {
          _triggerPoints->insert(abstractTP.boxExtremity(), _scene->getTriggerPoint(trgID));
          _triggerPoints->value(abstractTP.boxExtremity())->updatePosition();
          _scene->addItem(_triggerPoints->value(abstractTP.boxExtremity()));
        }
      updateFlexibility();
    }
}


void
BasicBox::addTriggerPoint(BoxExtremity extremity, TriggerPoint *tp)
{
  QMap<BoxExtremity, TriggerPoint*>::iterator it;
  if (!_triggerPoints->contains(extremity)) {
      _triggerPoints->insert(extremity, tp);
      _triggerPoints->value(extremity)->updatePosition();
      updateRelations(extremity);
      updateFlexibility();
    }
  else {
      std::cerr << "BasicBox::addTriggerPoint : already existing" << std::endl;
    }
}

void
BasicBox::removeTriggerPoint(BoxExtremity extremity)
{
  if(_triggerPoints->contains(extremity)){

      TriggerPoint *trgPoint = _triggerPoints->value(extremity);
      _triggerPoints->remove(extremity);
      Maquette::getInstance()->removeTriggerPoint(trgPoint->ID());
      _scene->removeFromTriggerQueue(trgPoint);

      updateFlexibility();
      updateRelations(extremity);
      _scene->update();
      update();
    }
}

void
BasicBox::setTriggerPointMessage(BoxExtremity extremity, const string &message)
{
  QMap<BoxExtremity, TriggerPoint*>::iterator it;

//	if ((it = _triggerPoints.find(extremity)) != _triggerPoints.end()) {
  if (_triggerPoints->contains(extremity)) {
      _triggerPoints->value(extremity)->setMessage(message);
    }
}

string
BasicBox::triggerPointMessage(BoxExtremity extremity)
{
  if (_triggerPoints->contains(extremity)) {
      return _triggerPoints->value(extremity)->message();
    }

  else {
      return "";
    }
}

void
BasicBox::setFirstMessagesToSend(const vector<string> &messages)
{
  _abstract->setFirstMsgs(messages);
}

void
BasicBox::setStartMessages(NetworkMessages *messages)
{
  _abstract->setStartMessages(messages);
}

void
BasicBox::setSelectedItemsToSend(QMap<QTreeWidgetItem*, Data> itemsSelected)
{
  _abstract->setNetworkTreeItems(itemsSelected);
}


void
BasicBox::setExpandedItemsList(QList<QTreeWidgetItem *> itemsExpanded)
{
  _abstract->setNetworkTreeExpandedItems(itemsExpanded);
}

void
BasicBox::addToExpandedItemsList(QTreeWidgetItem *item)
{
  _abstract->addToNetworkTreeExpandedItems(item);
}

void
BasicBox::removeFromExpandedItemsList(QTreeWidgetItem *item)
{
  _abstract->removeFromNetworkTreeExpandedItems(item);
}

void
BasicBox::clearExpandedItemsList()
{
  _abstract->clearNetworkTreeExpandedItems();
}
void
BasicBox::setLastMessagesToSend(const vector<string> &messages)
{
  _abstract->setLastMsgs(messages);
}

void
BasicBox::setEndMessages(NetworkMessages *messages)
{
  _abstract->setEndMessages(messages);
}

NetworkMessages *
BasicBox::endMessages()
{
  return _abstract->endMessages();
}

void
BasicBox::setStartMessage(QTreeWidgetItem *item, QString address)
{
  _abstract->setStartMessage(item, address);
}

NetworkMessages *
BasicBox::startMessages()
{
  return _abstract->startMessages();
}

void
BasicBox::setEndMessage(QTreeWidgetItem *item, QString address)
{
  _abstract->setEndMessage(item, address);
}
vector<string>
BasicBox::firstMessagesToSend() const
{
  return _abstract->firstMsgs();
}

vector<string>
BasicBox::lastMessagesToSend() const
{
  return _abstract->lastMsgs();
}

AbstractCurve *
BasicBox::getCurve(const std::string &address)
{
  AbstractCurve * curve = NULL;
  map<string, AbstractCurve*>::iterator it;
  if ((it = _abstractCurves.find(address)) != _abstractCurves.end()) {
      curve = it->second;
    }

  return curve;
}

void
BasicBox::curveActivationChanged(string address, bool activated)
{
  if (!hasCurve(address)) {
      addCurve(address);
    }

  _boxContentWidget->curveActivationChanged(QString::fromStdString(address), activated);

  if (!activated) {
      removeCurve(address);
    }
}

void
BasicBox::setCurve(const string &address, AbstractCurve *curve)
{
  if (curve != NULL) {
      _abstractCurves[address] = curve;
    }
  else {
      removeCurve(address);
    }
}

void
BasicBox::addCurve(const std::string &address)
{
  if (!_curvesAddresses.contains(address)) {
      Maquette::getInstance()->addCurve(ID(), address);
      _curvesAddresses << address;
    }
}
void
BasicBox::addCurveAddress(const std::string &address)
{
  if (!_curvesAddresses.contains(address)) {
      _curvesAddresses << address;
    }
}

void
BasicBox::removeCurve(const string &address)
{
  map<string, AbstractCurve*>::iterator it = _abstractCurves.find(address);
  if (it != _abstractCurves.end()) {
      _abstractCurves.erase(it);
    }
  _boxContentWidget->removeCurve(address);
}

void
BasicBox::lock()
{
  setFlag(QGraphicsItem::ItemIsMovable, false);

//    setFlag(QGraphicsItem::ItemIsSelectable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, false);
}

void
BasicBox::unlock()
{
  setFlag(QGraphicsItem::ItemIsMovable);

//    setFlag(QGraphicsItem::ItemIsSelectable);
  setFlag(QGraphicsItem::ItemIsFocusable);
}

bool
BasicBox::resizing() const
{
  return (cursor().shape() == Qt::SizeFDiagCursor)
         || (cursor().shape() == Qt::SizeHorCursor)
         || (cursor().shape() == Qt::SizeVerCursor);
}

bool
BasicBox::operator<(BasicBox *box) const
{
  return beginPos() < box->beginPos();
}

unsigned int
BasicBox::date() const
{
  return beginPos() * MaquetteScene::MS_PER_PIXEL;
}

unsigned int
BasicBox::duration() const
{
  return width() * MaquetteScene::MS_PER_PIXEL;
}

unsigned int
BasicBox::ID() const
{
  return _abstract->ID();
}

void
BasicBox::setID(unsigned int ID)
{
  _abstract->setID(ID);
}

unsigned int
BasicBox::mother() const
{
  return _abstract->mother();
}

void
BasicBox::setMother(unsigned int motherID)
{
  _abstract->setMother(motherID);
}

bool
BasicBox::hasMother()
{
  return(_abstract->mother() != NO_ID && _abstract->mother() != ROOT_BOX_ID);
}

QVariant
BasicBox::itemChange(GraphicsItemChange change, const QVariant &value)
{
  QVariant newValue = QGraphicsItem::itemChange(change, value);

  //QVariant newValue(value);
  if (change == ItemPositionChange) {
      QPointF newPos = value.toPoint();
      QPointF newnewPos(newPos);
      if (hasMother()) {
          BasicBox *motherBox = _scene->getBox(_abstract->mother());
          if (motherBox != NULL) {
              if ((motherBox->getTopLeft().y() + _abstract->height() / 2.) >= newPos.y()) {
                  if (_scene->resizeMode() == NO_RESIZE) {   // Trying to escape by a move to the top
                      newnewPos.setY(motherBox->getTopLeft().y() + _abstract->height() / 2.);
                    }
                  else if (_scene->resizeMode() == VERTICAL_RESIZE
                           || _scene->resizeMode() == DIAGONAL_RESIZE) {  // Trying to escape by a resize to the top
                      // Not happening
                    }
                }
              if ((motherBox->getBottomRight().y() - _abstract->height() / 2.) <= newPos.y()) {
                  if (_scene->resizeMode() == NO_RESIZE) {   // Trying to escape by a move to the bottom
                      newnewPos.setY(motherBox->getBottomRight().y() - _abstract->height() / 2);
                    }
                  else if (_scene->resizeMode() == VERTICAL_RESIZE
                           || _scene->resizeMode() == DIAGONAL_RESIZE) {  // Trying to escape by a resize to the bottom
                      _abstract->setHeight(motherBox->getBottomRight().y() - _abstract->topLeft().y());
                      newnewPos.setY(_abstract->topLeft().y() + _abstract->height() / 2);
                    }
                }
              if ((motherBox->getTopLeft().x() + _abstract->width() / 2.) >= newPos.x()) {
                  if (_scene->resizeMode() == NO_RESIZE) {   // Trying to escape by a move to the left
                      newnewPos.setX(motherBox->getTopLeft().x() + _abstract->width() / 2.);
                    }
                  else if (_scene->resizeMode() == HORIZONTAL_RESIZE
                           || _scene->resizeMode() == DIAGONAL_RESIZE) {  // Trying to escape by a resize to the left
                      // Not happening
                    }
                }
              if ((motherBox->getBottomRight().x() - _abstract->width() / 2.) <= newPos.x()) {
                  if (_scene->resizeMode() == NO_RESIZE) {   // Trying to escape by a move to the right
                      newnewPos.setX(motherBox->getBottomRight().x() - _abstract->width() / 2);
                    }
                  else if (_scene->resizeMode() == HORIZONTAL_RESIZE
                           || _scene->resizeMode() == DIAGONAL_RESIZE) {  // Trying to escape by a resize to the right
                      // Handled by BasicBox::resizeWidthEdition()
                    }
                }
              newValue = QVariant(newnewPos);
            }
        }
    }

  return newValue;
}

QPainterPath
BasicBox::shape() const
{
  QPainterPath path;

  path.addRect(_boxRect);
  path.addRect(_leftEar);
  path.addRect(_rightEar);
  path.addRect(_startTriggerGrip);
  path.addRect(_endTriggerGrip);

  return path;
}

void
BasicBox::updateBoxSize()
{
  _boxRect = QRectF(-_abstract->width() / 2, -_abstract->height() / 2, _abstract->width(), _abstract->height());
}

// Bounding box of the item - useful to detect mouse interaction
QRectF
BasicBox::boundingRect() const
{
  // Origine du repere = centre de l'objet
  return QRectF(-_abstract->width() / 2 - BOX_MARGIN, -_abstract->height() / 2 - 3 * BOX_MARGIN, _abstract->width() + 2 * BOX_MARGIN, _abstract->height() + 4 * BOX_MARGIN);
}

QRectF
BasicBox::boxRect()
{
  return _boxRect;
}

QRectF
BasicBox::boxBody()
{
  return QRectF(_boxRect.topLeft() + QPointF(0, RESIZE_TOLERANCE), _boxRect.bottomRight());
}
void
BasicBox::keyPressEvent(QKeyEvent *event)
{
  QGraphicsItem::keyPressEvent(event);
}

void
BasicBox::keyReleaseEvent(QKeyEvent *event)
{
  QGraphicsItem::keyReleaseEvent(event);
}

void
BasicBox::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mousePressEvent(event);

  if (_startMenu != NULL) {
      _startMenu->close();
    }
  if (_endMenu != NULL) {
      _endMenu->close();
    }

  if (event->button() == Qt::LeftButton) {
      setSelected(true);

//        if(_startMenuButton->is){
//            std::cout<<"<<<<<CHECKED>>>>>"<<std::endl;
//            if(event->modifiers()==Qt::ControlModifier){
//                std::cout<<"<<<<<STARTMENU>>>>>"<<std::endl;
//                _updateStartCue->trigger();
//            }
//        }

      if (cursor().shape() == Qt::OpenHandCursor) {
          setCursor(Qt::ClosedHandCursor);
        }
      if (cursor().shape() == Qt::ArrowCursor) {
          lock();
        }
      else if (cursor().shape() == Qt::CrossCursor) {
          lock();
          if (event->pos().x() < _boxRect.topLeft().x() + RESIZE_TOLERANCE) {
              _scene->setRelationFirstBox(_abstract->ID(), BOX_START);
            }
          else if (event->pos().x() > _boxRect.topRight().x() - RESIZE_TOLERANCE) {
              _scene->setRelationFirstBox(_abstract->ID(), BOX_END);
            }
        }
      else if (cursor().shape() == Qt::PointingHandCursor) {
          lock();
          if (event->pos().x() < _boxRect.topLeft().x() + RESIZE_TOLERANCE) {
              addTriggerPoint(BOX_START);
            }
          else if (event->pos().x() > _boxRect.topRight().x() - RESIZE_TOLERANCE) {
              addTriggerPoint(BOX_END);
            }
        }
      else if (cursor().shape() == Qt::SizeHorCursor) {
          _scene->setResizeMode(HORIZONTAL_RESIZE);
          _scene->setResizeBox(_abstract->ID());
        }
      else if (cursor().shape() == Qt::SizeVerCursor) {
          _scene->setResizeMode(VERTICAL_RESIZE);
          _scene->setResizeBox(_abstract->ID());
        }
      else if (cursor().shape() == Qt::SizeFDiagCursor) {
          _scene->setResizeMode(DIAGONAL_RESIZE);
          _scene->setResizeBox(_abstract->ID());
        }
      update();
    }
}

void
BasicBox::lower(bool state)
{
  _low = state;

  if (_low) {
      setZValue(-1);
      setEnabled(false);
      setOpacity(0.5);
    }
  else {
      setOpacity(1);
      setZValue(0);
      setEnabled(true);
    }
  updateRelations(BOX_START);
  updateRelations(BOX_END);
}

void
BasicBox::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
  QGraphicsItem::contextMenuEvent(event);

  if (_hasContextMenu) {
      setSelected(false);
      _contextMenu->exec(event->screenPos());
    }
}

QInputDialog *
BasicBox::nameInputDialog()
{
  QInputDialog *nameDialog = new QInputDialog(_scene->views().first(), Qt::Popup);
  nameDialog->setInputMode(QInputDialog::TextInput);
  nameDialog->setLabelText(QObject::tr("Enter the box name :"));
  nameDialog->setTextValue(QString::fromStdString(this->_abstract->name()));
  QPoint position = _scene->views().first()->parentWidget()->pos();
  int MMwidth = _scene->views().first()->parentWidget()->width();
  nameDialog->move(position.x() + MMwidth / 2, position.y());

  return nameDialog;
}

void
BasicBox::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);
  QRectF textRect(mapFromScene(getTopLeft()).x(), mapFromScene(getTopLeft()).y(), width(), RESIZE_TOLERANCE - LINE_WIDTH);
  qreal x1, x2, y1, y2;
  textRect.getCoords(&x1, &y1, &x2, &y2);
  bool inTextRect = event->pos().x() > x1 && event->pos().x() < x2 && event->pos().y() > y1 && event->pos().y() < y2;


  if (!_scene->playing() && (_scene->resizeMode() == NO_RESIZE && cursor().shape() == Qt::OpenHandCursor) && inTextRect) {
      QInputDialog *nameDialog = nameInputDialog();

      bool ok = nameDialog->exec();
      QString nameValue = nameDialog->textValue();

      if (ok) {
          _abstract->setName(nameValue.toStdString());
          this->update();
          _scene->displayMessage(QObject::tr("Box's name successfully updated").toStdString(), INDICATION_LEVEL);
        }

//     delete nameDialog;
//        displayCurveEditWindow();
//        BoxCurveEdit *boxCurveEdit = new BoxCurveEdit(0,this);
//        boxCurveEdit->exec();
    }

  else {
//        emit(doubleClickInBox());
    }
}

void
BasicBox::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mouseMoveEvent(event);

  // Draw cursor coordinates as a tooltip
//    CurveWidget *curve = (static_cast<CurveWidget *>(_boxContentWidget->_stackedLayout->currentWidget()));
//    QPointF mousePos = curve->relativeCoordinates(event->pos());
//    QRect rect;
//    QString posStr = QString("%1 ; %2").arg(mousePos.x(),0,'f',2).arg(mousePos.y(),0,'f',2);
//    QPoint pos = this->getBottomRight().toPoint();
//    QToolTip::showText(pos, posStr);

  if (_scene->resizeMode() == NO_RESIZE && cursor().shape() == Qt::ClosedHandCursor) {
      _scene->selectionMoved();
    }
  else if (_scene->resizeMode() != NO_RESIZE && (cursor().shape() == Qt::SizeVerCursor || cursor().shape() == Qt::SizeHorCursor || cursor().shape() == Qt::SizeFDiagCursor)) {
      switch (_scene->resizeMode()) {
          case HORIZONTAL_RESIZE:
            resizeWidthEdition(_abstract->width() + event->pos().x() - _boxRect.topRight().x());
            break;

          case VERTICAL_RESIZE:
            resizeHeightEdition(_abstract->height() + event->pos().y() - _boxRect.bottomRight().y());
            break;

          case DIAGONAL_RESIZE:
            resizeAllEdition(_abstract->width() + event->pos().x() - _boxRect.topRight().x(),
                             _abstract->height() + event->pos().y() - _boxRect.bottomRight().y());
            break;
        }
      QPainterPath nullPath;
      nullPath.addRect(QRectF(QPointF(0., 0.), QSizeF(0., 0.)));
      _scene->setSelectionArea(nullPath);
      setSelected(true);
      _scene->boxResized();
    }
}

void
BasicBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsItem::mouseReleaseEvent(event);

  if (event->button() == Qt::LeftButton) {
      if (cursor().shape() == Qt::ClosedHandCursor) {
          setCursor(Qt::OpenHandCursor);
        }
      _scene->setAttributes(_abstract);
    }

  if (!playing()) {
      unlock();
    }

  _scene->setResizeMode(NO_RESIZE);
}

void
BasicBox::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  QGraphicsItem::hoverEnterEvent(event);

  _hover = true;

  const float RESIZE_ZONE_WIDTH = 3 * LINE_WIDTH;

  QRectF textRect(_boxRect.topLeft(), _boxRect.topRight() + QPointF(0, RESIZE_TOLERANCE));

  QRectF triggerGripLeft = _startTriggerGrip;
  QRectF triggerGripRight = _endTriggerGrip;

  QRectF relationGripLeft = _leftEar;
  QRectF relationGripRight = _rightEar;

  QRectF vertResize_bottom(_boxRect.bottomLeft() + QPointF(0, -RESIZE_ZONE_WIDTH), _boxRect.bottomRight() - QPointF(RESIZE_ZONE_WIDTH, 0));
  QRectF diagResize_bottomRight(_boxRect.bottomRight() - QPointF(RESIZE_ZONE_WIDTH, RESIZE_ZONE_WIDTH), _boxRect.bottomRight());

  //bandeau zone (text rect) - top
  if (textRect.contains(event->pos())) {
      setCursor(Qt::OpenHandCursor);
    }

  //Trigger zone - left
  else if (triggerGripLeft.contains(event->pos())) {
      setCursor(Qt::PointingHandCursor);
    }

  //Trigger zone - right
  else if (triggerGripRight.contains(event->pos())) {
      setCursor(Qt::PointingHandCursor);
    }

  //Relation grip zone - left
  else if (relationGripLeft.contains(event->pos())) {
      setCursor(Qt::CrossCursor);
    }

  //Relation grip zone - right
  else if (relationGripRight.contains(event->pos())) {
      setCursor(Qt::CrossCursor);
    }

  //Vertical resize zone - bottom
  else if (vertResize_bottom.contains(event->pos())) {
      setCursor(Qt::SizeVerCursor);
    }

  //Diag resize zone - Bottom right
  else if (diagResize_bottomRight.contains(event->pos())) {
      setCursor(Qt::SizeFDiagCursor);
    }

  else {
      setCursor(Qt::ArrowCursor);
    }
}


void
BasicBox::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  //Bug qui survient parfois :
  //  Le curseur a mémorisé la forme qu'il avait avant de rentrer dans la zone BoxWidget.
  //  Même si il est pourtant visible avec la forme ArrowCursor, il exécute au click l'action associée au curseur précédent.
  //
  //Solution provisoire :
  //  Pour palier à cela il y a une zone "vide" (qui met le curseur à ArrowCursor) avant d'entrer dans le widget.
  //  Seulement si on passe trop vite dessus, il n'a pas le temps de l'affecter.

  QGraphicsItem::hoverEnterEvent(event);
  _hover = true;
  const float RESIZE_ZONE_WIDTH = 3 * LINE_WIDTH;

  QRectF textRect(_boxRect.topLeft(), _boxRect.topRight() + QPointF(0, 3 * RESIZE_TOLERANCE / 4));

  QRectF triggerGripLeft = _startTriggerGrip;
  QRectF triggerGripRight = _endTriggerGrip;

  QRectF relationGripLeft = _leftEar;
  QRectF relationGripRight = _rightEar;

  QRectF vertResize_bottom(_boxRect.bottomLeft() + QPointF(0, -RESIZE_ZONE_WIDTH), _boxRect.bottomRight() - QPointF(RESIZE_ZONE_WIDTH, 0));
  QRectF diagResize_bottomRight(_boxRect.bottomRight() - QPointF(RESIZE_ZONE_WIDTH, RESIZE_ZONE_WIDTH), _boxRect.bottomRight());

  //bandeau zone (text rect) - top
  if (textRect.contains(event->pos())) {
      setCursor(Qt::OpenHandCursor);
    }

  //Trigger zone - left
  else if (triggerGripLeft.contains(event->pos())) {
      setCursor(Qt::PointingHandCursor);
    }

  //Trigger zone - right
  else if (triggerGripRight.contains(event->pos())) {
      setCursor(Qt::PointingHandCursor);
    }

  //Relation grip zone - left
  else if (relationGripLeft.contains(event->pos())) {
      setCursor(Qt::CrossCursor);
    }

  //Relation grip zone - right
  else if (relationGripRight.contains(event->pos())) {
      setCursor(Qt::CrossCursor);
    }

  //Vertical resize zone - bottom
  else if (vertResize_bottom.contains(event->pos())) {
      setCursor(Qt::SizeVerCursor);
    }

  //Diag resize zone - Bottom right
  else if (diagResize_bottomRight.contains(event->pos())) {
      setCursor(Qt::SizeFDiagCursor);
    }

  else {
      setCursor(Qt::ArrowCursor);
    }
}

void
BasicBox::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  QGraphicsItem::hoverLeaveEvent(event);
  setCursor(Qt::ArrowCursor);
  _hover = false;
}

void
BasicBox::drawTriggerGrips(QPainter *painter)
{
  int earWidth, earHeight;
  float adjust = LINE_WIDTH / 2;
  earWidth = TRIGGER_ZONE_WIDTH * 2;
  earHeight = TRIGGER_ZONE_HEIGHT;

  int startAngle, spanAngle;
  int newX = -width() / 2 - adjust - earWidth / 2;
  int newYLeft;
  int newYRight;
  int railY = -height() / 2 - RAIL_HEIGHT / 2;

  QSize shapeSizeLeft;
  QSize shapeSizeRight;
  QRectF rectLeft;
  QRectF rectRight;

  if (hasTriggerPoint(BOX_START)) {
      newYLeft = -height() / 2 - earHeight * TRIGGER_EXPANSION_FACTOR / 2;
      shapeSizeLeft = QSize(earWidth / 2, (earHeight * TRIGGER_EXPANSION_FACTOR / 2));
      rectLeft = QRectF(0, 0, earWidth, earHeight * TRIGGER_EXPANSION_FACTOR);
    }
  else {
      newYLeft = -height() / 2 - earHeight / 2;
      shapeSizeLeft = QSize(earWidth / 2, earHeight / 2);
      rectLeft = QRectF(0, 0, earWidth, earHeight);
    }

  if (hasTriggerPoint(BOX_END)) {
      newYRight = -height() / 2 - earHeight * TRIGGER_EXPANSION_FACTOR / 2;
      shapeSizeRight = QSize(earWidth / 2, (earHeight / 2) * TRIGGER_EXPANSION_FACTOR);
      rectRight = QRectF(0, 0, earWidth, earHeight * TRIGGER_EXPANSION_FACTOR);
    }
  else {
      newYRight = -height() / 2 - earHeight / 2;
      shapeSizeRight = QSize(earWidth / 2, (earHeight / 2));
      rectRight = QRectF(0, 0, earWidth, earHeight);
    }

  QPen pen(isSelected() ? _color : _colorUnselected);

  QBrush brush(Qt::SolidPattern);
  brush.setColor(isSelected() ? _color : _colorUnselected);

  painter->setBrush(brush);
  painter->setPen(pen);



  //The left one
  startAngle = 0;
  spanAngle = 90 * 16;
  rectLeft.moveTo(newX, newYLeft);
  _startTriggerGrip = QRectF(QPointF(newX + earWidth / 2, newYLeft), shapeSizeLeft);
  painter->drawPie(rectLeft, startAngle, spanAngle);

  //Point left
  QPainterPath startCircle, endCircle;
  startCircle.addEllipse(QPointF(newX + 2.5 * earWidth / 4, railY + RAIL_HEIGHT / 4), GRIP_CIRCLE_SIZE / 2, GRIP_CIRCLE_SIZE / 2);
  painter->fillPath(startCircle, Qt::black);

  //The right one
  startAngle = 90 * 16;
  float newX2 = width() / 2 - earWidth / 2 + adjust;
  rectRight.moveTo(newX2, newYRight);
  _endTriggerGrip = QRectF(QPointF(newX2, newYRight), shapeSizeRight);
  painter->drawPie(rectRight, startAngle, spanAngle);

  //Point left
  endCircle.addEllipse(QPointF(newX2 + 1.5 * earWidth / 4, railY + RAIL_HEIGHT / 4), GRIP_CIRCLE_SIZE / 2, GRIP_CIRCLE_SIZE / 2);
  painter->fillPath(endCircle, Qt::black);

  //Interval line
  painter->save();
  QPen linePen = QPen(Qt::black);
  linePen.setWidth(LINE_WIDTH);
  linePen.setStyle(_flexible ? Qt::DotLine : Qt::SolidLine);
  painter->setPen(linePen);
  painter->drawLine(QPointF(newX + 2.5 * earWidth / 4, railY + RAIL_HEIGHT / 4), QPointF(newX2 + 1.5 * earWidth / 4, railY + RAIL_HEIGHT / 4));
  painter->restore();
}

void
BasicBox::drawInteractionGrips(QPainter *painter)
{
  float earWidth = EAR_WIDTH * 2;
  float earHeight = EAR_HEIGHT;

  QPen pen(isSelected() ? _color : _colorUnselected);

  QBrush brush(Qt::SolidPattern);
  brush.setColor(isSelected() ? _color : _colorUnselected);

  painter->setBrush(brush);
  painter->setPen(pen);

  QRectF rect(0, 0, earWidth, earHeight);

  int startAngle = 30 * 16;
  int spanAngle = 120 * 16;
  painter->rotate(90);
  rect.moveTo(QPointF(-(earWidth / 2), -(earHeight / 4 + width() / 2)));

  int newX = -(earHeight / 4 + width() / 2);
  int newY = -(earWidth / 2);
  _leftEar = QRectF(QPointF(newX, newY), QSize(earHeight / 4, earWidth));

  newX = width() / 2;
  _rightEar = QRectF(QPointF(newX, newY), QSize(earHeight / 4, earWidth));

  painter->drawChord(rect, startAngle, spanAngle);
  painter->rotate(-180);
  painter->drawChord(rect, startAngle, spanAngle);
  painter->rotate(90);
}

void
BasicBox::drawMsgsIndicators(QPainter *painter)
{
  if (width() > 2 * RESIZE_TOLERANCE) {
      _startMsgsIndicator = QRectF(QPointF(_boxRect.topLeft().x(), _boxRect.topLeft().y() + RESIZE_TOLERANCE - LINE_WIDTH), QPointF(_boxRect.x() + MSGS_INDICATOR_WIDTH, _boxRect.bottomLeft().y()));
      _endMsgsIndicator = QRectF(QPointF(_boxRect.topRight().x(), _boxRect.topRight().y() + RESIZE_TOLERANCE - LINE_WIDTH), QPointF(_boxRect.bottomRight().x() - MSGS_INDICATOR_WIDTH, _boxRect.bottomRight().y()));
    }
  else {
      _startMsgsIndicator = QRectF(QPointF(_boxRect.topLeft().x(), _boxRect.topLeft().y() + RESIZE_TOLERANCE - LINE_WIDTH), QPointF(_boxRect.x() + width() / 3, _boxRect.bottomLeft().y()));
      _endMsgsIndicator = QRectF(QPointF(_boxRect.topRight().x(), _boxRect.topRight().y() + RESIZE_TOLERANCE - LINE_WIDTH), QPointF(_boxRect.bottomRight().x() - width() / 3, _boxRect.bottomRight().y()));
    }

  if (hasStartMsgs()) {
      painter->setPen(Qt::NoPen);
      QLinearGradient lgradient(_startMsgsIndicator.topLeft(), _startMsgsIndicator.topRight());
      lgradient.setColorAt(0, isSelected() ? _color : _colorUnselected);

      lgradient.setColorAt(1, Qt::transparent);
      painter->fillRect(_startMsgsIndicator, lgradient);
    }

  if (hasEndMsgs()) {
      painter->setPen(Qt::NoPen);
      QLinearGradient lgradient(_endMsgsIndicator.topLeft(), _endMsgsIndicator.topRight());
      lgradient.setColorAt(1, Qt::transparent);
      lgradient.setColorAt(0, isSelected() ? _color : _colorUnselected);
      painter->fillRect(_endMsgsIndicator, lgradient);
    }
}

void
BasicBox::drawHoverShape(QPainter *painter)
{
  float interspace = 2.;
  int width = 2;

  painter->save();

  QPen penBlue(Qt::blue);
  penBlue.setWidth(width);
  painter->setPen(penBlue);

  QPainterPath path;
  path.addRect(_boxRect);
  QRect hoverRect(QPoint(_boxRect.topLeft().x() - interspace, _boxRect.topLeft().y() - interspace),
                  QPoint(_boxRect.bottomRight().x() + interspace, _boxRect.bottomRight().y() + interspace));

  path.addRect(hoverRect);
  painter->drawPath(path);

  painter->restore();
}

void
BasicBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

//    QPen penR(Qt::lightGray,isSelected() ? 2 * LINE_WIDTH : LINE_WIDTH);
  QPen penR(isSelected() ? _color : _colorUnselected, isSelected() ? 1.5 * LINE_WIDTH : LINE_WIDTH);


  //************* pour afficher la shape *************
  QPen penG(Qt::blue);
  penG.setWidth(4);
  if (isSelected()) {
      drawHoverShape(painter);
    }
  else if (_hover) {
      drawHoverShape(painter);
    }


//    painter->drawRect(_leftEar);
//    painter->drawRect(_rightEar);
//    painter->drawRect(_startTriggerGrip);
//    painter->drawRect(_endTriggerGrip);
  //***************************************************/

  /************   Draw boxRect ************/
  painter->setPen(penR);
  painter->setBrush(QBrush(Qt::white, Qt::Dense7Pattern));
  painter->drawRect(_boxRect);

  drawMsgsIndicators(painter);
  drawInteractionGrips(painter);
  drawTriggerGrips(painter);

  _comboBoxProxy->setVisible(_abstract->height() > RESIZE_TOLERANCE + LINE_WIDTH);
  _curveProxy->setVisible(_abstract->height() > RESIZE_TOLERANCE + LINE_WIDTH);

  QBrush brush(Qt::lightGray, isSelected() ? Qt::SolidPattern : Qt::SolidPattern);
  QPen pen(color(), isSelected() ? 2 * LINE_WIDTH : LINE_WIDTH);
  painter->setPen(pen);
  painter->setBrush(brush);
  painter->setRenderHint(QPainter::Antialiasing, true);

  QRectF textRect = QRectF(mapFromScene(getTopLeft()).x(), mapFromScene(getTopLeft()).y(), width(), RESIZE_TOLERANCE - LINE_WIDTH);

  QFont font;
  font.setCapitalization(QFont::SmallCaps);
  painter->setFont(font);
  painter->translate(textRect.topLeft());

  if (_abstract->width() <= 3 * RESIZE_TOLERANCE) {
      painter->translate(QPointF(RESIZE_TOLERANCE - LINE_WIDTH, 0));
      painter->rotate(90);
      textRect.setWidth(_abstract->height());
    }

  if (_abstract->width() <= 5 * RESIZE_TOLERANCE) {
      _comboBoxProxy->setVisible(false);
      _curveProxy->setVisible(false);
    }

  painter->fillRect(0, 0, textRect.width(), textRect.height(), isSelected() ? _color : _colorUnselected);

  painter->save();
  painter->setPen(QPen(Qt::black));
  painter->drawText(QRectF(BOX_MARGIN, 0, textRect.width(), textRect.height()), Qt::AlignLeft, name());
  painter->restore();

  if (_abstract->width() <= 3 * RESIZE_TOLERANCE) {
      painter->rotate(-90);
      painter->translate(-QPointF(RESIZE_TOLERANCE - LINE_WIDTH, 0));
    }


  painter->translate(QPointF(0, 0) - (textRect.topLeft()));

  if (cursor().shape() == Qt::SizeHorCursor) {
      static const float S_TO_MS = 1000.;
      painter->drawText(_boxRect.bottomRight() - QPoint(2 * RESIZE_TOLERANCE, 0), QString("%1s").arg((double)duration() / S_TO_MS));
    }

  painter->translate(_boxRect.topLeft());

  if (_playing) {
      QPen pen = painter->pen();
      pen.setColor(Qt::black);
      pen.setWidth(3);
      QBrush brush = painter->brush();
      brush.setStyle(Qt::NoBrush);
      painter->setPen(pen);
      brush.setColor(Qt::blue);
      painter->setBrush(brush);
      const float progressPosX = _scene->getProgression(_abstract->ID()) * (_abstract->width());
      painter->fillRect(0, _abstract->height() - RESIZE_TOLERANCE / 2., progressPosX, RESIZE_TOLERANCE / 2., Qt::darkGreen);

      painter->drawLine(QPointF(progressPosX, RESIZE_TOLERANCE), QPointF(progressPosX, _abstract->height()));
    }
  painter->translate(QPointF(0, 0) - _boxRect.topLeft());

  setOpacity(isSelected() ? 1 : 0.4);
}

void
BasicBox::curveShowChanged(const QString &address, bool state)
{
  _boxContentWidget->curveShowChanged(address, state);
}

void
BasicBox::displayCurveEditWindow()
{
  QWidget *editWindow = new QWidget;
  editWindow->setWindowModality(Qt::ApplicationModal);
  QGridLayout *layout = new QGridLayout;

  editWindow->setLayout(layout);
  editWindow->setGeometry(QRect(_scene->sceneRect().toRect()));
  editWindow->show();
}
