#ifndef CONDITIONALRELATION_HPP
#define CONDITIONALRELATION_HPP

/*!
 * \file ConditionalRelation.hpp
 *
 * \author Nicolas Hincker
 */

#include <QGraphicsItem>
#include <QPainter>
#include "Maquette.hpp"
#include "MaquetteScene.hpp"
#include "QGraphicsSceneMouseEvent"

class MaquetteScene;
class BasicBox;

/*!
 * \brief Enum used to define Relation's item type.
 */
enum { CONDITIONAL_RELATION_TYPE = QGraphicsItem::UserType + 7 };

class ConditionalRelation : public QGraphicsItem
{

public :
    ConditionalRelation(QList<BasicBox *> boxesAttached, MaquetteScene *parent);
    ConditionalRelation(unsigned int id, QList<BasicBox *> boxesAttached, MaquetteScene *parent);

    /*!
     * \brief Update relation extremities' coordinates.
     */
    void updateCoordinates();

    /*!
     * \brief Update relation extremities' coordinates and move boxes related except the boxId's box.
     */
    void updateCoordinates(unsigned int boxId);

    /*!
     * \brief Forces attached boxes to move to the boxId coordinates.
     */
    void updateBoxesCoordinates(unsigned int boxId);

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
    void detachBox(BasicBox * box);

    /*!
     * \brief Sorts all attached boxes to get the lowest and highest.
     *
     * \return A pair : first the lowest, second the highest.
     */
    QPair<BasicBox *, BasicBox *> getLowestHighestBoxes();

    /*!
     * \brief Gets attached boxes.
     *
     * \return The list of attached boxes.
     */
    inline QList<BasicBox *> getBoxes(){return _boxesAttached;}

    QInputDialog *disposeMessageInputDialog();

    inline unsigned int ID(){return _id;}

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

    /*!
     * \brief Redefinition of QGraphicsItem::type(). Used for Item casting.
     *
     * \return the item's type of the relation
     */
    virtual int type() const;


    static const QColor CONDITIONAL_RELATION_COLOR;
    static const QColor CONDITIONAL_RELATION_SELECTED_COLOR;
    static const float  BOUNDING_RECT_WIDTH;
    static const float  GRIP_SHIFT;

private :

    void init();

    unsigned int        _id;                //!< Time condition id.
    QList<BasicBox *>   _boxesAttached;     //!< Conditioned boxes attached to the relation.
    MaquetteScene *     _scene;             //!< The scene containing relation.
    QPointF             _start;             //!< The starting point of the relation.
    QPointF             _mousePosSave;
    QPointF             _end;               //!< The ending point of the relation.
    QColor              _color;             //!< Color of the relation.
    QColor              _selectedColor;     //!< Color of the relation when selected.
    bool                _mousePressed;      //!< State of mouse click.
};

#endif // CONDITIONALRELATION_HPP
