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
#ifndef BASIC_BOX
#define BASIC_BOX

/*!
 * \file BasicBox.hpp
 *
 * \author Pascal Baltazar, Nicolas Hincker, Luc Vercellin and Myriam Desainte-Catherine 
 */

#include <QGraphicsItem>
#include <QGraphicsSvgItem>
#include <map>
#include <QMap>
#include <vector>
#include <string>
#include <QInputDialog>
#include "AbstractBox.hpp"
#include "BoxWidget.hpp"
#include <QComboBox>
#include <QGraphicsProxyWidget>
#include <QObject>
#include <QPushButton>
#include <QMenu>
#include <QGraphicsColorizeEffect>

#include "TTScore.h"

class MaquetteScene;
class QGraphicsTextItem;
class AbstractComment;
class AbstractTriggerPoint;
class Comment;
class TriggerPoint;
class Relation;
class ConditionalRelation;
class AbstractCurve;
class QObject;

/*!
 * \brief Enum used to manage various box extremities.
 */
enum BoxExtremity { NO_EXTREMITY = -1, BOX_START = BEGIN_CONTROL_POINT_INDEX,
                    BOX_END = END_CONTROL_POINT_INDEX };

/*!
 * \class BasicBox
 *
 * \brief Base class for all boxes, derived from Qt's QGraphicsItem.
 */
class BasicBox : public QObject, public QGraphicsItem
{
  public:
    BasicBox(const QPointF &press, const QPointF &release, MaquetteScene *parent);

    BasicBox(AbstractBox *abstract, MaquetteScene *parent);

    virtual void updateDisplay(QString displayMode);
    virtual
    ~BasicBox();

    /*!
     * \brief Enum used to define Basic Box's item type.
     */
    enum { BASIC_BOX_TYPE = QGraphicsItem::UserType + 1 };
    
    static const QColor BOX_COLOR;
    static const QColor TEXT_COLOR;

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

    void updateRelations(BoxExtremity extremity);

    /*!
     * \brief Adds a relation to an extremity
     *
     * \param extremity : the extremity involved in the relation
     * \param relation : the relation to store
     */
    void addRelation(BoxExtremity extremity, Relation *relation);

    /*!
     * \brief Adds a conditional relation to the start extremity
     *
     * \param condRel : the conditional relation to store
     */
    void addConditionalRelation(ConditionalRelation *condRel);

    /*!
     * \brief Removes a relation from an extremity
     *
     * \param extremity : the extremity involved in the relation
     * \param relID: the relation ID
     */
    void removeRelation(BoxExtremity extremity, unsigned int relID);
    void removeRelations(BoxExtremity extremity);

    /*!
     * \brief Removes a conditional relation from the start extremity
     *
     * \param condRelID : the conditional relation ID to remove
     */
    void removeConditionalRelation(ConditionalRelation *condRel);
    void removeConditionalRelations();

    void detachFromCondition();

    /*!
     * \brief Returns Relations associated to the start box extremity.
     */
    QList<Relation *> getStartBoxRelations();
    /*!
     * \brief Returns Relations associated to the end box extremity.
     */
    QList<Relation *> getEndBoxRelations();
    /*!
     * \brief Gets relations from an extremity
     *
     * \param extremity : the extremity involved in the relation
     */
    QList<Relation *> getRelations(BoxExtremity extremity);

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

    /*!
     * \brief Gets trigger point on the extremity.
     *
     * \param extremity : the box extremity to get trigger point from.
     */
    TriggerPoint *getTriggerPoint(BoxExtremity extremity);

    AbstractCurve *getCurve(const std::string &address);
    void setCurve(const std::string &address, AbstractCurve *curve);
    void removeCurve(const std::string &address);
    void addCurve(const std::string &address);
    void addCurveAddress(const std::string &address);
    void curveShowChanged(const QString &address, bool state);
    QRectF boxRect();
    QRectF boxBody();
    void curveActivationChanged(string address, bool activated);
    void setMessage(QTreeWidgetItem *item, QString address);
    void createActions();
    void createMenus();

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

    NetworkMessages *startMessages();
    void setStartMessage(QTreeWidgetItem *item, QString address);

    /*!
     * \brief Sets networkTreeItems to send when the start of the box is reached.
     *
     * \param itemsSelected : the items to send at box's start
     */
    void setSelectedItemsToSend(QMap<QTreeWidgetItem*, Data> itemsSelected);

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

    void setSelectedTreeItems(QList<QTreeWidgetItem *> selectedItems);

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

    /*!
     * \brief Return, as a QString list, the start messages.
     *
     * \return Start cue.
     */
    std::vector<std::string> getStartMessages();

    NetworkMessages *endMessages();
    void setEndMessage(QTreeWidgetItem *item, QString address);
    void updateWidgets();
    void updateCurveRangeBoundMin(string address, float value);
    void updateCurveRangeBoundMax(string address, float value);

    //! \brief Handles line width.
    static const unsigned int LINE_WIDTH = 2;

    //! \brief Handles resizing tolerance.
    static const unsigned int RESIZE_TOLERANCE = 25;

