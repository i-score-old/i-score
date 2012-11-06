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
#ifndef RELATION_H
#define RELATION_H

/*!
 * \file Relation.h
 *
 * \author Luc Vercellin
 */

#include <QGraphicsItem>
#include <QPainterPath>
#include "BasicBox.hpp"

class MaquetteScene;
class BasicBox;
class QString;
class AbstractRelation;
class Abstract;

/*!
 * \brief Enum used to define Relation's item type.
 */
enum {RELATION_TYPE = QGraphicsItem::UserType + 4};

#define NO_LENGTH -1

/*!
 * \class Relation
 *
 * \brief Relation item, derived from Qt's QGraphicsItem.
 */
class Relation : public QGraphicsItem
{
 public :

  Relation(unsigned int firstBoxID, BoxExtremity firstBoxExt, unsigned int secondBoxID,
	   BoxExtremity secondBoxExt, MaquetteScene *parent);

  Relation(const AbstractRelation &abstract, MaquetteScene *parent);

  virtual ~Relation();

  /*!
   * \brief Initialises item's properties.
   */
  void init();
  /*!
   * \brief Gets thre relation's ID.
   *
   * \return the ID of the relation
   */
  unsigned int ID() const;
  /*!
   * \brief Gets the abstract of the item.
   */
  virtual Abstract* abstract() const;
  /*!
   * \brief Sets the ID of the relation.
   *
   * \param ID : the new ID to be set
   */
  void setID(unsigned int ID);
  /*!
   * \brief Computes a string describing the relation.
   * \return a string describing the relation
   */
  QString toString();
  /*!
   * \brief Redefinition of QGraphicsItem::type(). Used for Item casting.
   *
   * \return the item's type of the relation
   */
  virtual int type() const;
  /*!
   * \brief Gets the center of the relation.
   *
   * \return the bottom right corner
   */
  QPointF getCenter() const;
  /*!
   * \brief Update relation extremities' coordinates.
   */
  void updateCoordinates();
  /*!
   * \brief Change relation boundaries.
   *
   * \param minBound : the new min bound to be set
   * \param maxBound : the new max bound to be set
   */
  void changeBounds(const float &minBound, const float &maxBound);
  inline QRectF startBoundRect(){return _startBoundRect;}
  inline QRectF endBoundRect(){return _endBoundRect;}
  void updateFlexibility();

  static const float TOLERANCE_X;
  static const float TOLERANCE_Y;
  static const float GRIP_CIRCLE_SIZE;

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
  virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
  /*!
   * \brief Redefinition of QGraphicsItem::hoverMoveEvent().
   * Occurs when the mouse pointer moves in the box's bounding box.
   *
   * \param event : the variable containing information about the event
   */
  virtual void hoverMoveEvent ( QGraphicsSceneHoverEvent * event );
  /*!
   * \brief Redefinition of QGraphicsItem::hoverLeaveEvent().
   * Occurs when the mouse pointer leaves the box's bounding box.
   *
   * \param event : the variable containing information about the event
   */
  virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
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
  virtual void mouseDoubleClickEvent (QGraphicsSceneMouseEvent * event);
  /*!
   * \brief Painting method, redefinition of QGraphicsItem::paint().
   *
   * \param painter : the painter  used to draw
   * \param options : array of options
   * \param widget : the widget that is being painting on
   */
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		     QWidget *widget = 0);



 private :

  MaquetteScene * _scene; //!< The scene containing relation.

  QMenu* _contextMenu; //!< The contextual menu.

  AbstractRelation *_abstract; //!< The abstract relation containing main information.

  QPointF _start; //!< The starting point of the relation.
  QPointF _end; //!< The ending point of the relation.

  QRectF _startBoundRect;
  QRectF _endBoundRect;

  bool _leftHandleSelected; //!< Left handle selection state.
  bool _rightHandleSelected; //!< Right handle selection state.

  bool _flexibleRelation;//!< The relation can be extended

  static const float ARROW_SIZE; //!< The size of the arrow.
  static const float HANDLE_HEIGHT; //!< The height of a handle.
  static const float HANDLE_WIDTH; //!< The width of a handle.


};

#endif
