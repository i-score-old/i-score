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
#ifndef MAQUETTE_SCENE_HPP
#define MAQUETTE_SCENE_HPP

/*!
 * \file MaquetteScene.hpp
 *
 * \author Luc Vercellin, Bruno Valeze
 */

#include <QGraphicsScene>

#include "Maquette.hpp"
#include "Palette.hpp"
#include "Abstract.hpp"
#include "AbstractRelation.hpp"
#include "AbstractSoundBox.hpp"
#include "AbstractControlBox.hpp"
#include "AbstractParentBox.hpp"
#include "BasicBox.hpp"

#include <map>
#include <vector>
#include <string>

class MaquetteView;
class BasicBox;
class AttributesEditor;
class SoundBox;
class BoundingBox;
class MaquetteContextMenu;
class Relation;
class Comment;
class TriggerPoint;
class PlayingThread;

/*!
 * \class MaquetteScene
 *
 * \brief Derived class from Qt's QGraphicsScene. Representation of a Maquette
 * object, see Maquette.h.
 */

/*!
 * \brief Enum handling various user interaction modes.
 */
typedef enum InteractionMode {SELECTION_MODE = 0, CREATION_MODE = 1,
			      RELATION_MODE = 3, TEXT_MODE = 5, TRIGGER_MODE = 6} InteractionMode;
/*!
 * \brief Enum handling various box creation modes.
 */
typedef enum {NB_MODE = -1,CB_MODE = 0,SB_MODE = 1, PB_MODE = 2} BoxCreationMode;
/*!
 * \brief Enum handling various resize modes.
 */
typedef enum {NO_RESIZE, HORIZONTAL_RESIZE, VERTICAL_RESIZE,
	      DIAGONAL_RESIZE} ResizeMode;

class MaquetteScene : public QGraphicsScene

{
  Q_OBJECT

    public :

  MaquetteScene(const QRectF & rect, AttributesEditor *palette);
  virtual ~MaquetteScene();

