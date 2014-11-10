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

#include "ConditionalRelation.hpp"
#include "TriggerPoint.hpp"
#include <QDebug>
const QColor ConditionalRelation::CONDITIONAL_RELATION_COLOR = QColor(0, 0, 0);
const QColor ConditionalRelation::CONDITIONAL_RELATION_SELECTED_COLOR = QColor(Qt::blue);
const float ConditionalRelation::BOUNDING_RECT_WIDTH = 10.;
const float ConditionalRelation::GRIP_SHIFT = 4.;

ConditionalRelation::ConditionalRelation(QList<BasicBox *> boxesAttached, MaquetteScene *parent)
    : QGraphicsObject(), _scene(parent), _boxesAttached(boxesAttached)
{
    init();

    _id = Maquette::getInstance()->createCondition(boxesAttached);        


    //set conditional relation to each box
    QList<BasicBox *>::iterator     it;
    for(it = _boxesAttached.begin() ; it!=_boxesAttached.end() ; it++)
    {
        (*it)->addConditionalRelation(this);

        //default : auto-trigger = false for all triggerPoints attached
        Maquette::getInstance()->setTriggerPointDefault((*it)->getTriggerPoint(BOX_START)->ID(),false);
    }

    updateCoordinates();
}

ConditionalRelation::ConditionalRelation(unsigned int id, QList<BasicBox *> boxesAttached, MaquetteScene *parent)
    : QGraphicsObject(), _id(id), _scene(parent), _boxesAttached(boxesAttached)
{
    init();

    _scene->addItem(this);

    //set conditional relation to each box
    QList<BasicBox *>::iterator     it;
    for(it = _boxesAttached.begin() ; it!=_boxesAttached.end() ; it++)
        (*it)->addConditionalRelation(this);

    updateCoordinates();
}

void
ConditionalRelation::init (){

    _color = CONDITIONAL_RELATION_COLOR;
    _selectedColor = CONDITIONAL_RELATION_SELECTED_COLOR;

    _mousePressed = false;

    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemIsMovable, false); /// \todo Implement the relation move on mousePressed && moving
    setZValue(1);

    _scene->addItem(this);
}

void
ConditionalRelation::updateCoordinates()
{
    /// \todo Maquette::getInstance()->getConditionDate(ID()); then update start/end;
    if (nbOfAttachedBoxes() > 1) {
        QPair<BasicBox *, BasicBox *> lowestHighestBoxes = getLowestHighestBoxes();
        _start = lowestHighestBoxes.first->getLeftGripPoint() - QPointF(GRIP_SHIFT,0);
        _end = lowestHighestBoxes.second->getLeftGripPoint() - QPointF(GRIP_SHIFT,0);
    }
    update();
}

void
ConditionalRelation::updateCoordinates(unsigned int boxId)
{
    /// \todo Maquette::getInstance()->getConditionDate(ID()); then update start/end;    
    if (!Maquette::getInstance()->isZooming() ) {
        updateBoxesCoordinates(boxId);
    }
    updateCoordinates();
}

void
ConditionalRelation::updateBoxesCoordinates(unsigned int boxId)
{
    // Move related boxes, except boxId's box (already moved by user)
    QList<BasicBox *>::iterator     it;
    BasicBox                        *box = _scene->getBox(boxId),
                                    *curBox;
    float                           curBoxX,
                                    boxX = box->getLeftGripPoint().x();

    for(it=_boxesAttached.begin() ; it!=_boxesAttached.end() ; it++)
    {
        curBox = *it;

        if(curBox->ID() != box->ID())
        {
            curBoxX = curBox->getLeftGripPoint().x();

            if(boxX != curBoxX)
            {
                curBox->moveBy(boxX - curBoxX, 0.);
                _scene->boxMoved(curBox->ID());
            }
        }
    }
}

void
ConditionalRelation::attachBoxes(QList<BasicBox *> conditionedBox)
{
    QList<BasicBox *>::iterator it;

    for(it=conditionedBox.begin() ; it!=conditionedBox.end() ; it++)
    {
        if(!_boxesAttached.contains(*it))
        {
            _boxesAttached<<(*it);
            Maquette::getInstance()->attachToCondition(_id,*it);

            //set conditional relation to each box
            (*it)->addConditionalRelation(this);

            //default : auto-trigger = false for all triggerPoints attached
            Maquette::getInstance()->setTriggerPointDefault((*it)->getTriggerPoint(BOX_START)->ID(),false);
        }
    }
    updateCoordinates();
}

