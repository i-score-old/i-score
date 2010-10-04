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
#ifndef MAQUETTE_H
#define MAQUETTE_H

/*!
 * \file Maquette.h
 * \author Luc Vercellin, Bruno Valeze
 * \date 30 september 2009
 */

#include <QObject>
#include <QPoint>

#include <vector>
#include <map>
#include <string>
#include <list>
#include <utility>
#include <sstream>
#include "Palette.hpp"

#include "CSPTypes.hpp"
#include "BasicBox.hpp"

//! Default network host.

#define NETWORK_LOCALHOST "127.0.0.1"
//! Default network port.
static const int NETWORK_PORT = 7000;
#define NETWORK_PORT_STR "7000"

class PaletteActor;

class SoundBox;
class BasicBox;
class ParentBox;
class BoundingBox;
class MaquetteScene;
class QDomDocument;
class Engines;
class AbstractSoundBox;
class AbstractControlBox;
class AbstractParentBox;
class AbstractRelation;
class Relation;
class TriggerPoint;

//! Enum containing various error messages.
typedef enum {SUCCESS = 1, NO_MODIFICATION = 0, RETURN_ERROR = -1,
	      ARGS_ERROR = -2} ErrorMessage;

//! Enum containing various warning levels.

typedef enum {INDICATION_LEVEL,WARNING_LEVEL,ERROR_LEVEL} ErrorLevel;

/*!
 * \brief Structure used to contain boxes position or size.
 * Used for interaction with MaquetteScene.
 */
struct Coords {
  float topLeftX;
  float topLeftY;
  float sizeX;
  float sizeY;
};

class MyDevice {
public :
	MyDevice(const std::string &nameArg = "NO_STR", const std::string &pluginArg = "NO_STR",
			unsigned int networkPortArg = NETWORK_PORT, const std::string &networkHostArg = NETWORK_LOCALHOST)
	: name(nameArg),plugin(pluginArg),networkPort(networkPortArg), networkHost(networkHostArg) {
	}
	MyDevice(const MyDevice &other) {
		name = other.name;
		plugin = other.plugin;
		networkPort = other.networkPort;
		networkHost = other.networkHost;
	}
	~MyDevice() {
	}
	std::string name;
	std::string plugin;
	unsigned int networkPort;
	std::string networkHost;
};

/*!
 * \class Maquette
 *
 * \brief Representation of a musical piece.
 *
 * A musical piece will be represented by this class. It's composed of a set
 * of boxes organised on a a timeline and possibly with temporal constraints.
 * A Maquette is associated with a MaquetteScene that manages graphical
 * representation and user interaction.
 *
 */

class Maquette : public QObject
{
  Q_OBJECT

    public :

  static Maquette *getInstance() {
    static Maquette *instance = new Maquette();
    return instance;
  }

	 ~Maquette();

	/*!
	 * \brief Initialise the maquette elements.
	 */
  void init();
	/*!
	 * \brief Sets a new scene.
	 *
	 * \param scene : the new scene to be set
	 */
  void setScene(MaquetteScene *scene);
  /*!
   * \brief Evaluates the global duration of the maquette.
   *
   * \return the duration of the maquette
   */
  int duration();
  /*!
   * \brief Gets a set of relations IDs involving a particular entity.
   *
   * \param entityID : the entity to get relations from
   * \return a vector of relations IDs involving entityID
   */
  std::vector<unsigned int> getRelationsIDs(unsigned int entityID);
  /*!
   * \brief Gets the current network device used.
   *
   * \return the current network device used
   */
  std::string getNetworkDevice();
  /*!
   * \brief Gets a set of the available network devices.
   *
   * \return a set of the available network devices
   */
  std::map<std::string,MyDevice> getNetworkDevices();
  /*!
   * \brief Gets the current network port used.
   *
   * \return the current network port used
   */
  std::string getListeningPort();
  /*!
   * \brief Gets a set of the available listening ports.
   *
   * \return a set of the available listening ports
   */
  std::vector<std::string> getListeningPorts();
  /*!
   * \brief Gets the current network IP used.
   *
   * \return the current network IP used
   */
  std::string getNetworkHost();
  /*!
   * \brief Updates a specific network device with new parameters.
   *
   * \param deviceName : the device to be set
   * \param pluginName : the new plugin to use
   * \param IP : the new IP to use
   * \param port : the new port to use
   */
  void changeNetworkDevice(const std::string &deviceName, const std::string &pluginName, const std::string &IP, const std::string &port);

