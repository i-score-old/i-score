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
    void attachBoxes(QList<BasicBox *> conditionedBox);

    /*!
     * \brief detach a box or a group of boxes of the conditional relation.
     *
     * \param conditionedBoxes : the boxes to detach.
     */
    void detachBoxes(QList<BasicBox *> conditionedBox);

protected:
  /*!
   * \brief This method must be overloaded for QGraphicsItem derived classes.
   * The center of the bounding rectangle is (0,0)
   *
   * \return the bounding rectangle in items coordinates
   */
  virtual QRectF boundingRect() const;

  /*!
   * \brief Redefinition of QGraphicsItem::hoverEnterEvent().
   * Occurs when the mouse pointer enters in the box's bounding box.
   *
   * \param event : the variable containing information about the event
   */
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);

  /*!
   * \brief Redefinition of QGraphicsItem::hoverMoveEvent().
   * Occurs when the mouse pointer moves in the box's bounding box.
   *
   * \param event : the variable containing information about the event
   */
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);

  /*!
   * \brief Redefinition of QGraphicsItem::hoverLeaveEvent().
   * Occurs when the mouse pointer leaves the box's bounding box.
   *
   * \param event : the variable containing information about the event
   */
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

  /*!
   * \brief This method is overloaded from QGraphicsItem.
   * The center of the shape is (0,0)
   *
   * \return the shape in items coordinates
   */
  virtual QPainterPath shape() const;

  /*!
   * \brief Redefinition of QGraphicsItem::mousePressEvent().
   * Occurs when a mouse button is pressed.
   *
   * \param event : the variable containing information about the event
   */
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

  /*!
   * \brief Redefinition of QGraphicsItem::mouseMoveEvent().
   * Occurs when the mouse moves over the box.
   *
   * \param event : the variable containing information about the event
   */
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

  /*!
   * \brief Redefinition of QGraphicsItem::mouseReleaseEvent().
   * Occurs when a mouse button is released.
   *
   * \param event : the variable containing information about the event
   */
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

  /*!
   * \brief Redefinition of QGraphicsItem mouseDoubleClickEvent method.
   * Raised when a double click occurs.
   *
   * \param event : the information about the event
   */
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);

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
