/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin (08/03/2010)

luc.vercellin@labri.fr

This software is a computer program whose purpose is to provide
notation/composition combining synthesized as well as recorded
sounds, providing answers to the problem of notation and, drawing,
from its very design, on benefits from state of the art research
in musicology and sound/music computing.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/
#include "Relation.hpp"
#include "RelationEdit.hpp"
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

const float Relation::ARROW_SIZE = 12.;
const float Relation::TOLERANCE_X = 12.;
const float Relation::TOLERANCE_Y = 12.;
const float Relation::HANDLE_HEIGHT = 25.;
const float Relation::HANDLE_WIDTH = 12.;
const float Relation::GRIP_CIRCLE_SIZE = 4;

Relation::Relation(unsigned int firstBoxID, BoxExtremity firstBoxExt, unsigned int secondBoxID,
		   BoxExtremity secondBoxExt, MaquetteScene *parent)
  : QGraphicsItem()
{
  _scene = parent;

  _abstract = new AbstractRelation(firstBoxID,firstBoxExt,secondBoxID,secondBoxExt,NO_ID);

  init();

  //setPos(getCenter());
}

Relation::Relation(const AbstractRelation &abstract, MaquetteScene *parent)
  : QGraphicsItem()
{
  _scene = parent;
  _abstract = new AbstractRelation(abstract);

  init();

  //setPos(getCenter());
}