  void getNetworkDeviceNames(std::vector<std::string>& deviceName, std::vector<bool>& namespaceRequestable);

  std::vector<std::string> requestNetworkSnapShot(const std::string &address);



	void addCurve(unsigned int boxID, const std::string &address);

	void removeCurve(unsigned int boxID, const std::string &address);

	void clearCurves(unsigned int boxID);

	std::vector<std::string> getCurvesAddresses(unsigned int boxID);

/*
	void setCurveSampleRate(unsigned int boxID, const std::string &address, unsigned int sampleRate);

	unsigned int getCurveSampleRate(unsigned int boxID, const std::string &address);

	void setCurveRedundancy(unsigned int boxID, const std::string &address, bool redundancy);

	bool getCurveRedundancy(unsigned int boxID, const std::string &address);

	void getCurveArgTypes(const std::string &stringToParse, std::vector<std::string>& result);*/

	bool setCurveSections(unsigned int boxID, const std::string &address, unsigned int argPosition,
				const std::vector<float> &xPercents, const std::vector<float> &yValues, const std::vector<short> &sectionType, const std::vector<float> &coeff);

/*	bool getCurveSections(unsigned int boxID, const std::string &address, unsigned int argPosition,
				std::vector<float> &xPercents, std::vector<float> &yValues, std::vector<short> &sectionType, std::vector<float> &coeff);

	bool getCurveValues(unsigned int boxID, const std::string &address, unsigned int argPosition, std::vector<float>& values);
*/

	bool getCurveAttributes(unsigned int boxID, const std::string &address, unsigned int argPosition, unsigned int & sampleRate,
			bool &redundancy, std::vector<float>& values,	std::vector<std::string> & argTypes, std::vector<float> &xPercents,
			std::vector<float> &yValues, std::vector<short> &sectionType, std::vector<float> &coeff);

/*	bool setCurveAttributes(unsigned int boxID, const std::string &address, unsigned int argPosition, const std::vector<float>& values,
			const std::vector<float> &xPercents, const std::vector<float> &yValues, const std::vector<short> &sectionType, const std::vector<float> &coeff);*/

