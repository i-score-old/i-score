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
#ifndef TRIGGER_POINT_H
#define TRIGGER_POINT_H

/*!
 * \file TriggerPoint.h
 *
 * \author Luc Vercellin
 */

#include <QGraphicsItem>
#include "BasicBox.hpp"
#include "AbstractTriggerPoint.hpp"
#include <string>
#include <QInputDialog>
#include "TriggerPointEdit.hpp"

class MaquetteScene;
class Abstract;

/*!
 * \brief Enum used to define TriggerPoint's item type.
 */
enum { TRIGGER_POINT_TYPE = QGraphicsItem::UserType + 6 };

/*!
 * \class TriggerPoint
 *
 * \brief TriggerPoint item, derived from Qt's QGraphicsItem.
 */
class TriggerPoint : public QGraphicsItem
{
  public:
    TriggerPoint(unsigned int boxID, BoxExtremity extremity,
                 const std::string &message, unsigned int ID, MaquetteScene *parent);

    TriggerPoint(const AbstractTriggerPoint &abstract, MaquetteScene *parent);

    bool isConditioned();

    virtual
    ~TriggerPoint();

    /*!
     * \brief Initialises item's properties.
     */
    void init();

    /*!
     * \brief Gets the trigger point's date.
     *
     * \return the date
     */
    inline double
    date() const { return this->pos().x() * (MaquetteScene::MS_PER_PIXEL); }

    /*!
     * \brief Gets the trigger point's ID.
     *
     * \return the ID
     */
    inline unsigned int
    ID() const
    { return _abstract->ID(); }

    /*!
     * \brief Sets the trigger point's ID.
     *
     * \param : the new ID
     */
    inline void
    setID(unsigned int ID)
    { _abstract->setID(ID); }

    /*!
     * \brief Gets the trigger point box's extremity.
     *
     * \return the box's extremity
     */
    BoxExtremity
    boxExtremity() const
    { return _abstract->boxExtremity(); }

    /*!
     * \brief Gets the trigger point box's ID.
     *
     * \return the box's ID
     */
    unsigned int
    boxID() const
    { return _abstract->boxID(); }

    /*!
     * \brief Gets the abstract of the item.
     */
    virtual Abstract* abstract() const;

    /*!
     * /brief Gets message waited by the trigger point.
     *
     * \return message waited by the trigger point
     */
    inline std::string
    message() const
    { return _abstract->message(); }

    /*!
     * /brief Sets the message waited by the trigger point.
     *
     * \param message : the new message waited by the trigger point
     */
    inline void
    setMessage(const std::string &message)
    {
      _abstract->setMessage(message);
      update();
    }

    /*!
     * /brief Sets the waiting state of the trigger point.
     *
     * \param waiting : the new waiting state of the trigger point
     */
    inline void
    setWaiting(bool waiting)
    {
      _abstract->setWaiting(waiting);
      update();
    }

    /*!
     * /brief Gets the waiting state of the trigger point.
     *
     * \param waiting : the new waiting state of the trigger point
     */
    inline bool
    isWaiting()
    {
      return _abstract->isWaiting();
    }

    /*!
     * \brief Sets the ID of the Trigger Point.
     *
     * \param ID : the new ID to be set
     */
    void
    setBoxID(unsigned int boxID)
    { _abstract->setBoxID(boxID); }

    /*!
     * \brief Updates Trigger Point position.
     */
    void updatePosition();

    /*!
     * \brief Redefinition of QGraphicsItem::type(). Used for Item casting.
     *
     * \return the item's type of the Trigger Point
     */
    virtual int
    type() const
    { return TRIGGER_POINT_TYPE; }

    void drawFlag(QPainter *painter, QColor color);

  protected:
    virtual void keyPressEvent(QKeyEvent *event);

    /*!
     * \brief Redefinition of QGraphicsItem::mousePressEvent().
     * Occurs when a mouse button is pressed.
     *
     * \param event : the variable containing information about the event
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

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
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    /*!
     * \brief This method must be overloaded for QGraphicsItem derived classes.
     * The center of the bounding rectangle is (0,0)
     *
     * \return the bounding rectangle in items coordinates
     */
    virtual QRectF boundingRect() const;

    /*!
     * \brief This method is a redefinition of for QGraphicsItem::shape().
     *
     * \return the shape used for mouse interaction and collision
     */
    virtual QPainterPath shape() const;

    /*!
     * \brief Create the QInputDialog, for asking new name.
     */
    QInputDialog *nameInputDialog();
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

  private:
    static const float WIDTH;        //!< Trigger point width.
    static const float HEIGHT;       //!< Trigger point height.

    MaquetteScene * _scene;          //!< The scene containing trigger point.

    AbstractTriggerPoint *_abstract; //!< The abstract trigger point containing main information.

    TriggerPointEdit *_edit;
};
#endif