  /*!
   * \brief Initialises scene parameters.
   */
  void init();
  /*!
   * \brief Updates current view.
   */
  void updateView();
  /*!
   * \brief Returns the current interaction mode.
   *
   * \return the current interaction mode
   */
  int currentMode();
  /*!
   * \brief Defines a new interaction mode with an eventual box creation mode if needed.
   *
   * \param interactionMode : the new interaction mode
   * \param box : the new box creation mode
   */
  void setCurrentMode(int interactionMode, BoxCreationMode box = NB_MODE);
  /*!
   * \brief Gets the current resizing mode.
   *
   * \return the current resizing mode
   */
  int resizeMode() const;
  /*!
   * \brief Sets the current resizing mode.
   *
   * \param resizeMode the new resizing mode
   */
  void setResizeMode(int resizeMode);
  /*!
   * \brief Sets the current main resizing box.
   *
   * \param rb the box
   */
  void setResizeBox(unsigned int rb);
  /*!
   * \brief Requests the maquette for the box identified by ID.
   *
   * \param : the ID of the box wanted
   * \return the box identified
   */
  BasicBox *getBox(unsigned int box);
  /*!
   * \brief Requests the maquette for the relation identified by ID.
   *
   * \param : the ID of the relation wanted
   * \return the relation identified
   */
  Relation *getRelation(unsigned int rel);
  /*!
   * \brief Adds a new comment to the maquette with a specific position.
   *
   * \param comment : the comment text
   * \param position : the position of the comment
   * \param boxID : the ID of the eventual box
   */
  Comment *addComment(const std::string &comment,const QPointF &position, unsigned int boxID);
  /*!
   * \brief Removes a specific comment.
   *
   * \param comment : the comment to remove
   */
  void removeComment(Comment *comment);
  /*!
   * \brief Adds a new trigger point to a box.
   *
   * \param boxID : box to add trigger point
   * \param extremity : box's extremity concerned by the trigger point
   * \param message : the message of the trigger point
   *
   * \return the ID of the trigger added
   */
  int addTriggerPoint(unsigned int boxID, BoxExtremity extremity, const std::string &message);
  /*!
   * \brief Removes a trigger point.
   *
   * \param trgID : trigger point ID to remove
   */
  void removeTriggerPoint(unsigned int trgID);
  /*!
   * \brief Gets a specific trigger point.
   *
   * \param trgID : trigger point ID to remove
   *
   * \return the trigger point
   */
  TriggerPoint *getTriggerPoint(unsigned int trgID);
  /*!
   * \brief Adds a new sound box to the maquette with specific coordinates.
   *
   * \param topLeft : the top left coordinate
   * \param bottomRight : the bottom right coordinate
   * \param name : the name of the box
   * \param palette : the palette to assign to the box
   * \return the ID of box created
   */
  unsigned int addSoundBox(const QPointF &topLeft, const QPointF &bottomRight, const std::string &name, const Palette &palette);
  /*!
   * \brief Adds a new sound box containing specific information.
   *
   * \param abstract : the abstract sound box containing information about the box
   * \return the ID of box created
   */
  unsigned int addSoundBox(const AbstractSoundBox &abstract);
  /*!
   * \brief Adds an existing sound box as an item to the scene.
   *
   * \param ID : the ID to be set
   * \return the ID of box created
   */
  unsigned int addSoundBox(unsigned int ID);
  /*!
   * \brief Adds a new control box to the maquette with specific coordinates.
   *
   * \param topLeft : the top left coordinate
   * \param bottomRight : the bottom right coordinate
   * \param name : the name of the box
   * \return the ID of box created
   */
  unsigned int addControlBox(const QPointF &topLeft, const QPointF &bottomRight, const std::string &name);
  /*!
   * \brief Adds a new control box to the maquette with specific information.
   *
   * \param abstract : the abstract sound box containing information about the box
   * \return the ID of box created
   */
  unsigned int addControlBox(const AbstractControlBox &abstract);
  /*!
   * \brief Adds an existing control box as an item to the maquette with a specific ID.
   *
   * \param ID : the ID to be set
   * \return the ID of box created
   */
  unsigned int addControlBox(unsigned int ID);
  /*!
   * \brief Adds a new parent box to the maquette with specific coordinates.
   *
   * \param topLeft : the top left coordinate
   * \param bottomRight : the bottom right coordinate
   * \param name : the name of the box
   * \return the ID of box created
   */
  unsigned int addParentBox(const QPointF &topLeft, const QPointF &bottomRight, const std::string &name);
  /*!
   * \brief Adds a new parent box to the maquette with specific information.
   *
   * \param abstract : the abstract sound box containing information about the box
   * \return the ID of box created
   */
  unsigned int addParentBox(const AbstractParentBox &abstract);
  /*!
   * \brief Adds an existing parent box as an item to the maquette with a specific ID.
   *
   * \param ID : the ID to be set
   * \return the ID of box created
   */
  unsigned int addParentBox(unsigned int ID);
  /*!
   * \brief Adds a new relation as an item to the maquette.
   *
   * \param abstractRel : the abstract to create relation from
   * \return the ID of relation to create
   */
  int addRelation(const AbstractRelation &abstractRel);
  /*!
   * \brief Change boundaries for a relation.
   *
   * \param relID : the relation to be modified
   * \param length : the new length
   * \param minBound : the new min bound
   * \param maxBound : the new max bound
   */
  void changeRelationBounds(unsigned int relID, const float &length, const float &minBound, const float &maxBound);
  /*!
   * \brief Creates a new interval relation between 2 objects.
   *
   * \param ID1 : the first box ID in the interval
   * \param ID2 : the second box ID in the interval
   * \param value : the interval value
   * \param tolerance : the interval tolerance
   *
   * \return true if the interval could be created
   */
  bool addInterval(unsigned int ID1, unsigned int ID2, int value, int tolerance);
  /*!
   * \brief Removes a relation.
   *
   * \param relID : the relation to be removed
   */
  void removeRelation(unsigned int relID);
  /*!
   * \brief Gets a QString describing a specific relation.
   *
   * \param relID : the relation to get a string from
   * \return the QString describing the relation
   */
  QString getStringFromRelation(unsigned int relID);
  /*!
   * \brief Removes a box both from the scene and maquette.
   *
   * \param box : the box to be removed
   */
  void removeBox(unsigned int box);
  /*!
   * \brief Called to move a set of boxes.
   *
   * \param moved : the boxes to be moved
   */
  void boxesMoved(const std::vector<unsigned int> &moved);
  /*!
   * \brief Apply moves of the current selection confirmed by the Engines.
   */
  void selectionMoved();
  /*!
   * \brief Apply move for a moving box if confirmed by the Engines.
   *
   * \param boxID : the box to be modified
   */
  bool boxMoved(unsigned int boxID);
  /*!
   * \brief Apply resize of the resizing box if confirmed by the Engines.
   */
  void boxResized();
  /*!
   * \brief Gets the palette contained in AttributesEditor.
   *
   * \return the palette contained in AttributesEditor
   */
  Palette getPalette() const;
  /*!
   * \brief Sets the abstractBox of AttributesEditor.
   *
   * \param abBox : the new abstract box to be set
   */
  void setAttributes(AbstractBox *abBox);
  /*!
   * \brief Sets the aspect to store during a box-to-box aspect copying.
   * \param palette : the palette to be stored
   */
  void setCopyAspect(const Palette &palette);
  /*!
   * \bief Gets the aspect to paste during a box-to-box aspect copying.
   *
   * \return the palette to paste
   */
  Palette getCopyAspect() const;
  /*!
   * \brief Gets a set of the available network devices.
   *
   * \return a set of the available network devices
   */
  std::map<std::string,MyDevice> getNetworkDevices();
  /*!
   * \brief Changes the network configuration.
   *
   * \param deviceName : device to be modified
   * \param pluginName : the new plugin for device
   * \param IP : the new IP for device
   * \param port : the new port for device
   */
  void changeNetworkDevice(const std::string &deviceName, const std::string &pluginName, const std::string &IP, const std::string &port);
  /*!
   * \brief Updates messages to send for a specific box.
   *
   * \param boxID : the box to modify messages sent from
   * \return true if messages could be modified
   */
  bool updateMessagesToSend(unsigned int boxID);
  /*!
   * \brief Sends a specific message with current device.
   *
   * \param message : the message to send
   * \return true if message could be sent
   */
  bool sendMessage(const std::string &message);
  /*!
   * \brief Displays a message in a specific warning level.
   *
   * \param message : the message to display
   * \param warningLevel : the level of the message
   */
  void displayMessage(const std::string &message, unsigned int warningLevel);
  /*!
   * \brief Removes the items contained in the selection.
   */
  void removeSelectedItems();
  /*!
   * \brief Trigger with a specific message.
   *
   * \param message : the message to send for triggering
   */
  void trigger(const std::string &message);
  /*!
   * \brief Set trigger point 's message.
   *
   * \param trgID : the trigger point to be modified
   * \param message : the new message
   */
  bool setTriggerPointMessage(unsigned int trgID, const std::string &message);
  /*!
   * \brief Selects the whole set of selectable items.
   */
  void selectAll();
  /*!
   * \brief Shows/Hides tracks.
   *
   * \param view : the new state of tracks viewing activation
   */
  void setTracksView(bool view);
  /*!
   * \brief Gets the current tracks view state.
   *
   * \return the current tracks view state
   */
  bool tracksView();
  /*!
   * \brief Pastes boxes copied at a specified position.
   *
   * \param position the place to paste boxes to
   */
  void pasteBoxes(const QPointF &position);
  /*!
   * \brief Redefinition of QGraphicsScene::clear(). Clears the scene.
   */
  virtual void clear();
  /*!
   * \brief Determines if a relation exists between two boxes.
   *
   * \param ID1 : the ID of the first box
   * \param ID2 : the ID of the second box
   * \return if a relation exists between the two boxes
   */
  bool areRelated(unsigned int ID1, unsigned int ID2);
  /*!
   * \brief Gets the current execution time in ms.
   *
   * \return the current execution time in ms
   */
  unsigned int getCurrentTime();
  /*!
   * \brief Gets the box progress ratio.
   *
   * \return the box progress ratio.
   */
  float getProgression(unsigned int boxID) ;
  /*!
   * \brief Gets the current playing mode.
   *
   * \return the current playing mode
   */
  SndBoxProp::PlayingMode playingMode() const;
  /*!
   * \brief Sets the current playing mode.
   *
   * \param mode the playing mode to be set
   */
  void setPlayingMode(unsigned int mode);
  /*!
   * \brief Gets a set of temporal relations involving a particular entity.
   *
   * \param entityID : the entity to get relations from
   * \return a vector of temporal relations involving entityID
   */
  std::vector<unsigned int> getRelationsIDs(unsigned int entityID);
  /*!
   * \brief Sets the first box of a relation beeing created.
   *
   * \param ID : the ID of the box
   * \param extremumType : the extremity of the box
   */
  void setRelationFirstBox(unsigned int ID, BoxExtremity extremumType);
  /*!
   * \brief Called when a possible box for relation in creation is found
   */
  void relationBoxFound();
  /*!
   * \brief Determines if the document was modified.
   *
   * \return if the document was modified
   */
  bool documentModified() const;
  /*!
   * \brief Sets the document as modified or not.
   *
   * \param modified : the new modified state
   */
  void setModified(bool modified);
  /*!
   * \brief Saves the current composition into a file.
   *
   * \param fileName : the file to save current composition into
   */
  void save(const std::string &fileName);
  /*!
   * \brief Loads a file into a new composition.
   *
   * \param fileName : the file to load composition from
   */
  void load(const std::string &fileName);
  /*!
   * \brief Determines the playing state.
   *
   * \return the playing state
   */
  bool playing() const ;
  /*!
   * \brief Determines the paused state.
   *
   * \return the paused state
   */
  bool paused() const ;
  /*!
   * \brief Updates the boxes currently playing.
   */
  void updatePlayingBoxes();
  /*!
   * \brief Set the stored playing state of a box.
   *
   * \param boxID : the box to change playing state
   * \param playing : the playing state
   */
  void setPlaying(unsigned int boxID, bool playing);