  /*!
   * \brief Raised when execution is finished
   */
  void executionFinished();
  /*!
   * \brief Updates messages to send for a specific box.
   *
   * \param boxID : the box to modify messages sent from
   * \return true if messages could be modified
   */
  bool updateMessagesToSend(unsigned int boxID);
  /*!
   * \brief Gets the set of messages to send for the beginning of a box.
   *
   * \param boxID : the box to get messages set from
   *
   * \return a set of the messages to send for the beginning of a box
   */
  std::vector<std::string> firstMessagesToSend(unsigned int boxID);
  /*!
   * \brief Gets the set of messages to send for the end of a box.
   *
   * \param boxID : the box to get messages set from
   *
   * \return a set of the messages to send for the end of a box
   */
  std::vector<std::string> lastMessagesToSend(unsigned int boxID);
  /*!
   * \brief Sets the set of messages to send for the beginning of a box.
   *
   * \param boxID : the box to get messages set from
   * \param messages : the new set of the messages
   *
   * \return if messages could be set
   */
  bool setFirstMessagesToSend(unsigned int boxID, const std::vector<std::string> &messages);
  /*!
   * \brief Sets the set of messages to send for the end of a box.
   *
   * \param boxID : the box to get messages set from
   * \param messages : the new set of the messages
   *
   * \return if messages could be set
   */
  bool setLastMessagesToSend(unsigned int boxID, const std::vector<std::string> &messages);
  /*!
   * \brief Sends a specific message with current device.
   *
   * \param message : the message to send
   * \return true if message could be sent
   */
  bool sendMessage(const std::string &message);
  /*!
   * \brief Adds a sound box to the maquette.
   *
   * \param corner1 : a first corner of the box's bounding rectangle
   * \param corner2 : the second corner of the box's bounding rectangle
   * \param name : the name given to the box
   * \param mother : the mother of the box
   * \return the ID of created sound box
   */
  unsigned int addSoundBox(const QPointF & corner1, const QPointF & corner2, const std::string & name, unsigned int mother);
  /*!
   * \brief Adds a new sound box to the maquette with specific information.
   *
   * \param abstract : the abstract sound box containing information about the box
   * \return the ID of box created
   */
  unsigned int addSoundBox(const AbstractSoundBox &abstract);
  /*!
   * \brief Adds a control box to the maquette.
   *
   * \param corner1 : a first corner of the box's bounding rectangle
   * \param corner2 : the second corner of the box's bounding rectangle
   * \param name : the name given to the box
   * \param mother : the mother of the box
   * \return the ID of created control box
   */
  unsigned int addControlBox(const QPointF & corner1, const QPointF & corner2, const std::string & name, unsigned int mother);
  /*!
   * \brief Adds a new control box to the maquette with specific information.
   *
   * \param abstract : the abstract control box containing information about the box
   * \return the ID of box created
   */
  unsigned int addControlBox(const AbstractControlBox &abstract);
  /*!
   * \brief Adds a parent box to the maquette.
   *
   * \param corner1 : a first corner of the box's bounding rectangle
   * \param corner2 : the second corner of the box's bounding rectangle
   * \param name : the name given to the box
   * \param mother : the mother of the box
   * \return the ID of created parent box
   */
  unsigned int addParentBox(const QPointF & corner1, const QPointF & corner2, const std::string & name, unsigned int mother);
  /*!
   * \brief Adds a new parent box to the maquette with specific information.
   *
   * \param abstract : the abstract parent box containing information about the box
   * \return the ID of box created
   */
  unsigned int addParentBox(const AbstractParentBox &abstract);

  std::map<unsigned int,ParentBox*> parentBoxes();

  /*!
   * \brief Adds a new AntPost relation between 2 objects extremities.
   *
   * \param ID1 : the first box in the relation
   * \param firstExtremum : the extremity of the first box used for relation
   * \param ID2 : the second box in the relation
   * \param secondExtremum : the extremity of the second box used for relation
   * \param antPostType : the anterior/posterior relation type
   * \return a specific error message in ErrorMessage enum
   */
  int addRelation(unsigned int ID1, BoxExtremity firstExtremum, unsigned int ID2,
		  BoxExtremity secondExtremum, int antPostType);
  /*!
   * \brief Adds a new AntPost relation described by a AbstractRelation.
   *
   * \param abstract : the information about the relation
   * \return a specific error message in ErrorMessage enum
   */
  int addRelation(const AbstractRelation &abstract);

