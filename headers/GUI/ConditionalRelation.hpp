#ifndef CONDITIONALRELATION_HPP
#define CONDITIONALRELATION_HPP

/*!
 * \file ConditionalRelation.hpp
 *
 * \author Nicolas Hincker
 */

#include <QGraphicsItem>

class MaquetteScene;
class BasicBox;

class ConditionalRelation : public QGraphicsItem
{

public :
    ConditionalRelation(QList<BasicBox *> boxesAttached, MaquetteScene *parent);

    /*!
     * \brief Update relation extremities' coordinates.
     */
    void updateCoordinates();

    /*!
     * \brief Attach a box or a group of boxes to the conditional relation.
     *
     * \param conditionedBoxes : the boxes to attach.
     */
    void attachBox(QList<BasicBox *> conditionedBox);

    /*!
     * \brief detach a box or a group of boxes of the conditional relation.
     *
     * \param conditionedBoxes : the boxes to detach.
     */
    void detachBox(QList<BasicBox *> conditionedBox);
    /*!
     * \brief Painting method, redefinition of QGraphicsItem::paint().
     *
     * \param painter : the painter  used to draw
     * \param options : array of options
     * \param widget : the widget that is being painting on
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = 0);

    static const QColor CONDITIONAL_RELATION_COLOR;

private :

    QList<BasicBox *> _boxesAttached;   //!< Conditioned boxes attached to the relation.
    MaquetteScene * _scene;             //!< The scene containing relation.
    QPointF _start;                     //!< The starting point of the relation.
    QPointF _end;                       //!< The ending point of the relation.
    QColor _color;                      //!< Color of the relation.
};

#endif // CONDITIONALRELATION_HPP