  //! Integer handling scene's maximal width.
  static const int MAX_SCENE_WIDTH = 100000;
  //! Integer handling scene's maximal height.
  static const int MAX_SCENE_HEIGHT = 100000;
  //! Integer handling scene's milliseconds per pixel rate.
  static float MS_PER_PIXEL;
  //! Float handling scene's precision in milliseconds.
  static const float MS_PRECISION;
  //! Default trigger message used.
  static const std::string DEFAULT_TRIGGER_MSG;

 protected :

  /*!
   * \brief Redifinition of QGraphicsScene::drawItems().
   * This method is automatically called by QGraphicsScene::update().
   *
   * \param painter : the painter used to draw
   * \param numItem : the number of items into the scene
   * \param items : collection of the graphical items
   * \param options : array of options
   * \param widget : the widget that is being painting on
   */
  virtual void drawItems(QPainter *painter, int numItems, QGraphicsItem *items[],
		 const QStyleOptionGraphicsItem options[],
		 QWidget *widget);

  /*!
   * \brief Redefinition of QGraphicsScene::drawForeground().
   * This method is automatically called by QGraphicsScene::update().
   *
   * \param painter : the painter used to draw
   * \param rect : the bounding rectangle
   */
  virtual void drawForeground ( QPainter * painter, const QRectF & rect );
  /*!
   * \brief Redefinition of QGraphicsScene::mousePressEvent().
   * Called when a mouse button is pressed on the scene.
   *
   * \param mouseEvent : contains the information about the event
   */
  virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
  /*!
   * \brief Redefinition of QGraphicsScene::mouseMoveEvent().
   * Called when the mouse pointer is moving on the scene.
   *
   * \param mouseEvent : contains the information about the event
   */
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
  /*!
   * \brief Redefinition of QGraphicsScene::mouseReleaseEvent().
   * Called when a mouse button is released on the scene.
   *
   * \param mouseEvent : contains the information about the event
   */
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
  /*!
   * \brief Redefinition of QGraphicsScene::contextMenuEvent().
   * Called when a mouse right button is released on the scene.
   *
   * \param event contains the information about the event
   */
  virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
  /*!
   * \brief Redefinition of QGraphicsScene dragEnter method.
   * Raised a drag&drop object enters in the scene.
   *
   * \param event : the information about the event
   */
  virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
  /*!
   * \brief Redefinition of QGraphicsScene dragMove method.
   * Raised when a drag&drop object moves in the scene.
   *
   * \param event : the information about the event
   */
  virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
  /*!
   * \brief Redefinition of QGraphicsScene dragLeave method.
   * Raised a drag&drop object leaves the scene.
   *
   * \param event : the information about the event
   */
  virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
  /*!
   * \brief Redefinition of QGraphicsScene dropEvent method.
   * Raised when a drag&drop object is dropped on the scene.
   *
   * \param event : the information about the event
   */
  virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