  void changeRelationBounds(unsigned int relID, const float &minBound, const float &maxBound);
  /*!
   * \brief Creates a new interval relation between 2 objects.
   *
   * \param ID1 : the first box ID in the interval
   * \param ID2 : the second box ID in the interval
   * \param value : the interval value
   * \param tolerance : the interval tolerance
   * \return true if the interval could be created
   */
  bool addInterval(unsigned int ID1, unsigned int ID2, int value, int tolerance);
  /*!
   * \brief Removes a temporal relation.
   * \param relationID : the relation to remove
   */
  void removeRelation(unsigned int relationID);
  /*!
   * \brief Determines if a relation exists between two boxes.
   *
   * \param ID1 : the first box in possible relation
   * \param ID2 : the first box in possible relation
   * \return if boxes are related
   */
  bool areRelated(unsigned int ID1, unsigned int ID2);
  /*!
   * \brief Adds a new trigger point and link it to a control point.
   *
   * \param boxID : the box concerned by the trigger point
   * \param extremity : the extremity of the box to link to trigger point
   * \param message : the message to link with the trigger point
   *
   * \return a specific error message in ErrorMessage enum
   */
  int addTriggerPoint(unsigned int boxID, BoxExtremity extremity, const std::string &message);
  int addTriggerPoint(const AbstractTriggerPoint &abstract);
  /*!
   * \brief Removes a trigger point.
   *
   * \param trgID : the trigger point ID
   */
  void removeTriggerPoint(unsigned int trgID);
  /*!
   * \brief Gets a trigger point.
   *
   * \param trgID : the trigger point ID
   *
   * \return the trigger point
   */
  TriggerPoint* getTriggerPoint(unsigned int trgID);
  /*!
   * \brief Simulates a message reception for a trigger point.
   *
   * \param message : the message to simulate
   */
  void simulateTriggeringMessage(const std::string &message);
  /*!
   * \brief Set trigger point 's message.
   *
   * \param trgID : the trigger point to be modified
   * \param message : the new message
   */
  bool setTriggerPointMessage(unsigned int trgID, const std::string &message);
  /*!
   * \brief Perform moving or resizing for a set of boxes.
   *
   * \param boxes : the boxes that have to be transformed with their new coordinates
   * \return false if the Engines can not perform the transformation
   */
  bool updateBoxes(const std::map<unsigned int,Coords> &boxes);

  bool updateBox(unsigned int boxID, const Coords &coord);
  /*!
   * \brief Informs relations that boxes have been moved.
   */
  void updateRelations();
  /*!
   * \brief Clears the maquette.
   */
  void clear();
  /*!
   * \brief Remove a box from the maquette.
   *
   * \param boxID : the box to remove
   * \return removed temporal relations
   */
  std::vector<unsigned int> removeBox(unsigned int boxID);
  /*!
   * \brief Gets a specific box.
   *
   * \param : the ID of the box wanted
   * \return the box specified by the ID
   */
  BasicBox* getBox(unsigned int ID);
  /*!
   * \brief Gets a specific relation.
   *
   * \param : the ID of the relation wanted
   * \return the relation specified by the ID
   */
  Relation* getRelation(unsigned int ID);
  /*!
   * \brief Returns the next sequential name number.
   *
   * \return the next ID to be used for box creation
   */
  unsigned int sequentialID();
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
   * \brief Gets the current execution time in ms.
   *
   * \return the current execution time in ms
   */
  unsigned int getCurrentTime() const;
  /*!
   * \brief Gets the box progress ratio.
   *
   * \return the box progress ratio
   */
  float getProgression(unsigned int boxID) ;

	int requestNetworkNamespace(const std::string &address, std::vector<std::string>& nodes, std::vector<std::string>& leaves,
						 std::vector<std::string>& attributes, std::vector<std::string>& attributesValue);

  void updateBoxesFromEngines();

  public slots :

  /*!
   * \brief Launches the playing process at a specific time.
   *
   * \param startTime : the relative date of the maquette when playing starts
   */
  void startPlaying(unsigned int startTime);
  /*!
   * \brief Stops the playing process.
   */
  void stopPlaying();
  /*!
   * \brief Sets a new acceleration factor.
   *
   * \param factor : the new acceleration factor value between 0 and 1
   */
  void setAccelerationFactor(const float &factor);
  /*!
   * \brief Called by the callback when a transition is crossed.
   *
   * \param boxID : the box whose transition is crossed
   * \param CPIndex : index of the box's control point crossed
   */
  void crossedTransition(unsigned int boxID, unsigned int CPIndex);
  /*!
   * \brief Called by the callback when a triggerPoint is triggered.
   *
   * \param waiting : new waiting state of the triggerPoint
   * \param trgID : trigger point ID
   */
  void crossedTriggerPoint(bool  waiting, unsigned int trgID);

 private :

	 Maquette();