    static unsigned int BOX_MARGIN;
    static const float TRIGGER_ZONE_WIDTH;
    static const float TRIGGER_ZONE_HEIGHT;
    static const float TRIGGER_EXPANSION_FACTOR;
    static const float RAIL_HEIGHT;
    static const float EAR_WIDTH;
    static const float EAR_HEIGHT;
    static const int COMBOBOX_WIDTH;
    static const int COMBOBOX_HEIGHT;
    static const float MSGS_INDICATOR_WIDTH;
    static const float GRIP_CIRCLE_SIZE;
    static const QString SCENARIO_MODE_TEXT;
    static const QString DEFAULT_MODE_TEXT;

    /*!
     * \brief Painting method, redefinition of QGraphicsItem::paint().
     *
     * \param painter : the painter used to draw
     * \param options : array of options
     * \param widget : the widget that is being painting on
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    /*!
     * \brief Binary operator to compare by starting positions.
     *
     * \param box : the box to compare with
     * \return true if this begins before box
     */
    bool operator<(BasicBox *box) const;

    void updateCurves();
    void updateCurve(string address, bool forceUpdate);
    void centerWidget();
    void createWidget();
    void drawInteractionGrips(QPainter *painter);
    void drawTriggerGrips(QPainter *painter);
    void drawHoverShape(QPainter *painter);
    void drawSelectShape(QPainter *painter);

    void updateBoxSize();
    inline QRectF
    leftEar(){ return _leftEar; }
    inline QRectF
    rightEar(){ return _rightEar; }
    inline BoxWidget *
    boxContentWidget(){ return _boxContentWidget; }
    inline QWidget *
    boxWidget(){ return _boxWidget; }
    inline MaquetteScene *
    maquetteScene(){ return _scene; }
    inline void
    setComboBox(QComboBox *cbox){ boxContentWidget()->setComboBox(cbox); }
    inline void
    setStackedLayout(QStackedLayout *slayout){ boxContentWidget()->setStackedLayout(slayout); }
    inline bool
    hasCurve(string address){ return _curvesAddresses.contains(address); }
    void addMessageToRecord(std::string address);
    void removeMessageToRecord(std::string address);

    QPointF getLeftGripPoint();
    QPointF getRightGripPoint();
    void displayCurveEditWindow();
    inline bool
    hasStartMsgs(){ return _abstract->hasFirstMsgs(); }
    inline bool
    hasEndMsgs(){ return _abstract->hasLastMsgs(); }
    void drawMsgsIndicators(QPainter *painter);
    void updateFlexibility();
    void addToComboBox(QString address);
    QString currentText();
    inline qreal
    currentZvalue(){ return _currentZvalue; }
    void lower(bool state);
    void changeColor(QColor color);
    inline QColor
    currentColor(){ return _color; }
    void select();
    void setRecMode(bool activated);
    void setMuteState(bool activated);
    inline bool recording(){return _recording;}
    inline bool getMuteState(){return _mute;}
    inline bool isConditioned(){return !_conditionalRelation.isEmpty();}
    inline QList<ConditionalRelation *> getConditionalRelations(){return _conditionalRelation;}

    void updateRecordingCurves();
    void setButtonsVisible(bool value);
    void updatePlayingModeButtons();

    /*!
     * \brief Return the messages list, like if the box just ended its execution.
     * \return QMap <message address, QPair<value, date> >
     */
    QMap<QString, QPair<QString, unsigned int> > getFinalState();
    QMap<QString, QPair<QString, unsigned int> > getStartState();

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
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

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
     * \brief Redefinition of QGraphicsItem::itemChange().
     * Occurs when a modification is done to the box.
     */
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    /*!
     * \brief Display a toolTip near the bottomRight point, showing the current duration of the box.
     */
    void displayBoxDuration();

    //! Managing main information of the box.
    AbstractBox *_abstract;

    MaquetteScene * _scene;                                                     //!< The scene containing box.
    bool _shift;                                                                //!< State of Shift Key.
    bool _playing;                                                              //!< State of playing.
    bool _recording;                                                            //!< State of recording.
    bool _mute;                                                                 //!< State of mute.    
    Comment *_comment;                                                          //!< The box comment.
    QMap<BoxExtremity, TriggerPoint*> *_triggerPoints;                          //!< The trigger points.
    std::map < BoxExtremity, std::map < unsigned int, Relation* > > _relations; //!< The relations.
    QList<ConditionalRelation *> _conditionalRelation;                          //!< The conditional relations attached.
    std::map<std::string, AbstractCurve*> _abstractCurves;                      //!< The Curves.
    BoxWidget *_boxContentWidget;

    QRectF _boxRect;
    QRectF _leftEar;
    QRectF _rightEar;
    QRectF _startTriggerGrip;
    QRectF _endTriggerGrip;
    QRectF _startMsgsIndicator;
    QRectF _endMsgsIndicator;

    QWidget *_boxWidget;
    QComboBox *_comboBox;
    QGraphicsProxyWidget *_curveProxy;
    QGraphicsProxyWidget *_comboBoxProxy;
    QList<string> _curvesAddresses;
    bool _flexible;
    qreal _currentZvalue;
    QColor _color;
    QColor _colorUnselected;
    QGraphicsColorizeEffect *_recEffect;

    bool _low;
    bool _hover;

    QAction *_jumpToStartCue;
    QAction *_jumpToEndCue;
    QAction *_updateStartCue;
    QAction *_updateEndCue;

    QMenu *_startMenu;
    QMenu *_endMenu;

    QPushButton *_startMenuButton;
    QPushButton *_endMenuButton;
    QPushButton *_playButton;
    QPushButton *_stopButton;

};
#endif