void
ConditionalRelation::detachBox(BasicBox * box)
{
    Maquette::getInstance()->detachFromCondition(_id,box);
    updateCoordinates();
    _boxesAttached.removeAll(box);
}

void
ConditionalRelation::detachBoxes(QList<BasicBox *> conditionedBox)
{
    QList<BasicBox *>::iterator it;

    for(it=conditionedBox.begin() ; it!=conditionedBox.end() ; it++)
        detachBox(*it);
}

QPair<BasicBox *, BasicBox *>
ConditionalRelation::getLowestHighestBoxes()
{
    BasicBox                        *curBox,
                                    *lowestBox  = _boxesAttached.first(),
                                    *highestBox = _boxesAttached.first();

    qreal                           curY,
                                    lowestY     = lowestBox->getLeftGripPoint().y(),
                                    highestY    = lowestBox->getLeftGripPoint().y();

    QList<BasicBox *>::iterator     it = _boxesAttached.begin();

    for(++it ; it!=_boxesAttached.end() ; it++)
    {
        curBox = *it;
        curY = curBox->getLeftGripPoint().y();

        if(curY > lowestY){
            lowestBox = curBox;
            lowestY = curY;
        }
        else if(curY < highestY){
            highestBox = curBox;
            highestY = curY;
        }
    }

    return qMakePair(lowestBox,highestBox);
}

int ConditionalRelation::nbOfAttachedBoxes()
{
    return _boxesAttached.size();
}

void
ConditionalRelation::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
    QGraphicsObject::hoverEnterEvent(event);
    setCursor(Qt::PointingHandCursor);
}

void
ConditionalRelation::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
    QGraphicsObject::hoverMoveEvent(event);
    setCursor(Qt::PointingHandCursor);
}

void
ConditionalRelation::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
    QGraphicsObject::hoverLeaveEvent(event);
    setCursor(Qt::ArrowCursor);
}

QInputDialog *
ConditionalRelation::disposeMessageInputDialog()
{
  QInputDialog *msgDialog = new QInputDialog(_scene->views().first(), Qt::Popup);
  msgDialog->setInputMode(QInputDialog::TextInput);
  msgDialog->setLabelText(QObject::tr("Enter the dispose message :"));
  msgDialog->setTextValue(QString::fromStdString(Maquette::getInstance()->getConditionMessage(_id)));

  return msgDialog;
}
void
ConditionalRelation::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsObject::mouseDoubleClickEvent(event);
      if (!_scene->playing())
      {
          QInputDialog *disposeMsgEdit = disposeMessageInputDialog();
          bool ok = disposeMsgEdit->exec();
          disposeMsgEdit->move(event->pos().toPoint());
          if (ok)
              Maquette::getInstance()->setConditionMessage(_id,disposeMsgEdit->textValue().toStdString());

          disposeMsgEdit->deleteLater();
      }
}

void
ConditionalRelation::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsObject::mousePressEvent(event);
    if(cursor().shape() == Qt::PointingHandCursor)
        setSelected(true);

    if (cursor().shape() == Qt::OpenHandCursor)
    {
        setCursor(Qt::ClosedHandCursor);
        _mousePosSave = event->pos();
        _mousePressed = true;
    }
}

void
ConditionalRelation::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsObject::mouseMoveEvent(event);

//    if(cursor().shape() == Qt::ClosedHandCursor){
        /// \todo
//    }
}

void
ConditionalRelation::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsObject::mouseReleaseEvent(event);
    _mousePressed = false;
}

QRectF
ConditionalRelation::boundingRect() const
{
    QPointF     bottomRight = QPointF(_start.x() , _start.y()),
                topLeft = QPointF(_end.x() - BOUNDING_RECT_WIDTH/2 , _end.y());

    return QRectF(topLeft,bottomRight);
}



void
ConditionalRelation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen dashLinePen = QPen(Qt::DashLine);
    dashLinePen.setColor(isSelected() ? _selectedColor : _color);
    dashLinePen.setWidth(2);
    painter->setPen(dashLinePen);

    painter->drawLine(_start,_end);
}

int
ConditionalRelation::type() const
{
    return CONDITIONAL_RELATION_TYPE;
}