  public slots :

  /*!
   * \brief Cuts selected boxes.
   */
  void cutBoxes();
  /*!
   * \brief Copies selected boxes.
   */
  void copyBoxes(bool erasing = false);
  /*!
   * \brief Pastes copied boxes.
   */
  void pasteBoxes();
  /*!
   * \brief Called when the time reaches the end.
   */
  void timeEndReached();
  /*
   * \brief Called to update the next starting time.
   *
   * \param value : the new starting time
   */
  void updateStartingTime(int value);
  /*!
   * \brief Plays the whole composition.
   */
  void play();
  /*!
   * \brief Stops playing the composition.
   */
  void stop();
  /*!
   * \brief Pauses playing the composition.
   */
  void pause();

 private :

  /*!
   * \brief Makes name sequential.
   * \param name : the box name
   * \return the string containing computed name
   */
  std::string sequentialName(const std::string &name);
  /*!
   * \brief Looks for a mother for specified coordinates.
   * \param topLeft : the top left coordinates of the box
   * \param size : the size of the box
   * \return the ID of the possible mother
   */
  unsigned int findMother(const QPointF &topLeft, const QPointF &size);
  /*!
   * \brief Adds a box.
   *
   * \param box : the box creation mode
   */
  void addBox(BoxCreationMode box);
  /*!
   * \brief Adds a new sound box using internal stored coordinates.
   *
   * \return the new box ID
   */
  unsigned int addSoundBox();
  /*!
   * \brief Adds a new control box using internal stored coordinates.
   *
   * \return the new box ID
   */
  unsigned int addControlBox();
  /*!
   * \brief Adds a new parent box using internal stored coordinates.
   *
   * \return the new box ID
   */
  unsigned int addParentBox();
  /*!
   * \brief Sets the second box of a relation beeing created.
   *
   * \param ID : the ID of the box
   * \param extremumType : the extremity of the box
   */
  void setRelationSecondBox(unsigned int ID, BoxExtremity extremumType);
  /*!
   * \brief Adds a relation using internal stored relations bounds.
   */
  int addPendingRelation();
  /*!
   * \brief Gets the current box mode creation.
   *
   * \return the current box mode creation (can be NoBox)
   */
  int currentBoxMode();

