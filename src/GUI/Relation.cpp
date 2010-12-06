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
const float Relation::HANDLE_HEIGHT = 50.;
const float Relation::HANDLE_WIDTH = 12.;

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
    ret += " start";
    break;
  case BOX_END :
    ret += " end";
    break;
  case NO_EXTREMITY :
    ret += " no extremity";
    break;
  default :
    ret += " unknown extremity";
    break;
  }

  ret += " [Before] ";
  ret += "'" + _scene->getBox(_abstract->secondBox())->name() + "'";

  switch (_abstract->secondExtremity()) {
  case BOX_START :
    ret += " start";
    break;
  case BOX_END :
    ret += " end";
    break;
  case NO_EXTREMITY :
    ret += " no extremity";
    break;
  default :
    ret += " unknown extremity";
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
    _start = box->getMiddleLeft();
    break;
  case BOX_END :
    _start = box->getMiddleRight();
    break;
  case NO_EXTREMITY :
    _start = box->getCenter();
    break;
  }

  box = _scene->getBox(_abstract->secondBox());
  switch (_abstract->secondExtremity()) {
  case BOX_START :
    _end = box->getMiddleLeft();
    break;
  case BOX_END :
    _end = box->getMiddleRight();
    break;
  case NO_EXTREMITY :
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
  double startX = mapFromScene(_start).x() , startY = mapFromScene(_start).y();
  double endY = mapFromScene(_end).y();
  double sizeY = endY - startY;
  double centerY = startY + sizeY/2.;
  double startBound = NO_BOUND;
  if (_abstract->minBound() != NO_BOUND) {
  	startBound = startX + _abstract->minBound();
  }
  double endBound = NO_BOUND;
  if (_abstract->maxBound() != NO_BOUND) {
  	endBound = startX + _abstract->maxBound();
  }
  if (QRectF(startBound,centerY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
  	if (startBound != NO_BOUND) {
  		setCursor(Qt::SplitHCursor);
  	}
  	else {
    	setCursor(Qt::ArrowCursor);
  	}
  }
  else if (QRectF(endBound - HANDLE_WIDTH,centerY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
  	if (endBound != NO_BOUND) {
  		setCursor(Qt::SplitHCursor);
  	}
    else {
    	setCursor(Qt::ArrowCursor);
    }
  }
  else {
  	setCursor(Qt::ArrowCursor);
  }
}

void
Relation::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )
{
	QGraphicsItem::hoverMoveEvent(event);
  double startX = mapFromScene(_start).x() , startY = mapFromScene(_start).y();
  double endY = mapFromScene(_end).y();
  double sizeY = endY - startY;
  double centerY = startY + sizeY/2.;
  double startBound = NO_BOUND;
  if (_abstract->minBound() != NO_BOUND) {
  	startBound = startX + _abstract->minBound();
  }
  double endBound = NO_BOUND;
  if (_abstract->maxBound() != NO_BOUND) {
  	endBound = startX + _abstract->maxBound();
  }
  if (QRectF(startBound,centerY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
  	if (startBound != NO_BOUND) {
  		setCursor(Qt::SplitHCursor);
  	}
  	else {
    	setCursor(Qt::PointingHandCursor);
  	}
  }
  else if (QRectF(endBound - HANDLE_WIDTH,centerY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
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
  	  double startX = mapFromScene(_start).x() , startY = mapFromScene(_start).y();
  	  double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();
  	  double sizeY = endY - startY;
  	  double centerY = startY + sizeY/2.;
  	  double startBound = startX;
  	  if (_abstract->minBound() != NO_BOUND) {
  	  	startBound = startX + _abstract->minBound();
  	  }
  	  double endBound = endX;
  	  if (_abstract->maxBound() != NO_BOUND) {
  	  	endBound = startX + _abstract->maxBound();
  	  }
  	  if (QRectF(startBound,centerY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
  	  	_leftHandleSelected = true;
  	  }
  	  else if (QRectF(endBound - HANDLE_WIDTH,centerY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT).contains(event->pos())) {
  	  	_rightHandleSelected = true;
  	  }
  	}
	}
}

void
Relation::mouseMoveEvent (QGraphicsSceneMouseEvent * event) {
  QGraphicsItem::mouseMoveEvent(event);
  double eventPosX = mapFromScene(event->scenePos()).x();
  double startX = mapFromScene(_start).x();
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
  return QRectF(0.-((fabs(_end.x() - _start.x()))/2.),0.-(std::max(fabs(_end.y() - _start.y())/2.,(double)HANDLE_HEIGHT)),
		std::max(fabs(_end.x() - _start.x()),(double)_abstract->maxBound()),std::max(fabs(_end.y() - _start.y()),(double)2.*HANDLE_HEIGHT));
}

QPainterPath
Relation::shape () const
{
  QPainterPath path;
  path.moveTo(mapFromScene(_start));
  double startX = mapFromScene(_start).x() , startY = mapFromScene(_start).y();
  double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();
  double sizeX = endX - startX, sizeY = endY - startY;
  double centerX = startX + sizeX/2., centerY = startY + sizeY/2.;
  double toleranceX = sizeX > 0 ? 2*ARROW_SIZE : -2*ARROW_SIZE;
  double toleranceY = sizeY > 0 ? 2*ARROW_SIZE : -2*ARROW_SIZE;

  path.moveTo(startX,startY - toleranceY);
  if (sizeX >= 2 * ARROW_SIZE) {
    path.lineTo(startX + ARROW_SIZE, startY - toleranceY);
  }

  path.quadTo(centerX + toleranceX, startY - toleranceY, centerX + toleranceX, centerY - toleranceY);
  if (sizeX >= 2 * ARROW_SIZE) {
    path.quadTo(centerX + toleranceX, endY - toleranceY, endX - ARROW_SIZE, endY - toleranceY);
    path.lineTo(endX,endY - toleranceY);
  }
  else {
    path.quadTo(centerX + toleranceX, endY - toleranceY, endX, endY - toleranceY);
  }

  path.lineTo(endX , endY + toleranceY);

  if (sizeX >= 2 * ARROW_SIZE) {
    path.lineTo(endX - toleranceX,endY + toleranceY);
  }

  path.quadTo(centerX - toleranceX, endY + toleranceY,centerX - toleranceX,centerY + toleranceY);
  if (sizeX >= 2 * ARROW_SIZE) {
    path.quadTo(centerX - toleranceX ,startY + toleranceY,startX + toleranceX,startY + toleranceY);
    path.lineTo(startX,startY + toleranceY);
  }
  else {
    path.quadTo(centerX - toleranceX, startY + toleranceY, startX, startY + toleranceY);
  }
  path.lineTo(startX,startY - toleranceY);

  // Handling zones
  double startBound = startX;
  if (_abstract->minBound() != NO_BOUND) {
  	startBound = startX + _abstract->minBound();
  }
  double endBound = endX;
  if (_abstract->maxBound() != NO_BOUND) {
  	endBound = startX + _abstract->maxBound();
  }
  path.moveTo(startBound,centerY - HANDLE_HEIGHT/2.);
  path.addRect(startBound,centerY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT);
  path.moveTo(endBound - HANDLE_WIDTH,centerY - HANDLE_HEIGHT/2.);
  path.addRect(endBound - HANDLE_WIDTH,centerY - HANDLE_HEIGHT/2.,HANDLE_WIDTH,HANDLE_HEIGHT);

  return path;
}

void
Relation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  //painter->drawRect(boundingRect());
  //painter->drawPath(shape());

  QPainterPath painterPath;
  painterPath.moveTo(mapFromScene(_start));
  double startX = mapFromScene(_start).x() , startY = mapFromScene(_start).y();
  double endX = mapFromScene(_end).x(), endY = mapFromScene(_end).y();
  double sizeY = endY - startY;
  double centerY = startY + sizeY/2.;

  if (fabs(startX-endX) >= 2*ARROW_SIZE) {
    if (startX <= endX) {
      painterPath.lineTo(startX + ARROW_SIZE,startY);
    }
    else {
      painterPath.lineTo(startX - ARROW_SIZE,startY);
    }
  }

  painterPath.quadTo((startX + endX)/2.,startY,(startX + endX)/2.,(startY + endY)/2.);
  if (fabs(startX-endX) >= 2*ARROW_SIZE) {
    if (startX <= endX) {
      painterPath.quadTo((startX + endX)/2.,endY,endX - ARROW_SIZE,endY);
    }
    else {
      painterPath.quadTo((startX + endX)/2.,endY,endX + ARROW_SIZE,endY);
    }
  }
  else {
    painterPath.quadTo((startX + endX)/2.,endY,endX,endY);
  }

  painter->save();
  QPen localPen;
  localPen.setWidth(isSelected() ? 2 * BasicBox::LINE_WIDTH : BasicBox::LINE_WIDTH);
  localPen.setStyle(Qt::DashDotLine);
  painter->setPen(localPen);
  painter->drawPath(painterPath);
  painter->restore();
  painterPath = QPainterPath();
  if (startX <= endX) {
    painterPath.moveTo(endX,endY);
    painterPath.lineTo(endX-ARROW_SIZE,endY - (ARROW_SIZE/2.));
    painterPath.lineTo(endX-ARROW_SIZE,endY + (ARROW_SIZE/2.));
    painterPath.lineTo(endX,endY);
  }
  else {
    painterPath.moveTo(endX,endY);
    painterPath.lineTo(endX+ARROW_SIZE,endY - (ARROW_SIZE/2.));
    painterPath.lineTo(endX+ARROW_SIZE,endY + (ARROW_SIZE/2.));
    painterPath.lineTo(endX,endY);
  }
  painter->fillPath(painterPath,QColor(60,60,60));
  painter->setBrush(QBrush(Qt::red));

  QPen rightBoundPen;
  rightBoundPen.setWidth(isSelected() ? 2 * BasicBox::LINE_WIDTH : BasicBox::LINE_WIDTH);
  rightBoundPen.setColor(Qt::cyan);
	rightBoundPen.setStyle(Qt::DotLine);
  QPen leftBoundPen;
  leftBoundPen.setWidth(isSelected() ? 2 * BasicBox::LINE_WIDTH : BasicBox::LINE_WIDTH);
  leftBoundPen.setColor(Qt::blue);
  if (_abstract->minBound() == NO_BOUND) {
  	if (_abstract->maxBound() == NO_BOUND) {
  		// All Dashes
  		leftBoundPen.setStyle(Qt::DotLine);
  	}
  	else {
  		// Dashes Left / Plain right
  		leftBoundPen.setStyle(Qt::DotLine);
  	}
  }
  else { // MinBound has value
  	if (_abstract->maxBound() == NO_BOUND) {
  		// Plain Left / Dashes right
  		leftBoundPen.setStyle(Qt::SolidLine);
  	}
  	else {
  		// All plain
  		leftBoundPen.setStyle(Qt::SolidLine);
  	}
  }


  double startBound = startX;
  if (_abstract->minBound() != NO_BOUND) {
  	startBound = startX + _abstract->minBound();
  }
  double endBound = endX;
  if (_abstract->maxBound() != NO_BOUND) {
  	endBound = startX + _abstract->maxBound();
  }

  painter->setPen(leftBoundPen);
  painter->drawLine(QPointF(startX,centerY), QPointF(startBound,centerY)); // Horizontal
  if (_abstract->minBound() != NO_BOUND) { // Left Handle
  	painter->drawLine(QPointF(startBound,centerY - HANDLE_HEIGHT/2.), QPointF(startBound,centerY + HANDLE_HEIGHT/2.)); // Vertical Left
  }

  painter->setPen(rightBoundPen);
  painter->drawLine(QPointF(startBound,centerY), QPointF(endBound,centerY)); // Horizontal
  if (_abstract->maxBound() != NO_BOUND) {
  	painter->drawLine(QPointF(endBound,centerY - HANDLE_HEIGHT/2.), QPointF(endBound,centerY + HANDLE_HEIGHT/2.)); // Vertical
  }
}
