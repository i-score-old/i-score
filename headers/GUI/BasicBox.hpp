/*
Copyright: LaBRI / SCRIME

Authors: Luc Vercellin and Bruno Valeze (08/03/2010)

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
#ifndef BASIC_BOX
#define BASIC_BOX

/*!
 * \file BasicBox.h
 *
 * \author Luc Vercellin, Bruno Valeze
 */

#include <QGraphicsItem>
#include <QGraphicsSvgItem>
#include <map>
#include <vector>
#include <string>
#include <QInputDialog>
#include "AbstractBox.hpp"
#include "CSPTypes.hpp"
#include "CurvesWidget.hpp"
#include "BoxWidget.hpp"

class MaquetteScene;
class QGraphicsTextItem;
class AbstractComment;
class AbstractTriggerPoint;
class Comment;
class TriggerPoint;
class TextEdit;
class Relation;
class AbstractCurve;

/*!
 * \brief Enum used to define Basic Box's item type.
 */
enum {BASIC_BOX_TYPE = QGraphicsItem::UserType + 1};
/*!
 * \brief Enum used to manage various box extremities.
 */
enum BoxExtremity {NO_EXTREMITY = -1, BOX_START = BEGIN_CONTROL_POINT_INDEX,
		   BOX_END = END_CONTROL_POINT_INDEX};

/*!
 * \class BasicBox
 *
 * \brief Base class for all boxes, derived from Qt's QGraphicsItem.
 */
class BasicBox : public QGraphicsItem
{

 public :

  BasicBox(const QPointF &press, const QPointF &release, MaquetteScene *parent);

  BasicBox(AbstractBox *abstract, MaquetteScene *parent);

  virtual ~BasicBox();
  /*!
   * \brief Redefinition of QGraphicsItem::type(). Used for Item casting.
   *
   * \return the item's type of the box
   */
  virtual int type() const;
  /*!
   * \brief Gets the abstract of the item.
   */
  virtual Abstract *abstract() const;
  /*!
   * \brief Initialises item's properties.
   */
  void init();
  /*!
   * \brief Gets the name of the box.
   *
   * \return the name of the box
   */
  QString name() const;
  /*!
   * \brief Sets the name of the box.
   *
   * \param name : the new name for the box
   */
  void setName(const QString & name);
  /*!
   * \brief Gets the color of the box.
   *
   * \return the color of the box
   */
  virtual QColor color() const;
  /*!
   * \brief Sets the color of the box.
   *
   * \param color : the new color of the box
   */
  virtual void setColor(const QColor &color);
  /*!
   * \brief Gets the ID of the box.
   *
   * \return the ID of the box
   */
  unsigned int ID() const;
  /*!
   * \brief Sets the ID of the box.
   *
   * \param ID : the new ID to be set
   */
  virtual void setID(unsigned int ID);
  /*!
   * \brief Gets the mother ID of the box.
   *
   * \return the mother ID of the box
   */
  unsigned int mother() const;
  /*!
   * \brief Sets the mother ID of the box.
   *
   * \param ID : the new mother ID to be set
   */
  void setMother(unsigned int motherID);
  /*!
   * \brief Determines if box has a mother.
   *
   * \return the possible mother of the box
   */
  bool hasMother();
  /*!
   * \brief Updates stuff related to the box graphical attributes.
   */
  void updateStuff();
  /*!
   * \brief Adds a relation to an extremity
   *
   * \param extremity : the extremity involved in the relation
   * \param relation : the relation to store
   */
  void addRelation(BoxExtremity extremity, Relation *relation);
  /*!
   * \brief Removes a relation from an extremity
   *
   * \param extremity : the extremity involved in the relation
   * \param relID: the relation ID
   */
  void removeRelation(BoxExtremity extremity, unsigned int relID);
  void removeRelations(BoxExtremity extremity);
  /*!
   * \brief Adds a comment "Comment" to the box.
   */
  void addComment();
  /*!
   * \brief Adds a specific comment to the box.
   *
   * \param comment : the commment description
   */
  void addComment(const AbstractComment &comment);
  /*!
   * \brief Removes comment from the box if one.
   */
  void removeComment();
  /*!
   * \brief Gets the comment from the box if one.
   *
   * \return the possible comment
   */
  Comment* comment() const;
  /*!
   * \brief Plays the box.
   */
  virtual void play() = 0;
  /*!
   * \brief Determines if the box is currently beeing played.
   *
   * \return the current playing state
   */
  bool playing() const;
  /*!
   * \brief Informs the box that a specific extremity is crossed while playing.
   *
   * \param extremity : the extremity crossed
   */
  void setCrossedExtremity(BoxExtremity extremity);
  /*!
   * \brief Informs the box that a specific extremity is triggered while playing.
   *
   * \param extremity : the extremity crossed
   */
  void setCrossedTriggerPoint(bool waiting, BoxExtremity extremity);
  /*!
   * \brief Determines if the box has a trigger point at a specific extremity.
   *
   * \param extremity : the extremity to know whether an extremity exists
   *
   * \return if the extremity is linked to a trigger point
   */
  bool hasTriggerPoint(BoxExtremity extremity);
  /*!
   * \brief Adds a trigger point to the box.
   *
   * \param extremity : the extremity of the box concerned by the trigger point
   * \return if trigger point could be created
   */
  bool addTriggerPoint(BoxExtremity extremity);
  /*!
   * \brief Adds an existing trigger point to the box.
   *
   * \param extremity : the extremity of the box concerned by the trigger point
   * \param tp : the trigger point to add
   */
  void addTriggerPoint(BoxExtremity extremity, TriggerPoint *tp);
  /*!
   * \brief Adds a trigger point to the box.
   *
   * \param abstract : the abstract trigger point to add
   */
  void addTriggerPoint(const AbstractTriggerPoint &abstract);
  /*!
   * \brief Removes a trigger point from the box.
   *
   * \param extremity : the extremity of the box concerned by the trigger point
   */
  void removeTriggerPoint(BoxExtremity extremity);
  /*!
   * \brief Sets the message waited by a control point.
   *
   * \param extremity : the box extremity to set message from
   * \param message : new message to wait for the trigger point
   */
  void setTriggerPointMessage(BoxExtremity extremity, const std::string &message);
  /*!
   * \brief Gets the message waited by a control point.
   *
   * \param extremity : the box extremity to get message from
   */
  std::string triggerPointMessage(BoxExtremity extremity);