Relation::~Relation() {
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
  setAcceptsHoverEvents(true);
  setZValue(0);
  _leftHandleSelected = false;
  _rightHandleSelected = false;
  _flexibleRelation = _scene->getBox(_abstract->secondBox())->hasTriggerPoint(BOX_START) || _scene->getBox(_abstract->secondBox())->hasTriggerPoint(BOX_END);
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
Relation::toString() {
  QString ret;
  ret = "'" + _scene->getBox(_abstract->firstBox())->name() + "'";
  switch (_abstract->firstExtremity()) {
  case BOX_START :
    ret += QObject::tr(" start");
    break;
  case BOX_END :
    ret += QObject::tr(" end");
    break;
  case NO_EXTREMITY :
    ret += QObject::tr(" no extremity");
    break;
  default :
    ret += QObject::tr(" unknown extremity");
    break;
  }

  ret += QObject::tr(" [Before] ");
  ret += "'" + _scene->getBox(_abstract->secondBox())->name() + "'";

  switch (_abstract->secondExtremity()) {
  case BOX_START :
    ret += QObject::tr(" start");
    break;
  case BOX_END :
    ret += QObject::tr(" end");
    break;
  case NO_EXTREMITY :
    ret += QObject::tr(" no extremity");
    break;
  default :
    ret += QObject::tr(" unknown extremity");
    break;
  }

  return ret;
}

void
Relation::updateCoordinates()
{
  BasicBox *box = _scene->getBox(_abstract->firstBox());

  switch (_abstract->firstExtremity()) {
  case BOX_START :
//    _start = box->getMiddleLeft();
      _start = box->getLeftGripPoint();

    break;
  case BOX_END :
//    _start = box->getMiddleRight();
      _start = box->getRightGripPoint();
    break;
  case NO_EXTREMITY :
    _start = box->getCenter();
    break;
  }

  box = _scene->getBox(_abstract->secondBox());
  switch (_abstract->secondExtremity()) {
  case BOX_START :
      if(box!=NULL){
//        _end = box->getMiddleLeft();
        _end = box->getLeftGripPoint();
      }
    break;
  case BOX_END :
      if(box!=NULL){
//        _end = box->getMiddleRight();
          _end = box->getRightGripPoint();
      }
    break;
  case NO_EXTREMITY :
      if(box!=NULL)
        _end = box->getCenter();
    break;
  }

  _abstract->_length = _end.x() - _start.x();
  setPos(getCenter());

}

QPointF
Relation::getCenter() const
{
  return QPointF(_start.x() + (_end.x() - _start.x())/2., _start.y() + (_end.y() - _start.y())/2.);
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
Relation::changeBounds(const float &minBound, const float &maxBound) {
	_abstract->setMinBound(minBound);
	_abstract->setMaxBound(maxBound);
}

void
Relation::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
  QGraphicsItem::hoverEnterEvent(event);
  double startX = mapFromScene(_start).x()+BasicBox::EAR_WIDTH/2 , startY = mapFromScene(_start).y();
  double endY = mapFromScene(_end).y();
  double sizeY = endY - startY;
  double centerY = startY + sizeY/2.;


  double endBound = NO_BOUND, startBound = NO_BOUND;
  if (_abstract->maxBound() != NO_BOUND) {
    endBound = startX + _abstract->maxBound();
  }
  if (_abstract->minBound() != NO_BOUND) {
    startBound = startX + _abstract->minBound();
  }

  if (_flexibleRelation){

      if (QRectF(startBound - HANDLE_WIDTH/2,endY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
        if (startBound != NO_BOUND) {
            setCursor(Qt::SplitHCursor);
        }
        else {
            setCursor(Qt::PointingHandCursor);
        }
      }
      else if (QRectF(endBound - HANDLE_WIDTH/2,endY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
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
}

void
Relation::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )
{
  QGraphicsItem::hoverMoveEvent(event);

  double startX = mapFromScene(_start).x()/*+BasicBox::EAR_WIDTH/2*/, startY = mapFromScene(_start).y();
  double endY = mapFromScene(_end).y();
  double sizeY = endY - startY;

  double endBound = NO_BOUND, startBound = NO_BOUND;
  if (_abstract->maxBound() != NO_BOUND) {
    endBound = startX + _abstract->maxBound();
  }
  if (_abstract->minBound() != NO_BOUND) {
    startBound = startX + _abstract->minBound();
  }
  if(_flexibleRelation){
      if (QRectF(startBound - HANDLE_WIDTH/2,endY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
        if (startBound != NO_BOUND) {
            setCursor(Qt::SplitHCursor);
        }
        else {
            setCursor(Qt::ArrowCursor);
        }
      }
      else if (QRectF(endBound - HANDLE_WIDTH/2,endY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
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
}

void
Relation::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event )
{
	QGraphicsItem::hoverLeaveEvent(event);
	setCursor(Qt::ArrowCursor);
}

void
Relation::mouseDoubleClickEvent (QGraphicsSceneMouseEvent * event) {
  QGraphicsItem::mouseDoubleClickEvent(event);
  if (!_scene->playing()) {
  	RelationEdit * relEdit = new RelationEdit(_scene,_abstract->ID(),_scene->views().first());
  	relEdit->move(mapToScene(boundingRect().topLeft()).x(),mapToScene(boundingRect().topLeft()).y());
  	relEdit->exec();
  	delete relEdit;    
	}
}

void
Relation::mousePressEvent (QGraphicsSceneMouseEvent * event) {
    QGraphicsItem::mousePressEvent(event);

    if (!_scene->playing()) {
        if (cursor().shape() == Qt::SplitHCursor) {
          double startX = mapFromScene(_start).x()/*+BasicBox::EAR_WIDTH/2*/ , startY = mapFromScene(_start).y();
          double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();

          double startBound = startX;
          if (_abstract->minBound() != NO_BOUND) {
            startBound = startX + _abstract->minBound();
          }

          double endBound = endX;
          if (_abstract->maxBound() != NO_BOUND) {
            endBound = startX + _abstract->maxBound();
          }

          if (QRectF(startBound - HANDLE_WIDTH/2 ,endY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
            _leftHandleSelected = true;
          }
          else if (QRectF(endBound - HANDLE_WIDTH/2,endY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
            _rightHandleSelected = true;
          }
        }
    }
}

void
Relation::mouseMoveEvent (QGraphicsSceneMouseEvent * event) {
  QGraphicsItem::mouseMoveEvent(event);
  double eventPosX = mapFromScene(event->scenePos()).x();
  double startX = mapFromScene(_start).x()/*+BasicBox::EAR_WIDTH/2*/;
  if (_leftHandleSelected) {
    _scene->changeRelationBounds(_abstract->ID(),NO_LENGTH,std::min((float)std::max(eventPosX - startX,0.),_abstract->maxBound()),_abstract->maxBound());
  	update();
  }
  else if (_rightHandleSelected) {
  	_scene->changeRelationBounds(_abstract->ID(),NO_LENGTH,_abstract->minBound(),std::max((float)std::max(eventPosX - startX,0.),_abstract->minBound()));
  	update();
  }
}

void
Relation::mouseReleaseEvent (QGraphicsSceneMouseEvent * event) {
  QGraphicsItem::mouseMoveEvent(event);
	_leftHandleSelected = false;
	_rightHandleSelected = false;
}

QRectF
Relation::boundingRect() const
{
  return QRectF(0.-((fabs(_end.x() - _start.x()))/2.),0.-(std::max(fabs(_end.y() - _start.y())/2.,(double)HANDLE_HEIGHT)) - TOLERANCE_Y,
        std::max(fabs(_end.x() - _start.x()),(double)_abstract->maxBound()+HANDLE_WIDTH),std::max(fabs(_end.y() - _start.y()),(double)2.*HANDLE_HEIGHT) + 2*TOLERANCE_Y);
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
    startBound = startX + _abstract->minBound();
  }
  double endBound = endX;
  if (_abstract->maxBound() != NO_BOUND) {
    endBound = startX + _abstract->maxBound();
  }

  path.moveTo(startX,startY + toleranceY);
  path.lineTo(startX + toleranceX, startY + toleranceY);
  path.lineTo(startX + toleranceX, endY - toleranceY);
  path.lineTo(endX - HANDLE_WIDTH, endY - toleranceY);

  path.lineTo(endX - HANDLE_WIDTH, endY + toleranceY);
  path.lineTo(startX, endY + toleranceY);
  path.lineTo(startX,startY + toleranceY);  

  if(_flexibleRelation && endBound>endX+HANDLE_WIDTH/2){
      path.moveTo(endBound - HANDLE_WIDTH/2,endY - HANDLE_HEIGHT/2);
      path.lineTo(endBound + HANDLE_WIDTH/2,endY - HANDLE_HEIGHT/2);
      path.lineTo(endBound + HANDLE_WIDTH/2,endY + HANDLE_HEIGHT/2);
      path.lineTo(endBound - HANDLE_WIDTH/2,endY + HANDLE_HEIGHT/2);
      path.lineTo(endBound - HANDLE_WIDTH/2,endY - HANDLE_HEIGHT/2);
  }



  return path;
}

void
Relation::updateFlexibility(){

    _flexibleRelation = _scene->getBox(_abstract->secondBox())->hasTriggerPoint(BOX_START) || _scene->getBox(_abstract->secondBox())->hasTriggerPoint(BOX_END);
    double startX = mapFromScene(_start).x();
    double endX = mapFromScene(_end).x();

    if (!_flexibleRelation){
        changeBounds(endX-startX,endX-startX);
        _scene->changeRelationBounds(_abstract->ID(),NO_LENGTH,endX-startX,endX-startX);
    }
    else{
        changeBounds(NO_BOUND,NO_BOUND);
        _scene->changeRelationBounds(_abstract->ID(),NO_LENGTH,_abstract->minBound(),_abstract->maxBound());

    }
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

    double startX = mapFromScene(_start).x()/* + BasicBox::EAR_WIDTH/2*/, startY = mapFromScene(_start).y() ;
    double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();
    double startBound = startX;
    double endBound = endX;

    QPen dotLine = QPen(Qt::DotLine);
    dotLine.setWidth(isSelected() ? 1.5 * BasicBox::LINE_WIDTH : BasicBox::LINE_WIDTH);
    QPen solidLine = QPen(Qt::SolidLine);
    solidLine.setWidth(isSelected() ? 1.5 * BasicBox::LINE_WIDTH : BasicBox::LINE_WIDTH);

    //grips' circles
    QPainterPath circle;
    circle.addEllipse( _abstract->firstExtremity() == BOX_END ? startX : startX-GRIP_CIRCLE_SIZE ,startY - GRIP_CIRCLE_SIZE/2,GRIP_CIRCLE_SIZE,GRIP_CIRCLE_SIZE);
    painter->fillPath(circle,QBrush(Qt::black));


    //----------------------- Flexible relation --------------------------//

    if (_flexibleRelation){

        /************ Draw relation ************/

        //Vertical line
        painter->setPen(solidLine);
        painter->drawLine(startX,startY,startX,endY);


        //horizontal line
        painter->setPen(dotLine);
        painter->drawLine(startX,endY,endX - BasicBox::EAR_WIDTH/2,endY);


        /****************** bounds ******************/
        QPen rightBoundPen;
        rightBoundPen.setWidth(isSelected() ? 2 * BasicBox::LINE_WIDTH : BasicBox::LINE_WIDTH);
        //  rightBoundPen.setColor(Qt::cyan);
        rightBoundPen.setStyle(Qt::DotLine);

        QPen leftBoundPen;
        leftBoundPen.setWidth(isSelected() ? 2 * BasicBox::LINE_WIDTH : BasicBox::LINE_WIDTH);
        leftBoundPen.setColor(Qt::blue);


        if (_abstract->minBound() != NO_BOUND)
            startBound = startX + _abstract->minBound();

        if (_abstract->maxBound() != NO_BOUND)
            endBound = startX + _abstract->maxBound();


        painter->setPen(leftBoundPen);
        //  painter->drawLine(QPointF(startX,endY), QPointF(startBound,endY)); // Horizontal
        if (_abstract->minBound() != NO_BOUND) { // Left Handle
            painter->drawLine(QPointF(startBound,endY - HANDLE_HEIGHT/2), QPointF(startBound,endY + HANDLE_HEIGHT/2)); // Vertical left
        }

        painter->setPen(rightBoundPen);
        //  painter->drawLine(QPointF(startBound,centerY), QPointF(endBound,centerY)); // Horizontal
        //  painter->drawLine(QPointF(startBound,endY), QPointF(endBound,endY)); // Horizontal
        if (_abstract->maxBound() != NO_BOUND) {
            painter->drawLine(QPointF(endBound,endY - HANDLE_HEIGHT/2), QPointF(endBound,endY + HANDLE_HEIGHT/2)); // Vertical
          }
    }


    //-------------------------- Rigid relation --------------------------//
    else{
        //Vertical line
        painter->setPen(solidLine);
        painter->drawLine(startX,startY,startX,endY);

        //horizontal line
        painter->drawLine(startX,endY,endX - BasicBox::EAR_WIDTH/2,endY);

        _scene->changeRelationBounds(_abstract->ID(),NO_LENGTH,endX-startX,endX-startX);
    }
}


