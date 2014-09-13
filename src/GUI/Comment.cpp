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
#include "Comment.hpp"
#include "MaquetteScene.hpp"
#include "BasicBox.hpp"
#include "AbstractComment.hpp"

#include <iostream>
#include <math.h>

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QRectF>
#include <QString>
#include <QMenu>
#include <QInputMethodEvent>
using std::string;

Comment::Comment(const string &text, unsigned int boxID, MaquetteScene *parent)
  : QGraphicsTextItem(QString::fromStdString(text)/*, parent->focusItem(), parent*/)
{
  _scene = parent;

  _abstract = new AbstractComment(text, boxID);

  init();
}

Comment::Comment(const AbstractComment &abstract, MaquetteScene *parent)
  : QGraphicsTextItem(QString::fromStdString(abstract.text())/*, parent->focusItem(), parent*/)
{
  _scene = parent;
  _abstract = new AbstractComment(abstract);

  init();
}

Comment::~Comment()
{
  delete _abstract;
}

void
Comment::init()
{
  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setTextInteractionFlags(Qt::TextEditable);
  setTextInteractionFlags(Qt::TextEditorInteraction);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  if (_abstract->ID() != NO_ID) {
      setFlag(QGraphicsItem::ItemIsMovable, false);
    }
  else {
      setFlag(QGraphicsItem::ItemIsMovable, true);
    }
  setVisible(true);
  setZValue(0);
}

Abstract *
Comment::abstract() const
{
  _abstract->setText(toPlainText().toStdString());
  _abstract->setPos(pos());

  return _abstract;
}

int
Comment::type() const
{
  return COMMENT_TYPE;
}

void
Comment::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  /// \todo Old TODO updated (by jC): solve position BUG
  QRectF comRect = boundingRect();
  painter->save();
  QPen pen = QPen(Qt::darkYellow, BasicBox::LINE_WIDTH);
  painter->setPen(pen);
  painter->setBrush(Qt::yellow);

  BasicBox *box = NULL;
  if (_abstract->ID() != NO_ID) {
      if ((box = _scene->getBox(_abstract->ID())) != NULL) {
          QPainterPath path;
          path.moveTo(comRect.topLeft() - QPointF(0., 20.));
          path.lineTo(comRect.topLeft());
          path.lineTo(comRect.bottomLeft());
          path.lineTo(comRect.bottomRight());
          path.lineTo(comRect.topRight());
          path.lineTo(comRect.topLeft() + QPointF(4., 0.));
          path.lineTo(comRect.topLeft() - QPointF(0., 20.));
          painter->drawPath(path);
        }
    }
  else {
      painter->drawRect(boundingRect());
    }
  QGraphicsTextItem::paint(painter, option, widget);

  painter->restore();
}

unsigned int
Comment::ID() const
{
  return _abstract->ID();
}

void
Comment::setID(unsigned int ID)
{
  _abstract->setID(ID);
}

void
Comment::updatePos()
{
  if (_abstract->ID() != NO_ID) {
      BasicBox *box = _scene->getBox(_abstract->ID());
      if (box != NULL) {
          setPos(QPointF(box->mapFromScene(box->getTopLeft()).x(), box->mapFromScene(box->getBottomRight()).y() + boundingRect().height()));
        }
    }
  else {
      std::cerr << "Comment::updatePos : NO_ID detected" << std::endl;
    }
  update();
}

void
Comment::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  setSelected(false);
  QGraphicsTextItem::mousePressEvent(event);
  update();
}