  AbstractCurve *getCurve(const std::string &address);
  void setCurve(const std::string &address, AbstractCurve *curve);
  void removeCurve(const std::string &address);

  /*!
   * \brief Gets the top left of the box in the scene coordinates.
   *
   * \return the top left corner in scene coordinates
   */
  QPointF getTopLeft() const;
  /*!
   * \brief Gets the bottom right corner in scene coordinates.
   *
   * \return the bottom right corner in scene coordinates
   */
  QPointF getBottomRight() const;
  /*!
   * \brief Gets the center of the box.
   *
   * \return the center
   */
  virtual QPointF getCenter() const;
  /*!
   * \brief Gets the relative center of the box.
   *
   * \return the relative center
   */
  virtual QPointF getRelativeCenter() const;
  /*!
   * \brief Gets the shape's right side center.
   *
   * \return the shape's right side center
   */
  virtual QPointF getMiddleRight() const;
  /*!
   * \brief Gets the shape's left side center.
   *
   * \return the shape's left side center
   */
  virtual QPointF getMiddleLeft() const;
  /*!
    * \brief Gets the shape top left point.
    *
    * \return the shape top left point.
    */
  virtual QPointF getShapeTopLeft() const;
  /*!
    * \brief Gets the shape top right point.
    *
    * \return the shape top right point.
    */
  virtual QPointF getShapeTopRight() const;
  /*!
   * \brief Gets the size of the box.
   *
   * \return the size
   */
  QPointF getSize() const;
  /*!
   * \brief This method must be overloaded for QGraphicsItem derived classes.
   * The center of the bounding rectangle is (0,0)
   *
   * \return the bounding rectangle in items coordinates
   */
  virtual QRectF boundingRect() const;
  /*!
   * \brief This method is overloaded from QGraphicsItem.
   * The center of the shape is (0,0)
   *
   * \return the shape in items coordinates
   */
  virtual QPainterPath shape() const;
  /*!
   * \brief Sets the upper left corner of the box.
   * It doesn't change the graphical representation, that task is
   * performed when the new system has been computed by the constraint
   * solver.
   *
   * \param tl : the upper left corner
   */
  void setTopLeft(const QPointF & tl);
  /*!
   * \brief Sets the upper left corner of the box in coordinates relative
   * to it's parent box.
   * It doesn't change the graphical representation, that task is
   * performed when the new system has been computed by the constraint
   * solver.
   *
   * \param rtl : the upper left corner relative coordinates
   */
  void setRelativeTopLeft(const QPointF & rtl);
  /*!
   * \brief Sets the size of the box : the upper left corner doesn't move.
   * It doesn't change the graphical representation, that task is
   * performed when the new system has been computed by the constraint
   * solver.
   *
   * \param size : the new size of the box
   */
  void setSize(const QPointF & size);
  /*!
   * \brief Gets the top left x position.
   *
   * \return the x value of the top left corner
   */
  float beginPos() const;
  /*!
   * \brief Gets the begin position relative to the parent box.
   *
   * \return the x value of the top left corner
   */
  float relativeBeginPos() const;
  /*!
   * \brief Gets the horizontal size of the box.
   *
   * \return the horizontal size of the box
   */
  float width() const;
  /*!
   * \brief Gets the vertical size of the box.
   *
   * \return the vertical size of the box
   */
  float height() const;
  /*!
   * \brief Gets the time position of the box in ms, the left size of the scene is
   * date 0.
   *
   * \return the date of the box
   */
  unsigned int date() const;
  /*!
   * \brief Gets the temporal duration of the box, in ms.
   *
   * \return the duration of the box
   */
  unsigned int duration() const;
  /*!
   * \brief Disable box edition.
   */
  void lock();
  /*!
   * \brief Enable box edition.
   */
  void unlock();
  /*!
   * \brief Sets the new horizontal size after a mouse horizontal resizing.
   * It doesn't change the graphical representation, that task is performed
   * when the new system has been computed by the constraint solver.
   *
   * \param width : the new width
   */
  virtual void resizeWidthEdition(float width);
  /*!
   * \brief Sets the new vertical size after a mouse vertical resizing.
   * It doesn't change the graphical representation, that task is performed
   * when the new system has been computed by the constraint solver.
   *
   * \param height : the new height
   */
  virtual void resizeHeightEdition(float height);
  /*!
   * \brief Sets the new size after a mouse resizing.
   * It doesn't change the graphical representation, that task is performed
   * when the new system has been computed by the constraint solver.
   *
   * \param length : the new length
   * \param height : the new height
   */
  virtual void resizeAllEdition(float height, float length);
  /*!
   * \brief Determines if the box is currently beeing resized.
   *
   * \return true if the box is currently beeing resized
   */
  bool resizing() const;
  /*!
   * \brief Determines if the box is commented.
   *
   * \return if the box is commented
   */
  bool hasComment() const;

