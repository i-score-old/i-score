/*
 * Copyright: LaBRI / SCRIME / L'Arboretum
 *
 * Authors: Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine (as of 16/03/2014)
 *
 * iscore.contact@gmail.com
 *
 * This software is an interactive intermedia sequencer.
 * It allows the precise and flexible scripting of interactive scenarios.
 * In contrast to most sequencers, i-score doesn’t produce any media, 
 * but controls other environments’ parameters, by creating snapshots 
 * and automations, and organizing them in time in a multi-linear way.
 * More about i-score on http://www.i-score.org
 *
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
#include "Relation.hpp"
#include "MaquetteScene.hpp"
#include "Maquette.hpp"
#include "AbstractRelation.hpp"

#include <iostream>
#include <math.h>

#include <QGraphicsView>
#include <QPointF>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QWidget>
#include <QRectF>
#include <QString>
#include <QMenu>
#include <QToolTip>
#include <QDebug>
const float Relation::ARROW_SIZE = 12.;
const float Relation::TOLERANCE_X = 12.;
const float Relation::TOLERANCE_Y = 12.;
const float Relation::HANDLE_HEIGHT = 10.;
const float Relation::HANDLE_WIDTH = 12.;
const float Relation::GRIP_CIRCLE_SIZE = 5;
const float Relation::RAIL_WIDTH = HANDLE_HEIGHT / 2;
const float Relation::LINE_WIDTH = 2;
const float Relation::RIGID_TOLERANCE = 0.;

Relation::Relation(unsigned int firstBoxID, BoxExtremity firstBoxExt, unsigned int secondBoxID,
                   BoxExtremity secondBoxExt, MaquetteScene *parent)
  : QGraphicsItem()
{
  _scene = parent;

  _abstract = new AbstractRelation(firstBoxID, firstBoxExt, secondBoxID, secondBoxExt, NO_ID);

  init();
}

Relation::Relation(const AbstractRelation &abstract, MaquetteScene *parent)
  : QGraphicsItem()
{
  _scene = parent;
  _abstract = new AbstractRelation(abstract);

  init();

  //setPos(getCenter());
}

Relation::~Relation()
{
  delete _abstract;
}

void
Relation::init()
{
  updateCoordinates();
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  setVisible(true);
  setAcceptHoverEvents(true);
  setZValue(1);
  _leftHandleSelected = false;
  _rightHandleSelected = false;
  _color = QColor(Qt::blue);
  _lastMaxBound = -1;
  _elasticMode = false;
  _mouseClickPosSave = mapToScene(_start);
  _hover = false;
  updateFlexibility();
}

Abstract *
Relation::abstract() const
{
  return _abstract;
}

int
Relation::type() const
{
  return RELATION_TYPE;
}

QString
Relation::toString()
{
  QString ret;
  ret = "'" + _scene->getBox(_abstract->firstBox())->name() + "'";
  switch (_abstract->firstExtremity()) {
      case BOX_START:
        ret += QObject::tr(" start");
        break;

      case BOX_END:
        ret += QObject::tr(" end");
        break;

      case NO_EXTREMITY:
        ret += QObject::tr(" no extremity");
        break;

      default:
        ret += QObject::tr(" unknown extremity");
        break;
    }

  ret += QObject::tr(" [Before] ");
  ret += "'" + _scene->getBox(_abstract->secondBox())->name() + "'";

  switch (_abstract->secondExtremity()) {
      case BOX_START:
        ret += QObject::tr(" start");
        break;

      case BOX_END:
        ret += QObject::tr(" end");
        break;

      case NO_EXTREMITY:
        ret += QObject::tr(" no extremity");
        break;

      default:
        ret += QObject::tr(" unknown extremity");
        break;
    }

  return ret;
}

void
Relation::updateCoordinates()
{
  BasicBox *box = _scene->getBox(_abstract->firstBox());
  if (box != nullptr) {
      switch (_abstract->firstExtremity()) {
          case BOX_START:
            _start = box->getLeftGripPoint();            
            break;

          case BOX_END:
            _start = box->getRightGripPoint();
            break;

          case NO_EXTREMITY:
            _start = box->getCenter();
            break;
        }
    }

  box = _scene->getBox(_abstract->secondBox());
  if (box != nullptr) {
      switch (_abstract->secondExtremity()) {
          case BOX_START:
            if (box != nullptr) {
                _end = box->getLeftGripPoint();
              }
            break;

          case BOX_END:
            if (box != nullptr) {
                _end = box->getRightGripPoint();
              }
            break;

          case NO_EXTREMITY:
            if (box != nullptr) {
                _end = box->getCenter();
              }
            break;
        }
    }
//  std::cout<<"\t start = "<<_start.x()*MaquetteScene::MS_PER_PIXEL<<std::endl;
//  std::cout<<"\t end = "<<_end.x()*MaquetteScene::MS_PER_PIXEL<<std::endl;
  //  std::cout<<"\t Length = "<<_abstract->_length*MaquetteScene::MS_PER_PIXEL<<std::endl;
  _abstract->_length = _end.x() - _start.x();
  setPos(getCenter());
}

void
Relation::lower(bool state)
{
  if (state) {
      setEnabled(false);
      setZValue(-1);
      setOpacity(0.5);
    }
  else {
      setEnabled(true);
      setZValue(1);
      setOpacity(1);
    }
}

QPointF
Relation::getCenter() const
{
  return QPointF(_start.x() + (_end.x() - _start.x()) / 2., _start.y() + (_end.y() - _start.y()) / 2.);
}

unsigned int
Relation::ID() const
{
  return _abstract->ID();
}

void
Relation::setID(unsigned int ID)
{
  _abstract->setID(ID);
}

void
Relation::changeBounds(const float &minBound, const float &maxBound)
{    
  _abstract->setMinBound(minBound);
  _abstract->setMaxBound(maxBound);
}

void
Relation::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  QGraphicsItem::hoverEnterEvent(event);
  _hover = true;
  double startX = mapFromScene(_start).x() + BasicBox::EAR_WIDTH / 2;
  double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();

  double endBound = NO_BOUND, startBound = NO_BOUND;
  if (_abstract->maxBound() != NO_BOUND) {
      endBound = startX + _abstract->maxBound() * _scene->zoom();
    }
  if (_abstract->minBound() != NO_BOUND) {
      startBound = startX + _abstract->minBound() * _scene->zoom();
    }

  if (_flexibleRelation) {
      if (QRectF(startBound - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2., HANDLE_WIDTH, HANDLE_HEIGHT).contains(event->pos())) {
          if (startBound != NO_BOUND) {
              setCursor(Qt::SplitHCursor);
            }
          else {
              setCursor(Qt::PointingHandCursor);
            }
        }
      else if (QRectF(endBound - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2., HANDLE_WIDTH, HANDLE_HEIGHT).contains(event->pos())) {
          if (endBound != NO_BOUND) {
              setCursor(Qt::SplitHCursor);
            }
          else {
              setCursor(Qt::PointingHandCursor);
            }
        }
      else {
          setCursor(Qt::PointingHandCursor);
        }
    }
  else {
      if (QRectF(startX + (endX - startX) / 2 - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2, HANDLE_WIDTH, HANDLE_HEIGHT).contains(event->pos())
          && !_elasticMode) {
          setCursor(Qt::SplitHCursor);
        }
      else {
          setCursor(Qt::PointingHandCursor);
        }
    }
}

void
Relation::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  QGraphicsItem::hoverMoveEvent(event);

  double startX = mapFromScene(_start).x();
  double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();

  double endBound = NO_BOUND, startBound = NO_BOUND;
  if (_abstract->maxBound() != NO_BOUND) {
      endBound = startX + _abstract->maxBound() * _scene->zoom();
    }
  if (_abstract->minBound() != NO_BOUND) {
      startBound = startX + _abstract->minBound() * _scene->zoom();
    }
  if (_flexibleRelation) {
      if (QRectF(startBound - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2., HANDLE_WIDTH, HANDLE_HEIGHT).contains(event->pos())) {
          if (startBound != NO_BOUND) {
              setCursor(Qt::SplitHCursor);
            }
          else {
              setCursor(Qt::ArrowCursor);
            }
        }
      else if (QRectF(endBound - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2., HANDLE_WIDTH, HANDLE_HEIGHT).contains(event->pos())) {
          if (endBound != NO_BOUND) {
              setCursor(Qt::SplitHCursor);
            }
          else {
              setCursor(Qt::PointingHandCursor);
            }
        }
      else {
          setCursor(Qt::PointingHandCursor);
        }
    }
  else {
      if (QRectF(startX + (endX - startX) / 2 - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2, HANDLE_WIDTH, HANDLE_HEIGHT).contains(event->pos())
          && !_elasticMode) {
          setCursor(Qt::SplitHCursor);
        }
      else {
          setCursor(Qt::PointingHandCursor);
        }
    }
}

void
Relation::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  QGraphicsItem::hoverLeaveEvent(event);
  _hover = false;
  setCursor(Qt::ArrowCursor);
}

void
Relation::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);
  float maxBound;
  if (!_scene->playing()) {
      if (!_flexibleRelation) {
          _middleHandleSelected = false;
          _elasticMode = !_elasticMode;
          updateFlexibility();
        }
      else if (_abstract->maxBound() == NO_BOUND || _lastMaxBound != -1) {
          if (_lastMaxBound != -1) {
              maxBound = std::max(_lastMaxBound * _scene->zoom(), (float)(mapFromScene(_end).x() - mapFromScene(_start).x() + LINE_WIDTH));
            }
          else {
              maxBound = (mapFromScene(_end).x() - mapFromScene(_start).x() + LINE_WIDTH);
            }

          changeBounds(_abstract->minBound(), maxBound);
          _scene->changeRelationBounds(_abstract->ID(), NO_LENGTH, _abstract->minBound(), maxBound / _scene->zoom());
          _lastMaxBound = -1;
        }
      else {
          _lastMaxBound = _abstract->maxBound();
          changeBounds(_abstract->minBound(), NO_BOUND);
          _scene->changeRelationBounds(_abstract->ID(), NO_LENGTH, _abstract->minBound(), NO_BOUND);
        }
      update();
//        else{
//          RelationEdit * relEdit = new RelationEdit(_scene,_abstract->ID(),_scene->views().first());
//          relEdit->move(mapToScene(boundingRect().topLeft()).x(),mapToScene(boundingRect().topLeft()).y());
//          relEdit->exec();
//          delete relEdit;
//        }
    }
}

void
Relation::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem::mousePressEvent(event);

  if (!_scene->playing()) {
      if (cursor().shape() == Qt::SplitHCursor) {
          double startX = mapFromScene(_start).x();
          double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();          

          double startBound = startX;
          if (_abstract->minBound() != NO_BOUND) {              
              startBound = startX + _abstract->minBound() * _scene->zoom();
            }

          double endBound = endX;
          if (_abstract->maxBound() != NO_BOUND) {
              endBound = startX + _abstract->maxBound() * _scene->zoom();
            }

          /// \todo : Le QRectF middleHandle devrait être en attribut. NH
          if (!_flexibleRelation && QRectF(startX + (endX - startX) / 2 - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2, HANDLE_WIDTH, HANDLE_HEIGHT).contains(event->pos())) {
              _middleHandleSelected = true;
              _mouseClickPosSave = event->pos();
            }
          else if (QRectF(startBound - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2., HANDLE_WIDTH, HANDLE_HEIGHT).contains(event->pos())) {
              _leftHandleSelected = true;
            }
          else if (QRectF(endBound - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2., HANDLE_WIDTH, HANDLE_HEIGHT).contains(event->pos())) {
              _rightHandleSelected = true;
            }
        }
    }
}

void
Relation::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem::mouseMoveEvent(event);
  double eventPosX = mapFromScene(event->scenePos()).x();
  double startX = mapFromScene(_start).x();

  if (_leftHandleSelected) {
      changeBounds(_abstract->maxBound() == NO_BOUND ? std::max(eventPosX - startX, 0.) : std::min((float)std::max((eventPosX - startX)/_scene->zoom(), 0.), _abstract->maxBound()),
                   _abstract->maxBound());
      _scene->changeRelationBounds(_abstract->ID(), NO_LENGTH, _abstract->minBound(), _abstract->maxBound());
      update();
    }
  else if (_rightHandleSelected) {
      _scene->changeRelationBounds(_abstract->ID(), NO_LENGTH, _abstract->minBound(), std::max((float)std::max((eventPosX - startX)/_scene->zoom(), 0.), _abstract->minBound()));
      update();
    }
  else if (_middleHandleSelected) {
      BasicBox *rightBox = _scene->getBox(_abstract->secondBox());
      _scene->changeRelationBounds(_abstract->ID(), NO_LENGTH, NO_BOUND, NO_BOUND);
      qreal factorX = 2 * (eventPosX - _mouseClickPosSave.x());
      rightBox->moveBy(factorX, 0.);
      rightBox->setTopLeft(rightBox->getTopLeft() + QPointF(factorX, 0.));
      rightBox->updateStuff();
      _scene->boxMoved(rightBox->ID());
    }
}

void
Relation::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem::mouseReleaseEvent(event);
  if (_middleHandleSelected) {
      double startX = mapFromScene(_start).x();
      double endX = mapFromScene(_end).x();
      _scene->changeRelationBounds(_abstract->ID(), NO_LENGTH, (endX - startX) / _scene->zoom(), (endX - startX) / _scene->zoom());
      _middleHandleSelected = false;
    }
  _leftHandleSelected = false;
  _rightHandleSelected = false;
}

QRectF
Relation::boundingRect() const
{
  return QRectF(0. - ((fabs(_end.x() - _start.x())) / 2.), 0. - (std::max(fabs(_end.y() - _start.y()) / 2., (double)HANDLE_HEIGHT)) - TOLERANCE_Y,
                std::max(fabs(_end.x() - _start.x()), (double)_abstract->maxBound() * _scene->zoom() + HANDLE_WIDTH), std::max(fabs(_end.y() - _start.y()), (double)2. * HANDLE_HEIGHT) + 2 * TOLERANCE_Y);
}

QPainterPath
Relation::shape() const
{
  QPainterPath path;
  path.moveTo(mapFromScene(_start));
  double startX = mapFromScene(_start).x(), startY = mapFromScene(_start).y();
  double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();
  double sizeX = endX - startX, sizeY = endY - startY;
  double toleranceX = sizeX > 0 ? ARROW_SIZE : -ARROW_SIZE;
  double toleranceY = sizeY > 0 ? ARROW_SIZE : -ARROW_SIZE;

  // Handling zones
  double startBound = startX;
  if (_abstract->minBound() != NO_BOUND) {
      startBound = startX + _abstract->minBound() * _scene->zoom();
    }
  double endBound = endX;
  if (_abstract->maxBound() != NO_BOUND) {
      endBound = startX + _abstract->maxBound() * _scene->zoom();
    }

  path.moveTo(startX, startY + toleranceY);
  path.lineTo(startX + toleranceX, startY + toleranceY);
  path.lineTo(startX + toleranceX, endY - toleranceY);


  path.lineTo(_abstract->secondExtremity() == BOX_END ? mapFromScene(_scene->getBox(_abstract->secondBox())->getLeftGripPoint()).x() - HANDLE_WIDTH : endX - HANDLE_WIDTH, endY - toleranceY);

  path.lineTo(_abstract->secondExtremity() == BOX_END  ? mapFromScene(_scene->getBox(_abstract->secondBox())->getLeftGripPoint()).x() - HANDLE_WIDTH : endX - HANDLE_WIDTH, endY + toleranceY);
  path.lineTo(startX, endY + toleranceY);
  path.lineTo(startX, startY + toleranceY);

  if (_flexibleRelation && endBound > endX) {
      path.moveTo(endBound - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2);
      path.lineTo(endBound + HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2);
      path.lineTo(endBound + HANDLE_WIDTH / 2, endY + HANDLE_HEIGHT / 2);
      path.lineTo(endBound - HANDLE_WIDTH / 2, endY + HANDLE_HEIGHT / 2);
      path.lineTo(endBound - HANDLE_WIDTH / 2, endY - HANDLE_HEIGHT / 2);
    }

  return path;
}

void
Relation::updateFlexibility()
{
  if (_scene->getBox(_abstract->secondBox()) != nullptr) {
      if (_abstract->secondExtremity() == BOX_START && _scene->getBox(_abstract->secondBox())->hasTriggerPoint(BOX_START)) {
          _flexibleRelation = true;
        }

      else if (_abstract->secondExtremity() == BOX_END && _scene->getBox(_abstract->secondBox())->hasTriggerPoint(BOX_END)) {
          _flexibleRelation = true;
        }
      else {
          _flexibleRelation = false;
        }
    }
  else {
      _flexibleRelation = false;
    }

  double startX = mapFromScene(_start).x();
  double endX = mapFromScene(_end).x();

  if (_elasticMode) {
      _scene->changeRelationBounds(_abstract->ID(), NO_LENGTH, NO_BOUND, NO_BOUND);
    }
  else if (!_flexibleRelation) {
      _scene->changeRelationBounds(_abstract->ID(), NO_LENGTH, (endX - startX) / _scene->zoom(), (endX - startX) / _scene->zoom() + RIGID_TOLERANCE);
    }
  else {
      changeBounds(0, NO_BOUND);
      _scene->changeRelationBounds(_abstract->ID(), NO_LENGTH, _abstract->minBound(), _abstract->maxBound());
    }
}

void
Relation::drawRail(QPainter *painter, double startBound, double endBound)
{
  painter->save();

  double startY = mapFromScene(_start).y();
  double endY = mapFromScene(_end).y();

  QPen solidLine = QPen(Qt::SolidLine);
  solidLine.setWidth(LINE_WIDTH);
  solidLine.setColor(isSelected() ? _color : Qt::black);
  painter->setPen(solidLine);

  if (endY > startY) {
      painter->drawLine(startBound, endY + RAIL_WIDTH, endBound, endY + RAIL_WIDTH);
    }
  else {
      painter->drawLine(startBound, endY - RAIL_WIDTH, endBound, endY - RAIL_WIDTH);
    }

  painter->restore();
}

void
Relation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

//  painter->drawRect(boundingRect());
//  painter->drawPath(shape());

  QPainterPath painterPath;
  painterPath.moveTo(mapFromScene(_start));

  double startX = mapFromScene(_start).x(), startY = mapFromScene(_start).y();
  double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();
  double startBound = startX;
  double endBound = endX;

  QPen dotLine = QPen(Qt::DotLine);
  dotLine.setColor(isSelected() ? _color : Qt::black);
  dotLine.setWidth(isSelected() ? 1.2 * LINE_WIDTH : LINE_WIDTH);

  QPen solidLine = QPen(Qt::SolidLine);
  solidLine.setColor(isSelected() ? _color : Qt::black);
  solidLine.setWidth(isSelected() ? 1.2 * LINE_WIDTH : LINE_WIDTH);

  //grips' circles
  QPainterPath startCircle, endCircle;
  startCircle.addEllipse(_abstract->firstExtremity() == BOX_END ? startX : startX - GRIP_CIRCLE_SIZE, startY - GRIP_CIRCLE_SIZE / 2, GRIP_CIRCLE_SIZE, GRIP_CIRCLE_SIZE);
  endCircle.addEllipse(_abstract->secondExtremity() == BOX_START ? endX - GRIP_CIRCLE_SIZE : endX, endY - GRIP_CIRCLE_SIZE / 2, GRIP_CIRCLE_SIZE, GRIP_CIRCLE_SIZE);
  painter->fillPath(startCircle, QBrush(isSelected() ? _color : Qt::black));
  painter->fillPath(endCircle, QBrush(isSelected() ? _color : Qt::black));


  //----------------------- Flexible relation --------------------------//
  if (_flexibleRelation) {

      /************ Draw relation ************/
      painter->setPen(solidLine);

      //Horizontal : Point to box
      painter->drawLine(_abstract->firstExtremity() == BOX_END ? startX : startX - GRIP_CIRCLE_SIZE / 2, startY, startX, startY);

      //Vertical line
      painter->drawLine(startX, startY, startX, endY);


      /****************** bounds ******************/
      QPen rightBoundPen;
      rightBoundPen.setWidth(isSelected() ? 1.2 * LINE_WIDTH : LINE_WIDTH);
      rightBoundPen.setStyle(Qt::SolidLine);
      rightBoundPen.setColor(isSelected() ? _color : Qt::black);

      QPen leftBoundPen;
      leftBoundPen.setWidth(isSelected() ? 1.2 * LINE_WIDTH : LINE_WIDTH);
      leftBoundPen.setStyle(Qt::SolidLine);
      leftBoundPen.setColor(isSelected() ? _color : Qt::black);


      if (_abstract->minBound() != NO_BOUND) {
          startBound = (startX + _abstract->minBound() * _scene->zoom());
        }

      if (_abstract->maxBound() != NO_BOUND) {
          endBound = (startX + _abstract->maxBound() * _scene->zoom());
        }

      // Left Handle
      painter->setPen(leftBoundPen);
      if (_abstract->minBound() != NO_BOUND) {
          painter->drawLine(QPointF(startBound, endY - HANDLE_HEIGHT / 2), QPointF(startBound, endY + HANDLE_HEIGHT / 2)); // Vertical left
        }

      // Right Handle
      painter->setPen(rightBoundPen);
      if (_abstract->maxBound() != NO_BOUND) {
          painter->drawLine(QPointF(endBound, endY - HANDLE_HEIGHT / 2), QPointF(endBound, endY + HANDLE_HEIGHT / 2)); // Vertical
        }

      //horizontal line
      painter->setPen(solidLine);
      painter->drawLine(startX, endY, startBound, endY);

      painter->setPen(dotLine);
      painter->drawLine(startBound, endY, endBound, endY);

      if (_abstract->maxBound() != NO_BOUND) {
          drawRail(painter, startBound, endBound);
        }
    }


  //-------------------------- Rigid relation --------------------------//
  else {

      //Duration text
      if(_hover){
          painter->save();
          QFont textFont;
          textFont.setPointSize(10.);
          painter->setFont(textFont);
          painter->setPen(solidLine);
          painter->drawText(QPoint(startX + (endX - startX) - 35 ,endY - HANDLE_HEIGHT/2. -1), QString("%1").arg(duration()));
          painter->restore();
      }

      //Vertical line
      painter->setPen(solidLine);
      painter->drawLine(_abstract->firstExtremity() == BOX_END ? startX : startX - GRIP_CIRCLE_SIZE / 2, startY, startX, startY);
      painter->drawLine(startX, startY, startX, endY);

      //horizontal line
      if (!_elasticMode) {
          painter->drawLine(startX, endY, _abstract->secondExtremity() == BOX_END ? endX + GRIP_CIRCLE_SIZE / 2 : endX - GRIP_CIRCLE_SIZE, endY);

          //handle
          QPen handlePen;
          handlePen.setWidth(LINE_WIDTH);
          handlePen.setStyle(Qt::SolidLine);
          handlePen.setColor(isSelected() ? _color : Qt::black);
          painter->setPen(handlePen);
          painter->drawLine(startX + (endX - startX) / 2, endY - HANDLE_HEIGHT / 2, startX + (endX - startX) / 2, endY + HANDLE_HEIGHT / 2);
      }
      else {
          double handleZone = 10;
          painter->drawLine(startX, endY, startX + (endX - startX) / 2 - handleZone, endY);
          painter->setPen(dotLine);
          painter->drawLine(startX + (endX - startX) / 2 - handleZone, endY, startX + (endX - startX) / 2 + handleZone, endY);
          painter->setPen(solidLine);
          painter->drawLine(startX + (endX - startX) / 2 + handleZone, endY, _abstract->secondExtremity() == BOX_END ? endX + GRIP_CIRCLE_SIZE / 2 : endX - GRIP_CIRCLE_SIZE, endY);
        }
    }
}

float
Relation::duration(){
    return (mapFromScene(_end).x() - mapFromScene(_start).x())*MaquetteScene::MS_PER_PIXEL / 1000;
}