  /*!
   * \brief Adds a sound box existing in Engines to the maquette with given ID and palette.
   *
   * \param ID : the ID to assign to the box
   * \param corner1 : a first corner of the box's bounding rectangle
   * \param corner2 : the second corner of the box's bounding rectangle
   * \param name : the name given to the box
   * \param pal : the palette to assign to the box
   * \param mother : the mother to assign to the box
   * \return the ID of created sound box
   */
  unsigned int addSoundBox(unsigned int ID, const QPointF & corner1, const QPointF & corner2, const std::string &name,
			   const Palette &pal, unsigned int mother);
  /*!
   * \brief Adds a box existing in Engines to the maquette with given ID.
   *
   * \param ID : the ID to assign to the box
   * \param corner1 : a first corner of the box's bounding rectangle
   * \param corner2 : the second corner of the box's bounding rectangle
   * \param name : the name given to the box
   * \param mother : the mother to assign to the box
   * \return the ID of created sound box
   */
  unsigned int addControlBox(unsigned int ID, const QPointF & corner1, const QPointF & corner2, const std::string &name,
			   unsigned int mother);
  /*!
   * \brief Adds a box existing in Engines to the maquette with given ID.
   *
   * \param ID : the ID to assign to the box
   * \param corner1 : a first corner of the box's bounding rectangle
   * \param corner2 : the second corner of the box's bounding rectangle
   * \param name : the name given to the box
   * \param mother : the mother to assign to the box
   * \return the ID of created sound box
   */
  unsigned int addParentBox(unsigned int ID, const QPointF & corner1, const QPointF & corner2, const std::string &name,
			   unsigned int mother);

  /*!
   * \brief Saves a box into doc.
   *
   * \param ID : the ID of the box to be saved
   */
  void saveBox(unsigned int ID);

  void updateCurves(unsigned int boxID, const std::vector<std::string> &startMsgs, const std::vector<std::string> &endMsgs);

  void updateBoxesFromEngines(const std::vector<unsigned int> &movedBoxes);

  //! The MaquetteScene managing display and interaction.
  MaquetteScene *_scene;
  //! The Engines object managing temporal constraints.
  Engines *_engines;

  //! The map of boxes (identified by IDs) managed by the maquette.
  std::map<unsigned int,BasicBox*> _boxes;
  //! The map of parent boxes (identified by IDs) managed by the maquette.
  std::map<unsigned int,ParentBox*> _parentBoxes;
  //! The map handling a set of control points for each box managed by the maquette.
  std::map<unsigned int,std::pair<unsigned int,unsigned int> > _controlPoints;
  //! The set of relations managed by the maquette.
  std::map<unsigned int,Relation*> _relations;
  //! The set of triggers points managed by the maquette.
  std::map<unsigned int,TriggerPoint*> _triggerPoints;

  //! The next ID to be used for sequential name purpose
  //unsigned int _currentID;

  //! The set of handled devices.
  std::map<std::string,MyDevice> _devices;
  //! The set of available Engines listening ports.
  std::vector<unsigned int> _listeningPorts;

  //Device _defaultDevice; //!< The default network device used.

  bool _recording;  //!< Handling recording state.
  bool _paused;   //!< Handling paused state.

  QDomDocument *_doc; //!< Handling document used for saving/loading.
};

/*!
 * \brief Callback called when a transition is crossed.
 *
 * \param boxID : the box whose transition is crossed
 * \param CPIndex : index of the box's control point crossed
 */
void crossTransitionCallback(unsigned int boxID, unsigned int CPIndex, std::vector<unsigned int> processesToStop);
/*!
 * \brief Callback called when a Trigger Point is triggered is crossed.
 *
 * \param waiting : the waiting state of the Trigger Point
 * \param trgID : index of the box's Trigger Point crossed
 * \param boxID : the box whose transition is crossed
 * \param CPIndex : index of the box's control point associated with trigger point
 * \param message : the message of the Trigger Point
 */
void crossTriggerPointCallback(bool waiting, unsigned int trgID, unsigned int boxID, unsigned int CPIndex, std::string message);
/*!
 * \brief Callback called when the execution is finished.
 */
void executionFinishedCallback();

#endif