  /*!
   * \brief Gets message to send when the start of the box is reached.
   *
   * \return the message to send at box's start
   */
  std::vector<std::string> firstMessagesToSend() const;
  /*!
   * \brief Gets message to send when the end of the box is reached.
   *
   * \return the message to send at box's end
   */
  std::vector<std::string> lastMessagesToSend() const;
  /*!
   * \brief Sets messages to send when the start of the box is reached.
   *
   * \param messages : the messages to send at box's start
   */
  void setFirstMessagesToSend(const std::vector<std::string> &messages);
  /*!
   * \brief Sets networkMessages to send when the start of the box is reached.
   *
   * \param messages : the messages to send at box's start
   */
  void setStartMessages(NetworkMessages *messages);
//  void setStartMessagesToSend(QMap<QTreeWidgetItem *, Message> messages);
  /*!
   * \brief Sets networkTreeItems to send when the start of the box is reached.
   *
   * \param itemsSelected : the items to send at box's start
   */
//  void setSelectedItemsToSend(QList<QTreeWidgetItem*> itemsSelected);
  void setSelectedItemsToSend(QMap<QTreeWidgetItem*,Data> itemsSelected);
  /*!
   * \brief Sets networkTreeExpandedItems to send.
   *
   * \param itemsExpanded : the expanded items to send
   */
  void setExpandedItemsList(QList<QTreeWidgetItem*> itemsExpanded);
  /*!
   * \brief Send the item to be add to the networkTreeExpandedItems.
   *
   * \param itemsExpanded : the expanded item to send
   */
  void addToExpandedItemsList(QTreeWidgetItem* item);
  /*!
   * \brief Send the items to be removed from the networkTreeExpandedItems.
   *
   * \param itemsExpanded : the expanded items to send
   */
  void removeFromExpandedItemsList(QTreeWidgetItem* item);
  /*!
   * \brief Clear the expandedItems list.
   */
  void clearExpandedItemsList();
  /*!
   * \brief Sets messages to send when the end of the box is reached.
   *
   * \param messages : the messages to send at box's end
   */
  void setLastMessagesToSend(const std::vector<std::string> &messages);
  /*!
   * \brief Sets networkMessages to send when the end of the box is reached.
   *
   * \param messages : the messages to send at box's end
   */
  void setEndMessages(NetworkMessages *messages);
//    void setEndMessagesToSend(QMap<QTreeWidgetItem *, Message> messages);
  //! \brief Handles line width.
  static const unsigned int LINE_WIDTH = 2;
  //! \brief Handles resizing tolerance.
  static const unsigned int RESIZE_TOLERANCE = 20;

