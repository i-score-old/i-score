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

const QColor ConditionalRelation::CONDITIONAL_RELATION_COLOR = QColor(0, 0, 0);
const QColor ConditionalRelation::CONDITIONAL_RELATION_SELECTED_COLOR = QColor(Qt::blue);

ConditionalRelation::ConditionalRelation(QList<BasicBox *> boxesAttached, MaquetteScene *parent)
    : QGraphicsItem(), _scene(parent), _boxesAttached(boxesAttached)
{
//    _id = Maquette::getInstance()->createCondition(boxesAttached);
    _color = CONDITIONAL_RELATION_COLOR;
    _selectedColor = CONDITIONAL_RELATION_SELECTED_COLOR;
    updateCoordinates();
}

void
ConditionalRelation::updateCoordinates()
{
    QPair<BasicBox *, BasicBox *> lowestHighestBoxes = getLowestHighestBoxes();

    _start = lowestHighestBoxes.first->getLeftGripPoint();
    _end = lowestHighestBoxes.second->getLeftGripPoint();

    update();
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
            //Maquette::getInstance()->attachToCondition(_id,(*it)->ID());
        }
    }
}

void
ConditionalRelation::detachBoxes(QList<BasicBox *> conditionedBox)
{
    QList<BasicBox *>::iterator it;

    for(it=conditionedBox.begin() ; it!=conditionedBox.end() ; it++)
    {
        _boxesAttached.removeAll(*it);
        //Maquette::getInstance()->detachFromCondition(_id,(*it)->ID());
    }
}

QPair<BasicBox *, BasicBox *>
ConditionalRelation::getLowestHighestBoxes()
{
    BasicBox                        *curBox,
                                    *lowestBox = _boxesAttached.first(),
                                    *highestBox = _boxesAttached.first();

    qreal                           curY,
                                    lowestY = lowestBox->getLeftGripPoint().y(),
                                    highestY = lowestBox->getLeftGripPoint().y();

    QList<BasicBox *>::iterator     it = _boxesAttached.begin()++;

    for(it ; it!=_boxesAttached.end() ; it++)
    {
        curBox = *it;
        curY = curBox->getLeftGripPoint().y();

        if(curY < lowestY)
            lowestBox = curBox;
        else if(curY > highestY)
            highestBox = curBox;
    }

    return qMakePair(lowestBox,highestBox);
}

void
ConditionalRelation::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
}

void
ConditionalRelation::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
}

void
ConditionalRelation::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
}

void
ConditionalRelation::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
}

void
ConditionalRelation::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
}

void
ConditionalRelation::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
}

void
ConditionalRelation::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
}

QRectF
ConditionalRelation::boundingRect() const
{
}

QPainterPath
ConditionalRelation::shape() const
{

}

void
ConditionalRelation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen dashLinePen = QPen(Qt::DashLine);
    dashLinePen.setColor(isSelected() ? _selectedColor : _color);
    painter->setPen(dashLinePen);

    painter->drawLine(_start , _end);
}