  MaquetteView *_view; //!< The QGraphicsView related.
  AttributesEditor *_editor; //!< The logical representation of the Editor.
  Maquette *_maquette;  //!< The logical representation of the Maquette.

  Palette _copyPalette; //!< Used to store sound box aspect beeing copied from one box to another.

  std::vector<Abstract*> _toCopy; //!< Used to store items beeing copied.
  std::map<unsigned int,AbstractBox*> _boxesToCopy;
  QPointF _copyPos; //!< Used to store position to copy from.
  QPointF _copySize; //!< Used to store size of selection copied.

  bool _modified; //!< Handling document 'was modified' state.

  QPointF _pressPoint; //!< Last pression point.
  QPointF _releasePoint; //!< Last release point.
  QGraphicsRectItem *_tempBox; //!< Temporary box when a creation is in progress.

  int _currentZValue; //!< ZValue to be attributed to the next bounding box.
  bool _tracksView; //! Visibility of track.
  int _resizeMode; //! Current resizing mode.
  int _currentInteractionMode; //!< Current interaction mode.
  int _currentBoxMode; //!< Current box creation mode.
  int _savedInteractionMode; //!< Saved interation mode.
	int _savedBoxMode; //!< Saved box interation mode.
  SndBoxProp::PlayingMode _playMode; //!< Current playing mode.

  unsigned int _resizeBox; //!< During a resizing operation, the concerned box
  bool _clicked; //!< Handles if a click just occured.
  bool _playing; //!< Handles playing state.
  bool _paused; //!< Handles paused state.

  unsigned int _startingValue; //!< Starting time in ms.

  QPointF _mousePos; //!< Handles mouse current position.
  bool _relationBoxFound; //!< Determines if a relation can be created through release.

  //! Handles information about relation beeing created.
  AbstractRelation * _relation;

  std::map<unsigned int,BasicBox*> _playingBoxes; //!< Handles the whole set of currently playing boxes

  PlayingThread *_playThread; //!< The thread handling playing.
};

#endif