  /*!
   * \brief Painting method, redefinition of QGraphicsItem::paint().
   *
   * \param painter : the painter used to draw
   * \param options : array of options
   * \param widget : the widget that is being painting on
   */
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	     QWidget *widget = 0);
  /*!
   * \brief Binary operator to compare by starting positions.
   *
   * \param box : the box to compare with
   * \return true if this begins before box
   */
  bool operator<(BasicBox *box) const;
  void updateCurves();
  void centerWidget();
  void createWidget();



 protected:
  /*!
   * \brief Create the QInputDialog, for asking new name.
   */
  QInputDialog *nameInputDialog();
  /*!
   * \brief Redefinition of QGraphicsItem::mousePressEvent().
   * Occurs when a mouse button is pressed.
   *
   * \param event : the variable containing information about the event
   */
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

  /*!
   * \brief Redefinition of QGraphicsItem::contextMenuEvent().
   * Occurs when a right mouse button is pressed (or CTRL + click on MacOS X).
   *
   * \param event : the variable containing information about the event
   */
  virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
  /*!
   * \brief Redefinition of QGraphicsItem::mouseMoveEvent().
   * Occurs when the mouse moves over the box.
   *
   * \param event : the variable containing information about the event
   */
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  /*!
   * \brief Redefinition of QGraphicsItem::mouseDoubleClickEvent().
   * Occurs when the mouse moves over the box.
   *
   * \param event : the variable containing information about the event
   */
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
  /*!
   * \brief Redefinition of QGraphicsItem::mouseReleaseEvent().
   * Occurs when a mouse button is released.
   *
   * \param event : the variable containing information about the event
   */
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
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
   * \brief Redefinition of QGraphicsItem::itemChange().
   * Occurs when a modification is done to the box.
   */
  virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);



  //! Managing main information of the box.
  AbstractBox *_abstract;

  MaquetteScene * _scene; //!< The scene containing box.
  bool _hasContextMenu; //!< True if box has contextual menu.
  QMenu* _contextMenu; //!< The contextual menu, if one.
  bool _shift; //!< State of Shift Key.
  bool _playing; //!< State of playing.
  TextEdit *_trgPntMsgEdit; //!< The trigger point editing dialog.
  Comment *_comment; //!< The box comment.
  std::map<BoxExtremity,TriggerPoint*> _triggerPoints; //!< The trigger points.
  std::map < BoxExtremity,std::map < unsigned int, Relation* > > _relations; //!< The relations.
  std::map<std::string,AbstractCurve*> _abstractCurves; //!< The Curves
//  BoxWidget *_curvesWidget;
  BoxWidget *_curvesWidget;
//  CurvesWidget *_curvesWidget;
  QWidget *_boxWidget;

//signals :
// void doubleClickInBox();
};

#endif
