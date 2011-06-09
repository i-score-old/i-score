/*
Copyright: LaBRI (http://www.labri.fr)

Author(s): Raphael Marczak
Last modification: 08/03/2010

Adviser(s): Myriam Desainte-Catherine (myriam.desainte-catherine@labri.fr)

This software is a computer program whose purpose is to propose
a library for interactive scores edition and execution.

This software is governed by the CeCILL-C license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-C
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
knowledge of the CeCILL-C license and that you accept its terms.
*/

#ifndef ENGINES_HPP_
#define ENGINES_HPP_

/*!
 * \file Engines.hpp
 * \author Raphael Marczak (LaBRI)
 * \date 2008-2009
 *
 * \brief This file contains the only class a user have to instantiate and manipulate
 * for all edition and execution manipulation.
 */

#include <string>
#include <map>
#include <vector>

#define CURVE_POW 1

#include "CSPTypes.hpp"

#define ENGINE_VERSION "0.8"

#define PLUGGINS_DEFAULT_LOCATION "/Library/Application Support/Virage/"
#define SCENARIO_SIZE 600000

class EnginesPrivate;
class ECOMachine;
class Editor;

enum EnginesMessageManagement { UNKNOWN_MESSAGE, TRIGGER_MESSAGE, ENGINES_PLAY, ENGINES_PAUSE_MODIFICATION, ENGINES_STOP, ENGINES_SPEED_MODIFICATION, ENGINES_REWIND, ENGINES_GOTO_MODIFICATION };

/*!
 * \class Engines
 *
 * \author Raphael Marczak (LaBRI)
 * \date 2008-2009
 *
 * \brief Main class which contains all functions for edition and execution.
 *
 * This class is the only class the user have to instance and manipulate.
 * It contains all functions for edition and execution.
 */
class Engines
{
public:
	static const int  REQUEST_NOT_SENT = -2;
	static const int  TIMEOUT_EXCEEDED = -1;
	static const int  NO_ANSWER = 0;
	static const int  ANSWER_RECEIVED = 1;

	/*!
	 * Engines constructor.
	 *
	 * \param plugginsLocation : (optional) network plugins location (absolute).
	 */
	Engines(unsigned int scenarioSize = SCENARIO_SIZE, std::string pluginsLocation = PLUGGINS_DEFAULT_LOCATION);

	/*!
	 * Engines constructor.
	 *
	 * \param crossAction : callback, called during the execution when a control point is crossed.
	 * This callback gives information about the box id, and the control point index.
	 * \param triggerAction : callback, called during the execution, when a trigger point has a new state.
	 * This callback gives information about its waiting state, the trigger id, the box id, the control point index
	 * and the waited network message.
	 * \param isExecutionFinished : (optional) callback, called when the execution is finished
	 * (with a stop action, or because the scenario ended normally).
	 * \param plugginsLocation : (optional) network plugins location (absolute).
	 */
	Engines(void(*crossAction)(unsigned int, unsigned int, std::vector<unsigned int>),
			void(*triggerAction)(bool, unsigned int, unsigned int, unsigned int, std::string),
			void(*isExecutionFinished)(),
			unsigned int scenarioSize = SCENARIO_SIZE,
			std::string plugginsLocation = PLUGGINS_DEFAULT_LOCATION);

	/*!
	 * Engines destructor.
	 */
	virtual ~Engines();

	/*!
	 * Resets both engines (all is remove in the editor and the execution machine). All the callback are removed too.
	 */
	void reset(unsigned int scenarioSize = SCENARIO_SIZE);

	/*!
	 * Resets both engines (all is remove in the editor and the execution machine) and specifies new callback.
	 *
	 *  \param crossAction : callback, called during the execution when a control point is crossed.
	 * This callback gives information about the box id, and the control point index.
	 * \param triggerAction : callback, called during the execution, when a trigger point has a new state.
	 * This callback gives information about its waiting state, the trigger id, the box id, the control point index
	 * and the waited network message.
	 */
	void reset(void(*crossAction)(unsigned int, unsigned int, std::vector<unsigned int>),
			void(*triggerAction)(bool, unsigned int, unsigned int, unsigned int, std::string),
			unsigned int scenarioSize = SCENARIO_SIZE);


	/*!
	 * Adds the callback called during the execution when a control point is crossed.
	 * This callback gives information about the box id, and the control point index.
	 *
	 * \param pt2Func : the callback.
	 */
	void addCrossingCtrlPointCallback(void(*pt2Func)(unsigned int, unsigned int, std::vector<unsigned int>));

	/*!
	 * Removes the callback called during the execution when a control point is crossed.
	 */
	void removeCrossingCtrlPointCallback();

	/*!
	 * Adds the callback called during the execution when a control point is crossed.
	 * This callback gives information about the box id, and the control point index.
	 *
	 * \param pt2Func : the callback.
	 */
	void addCrossingTrgPointCallback(void(*pt2Func)(bool, unsigned int, unsigned int, unsigned int, std::string));

	/*!
	 * Removes the callback called during the execution when a control point is crossed.
	 */
	void removeCrossingTrgPointCallback();

	/*!
	 * Adds the callback called when the execution is finished
	 * (with a stop action, or because the scenario ended normally).
	 *
	 * \param pt2Func : the callback.
	 */
	void addExecutionFinishedCallback(void(*pt2Func)());

	/*!
	 * Removes the callback called when the execution is finished
	 */
	void removeExecutionFinishedCallback();

	void addEnginesNetworkUpdateCallback(void(*pt2Func)(unsigned int, std::string, std::string));
	void removeEnginesNetworkUpdateCallback();




	// Edition ////////////////////////////////////////////////////////////////////////

	/*!
	 * Adds a new box in the CSP.
	 *
	 * \param boxBeginPos : the begin value in ms.
	 * \param boxLength : the length value in ms.
	 * \param motherId : mother box ID if any, NO_ID if the box to create has no mother.
	 * \param maxSceneWidth : the max scene width.
	 *
	 * \return the newly created box ID.
	 */
	unsigned int addBox(int boxBeginPos, int boxLength, unsigned int motherId);

	/*!
	 * Adds a new box in the CSP, with a min bound and max bound for the box length.
	 *
	 * \param boxBeginPos : the begin value in ms.
	 * \param boxLength : the length value in ms.
	 * \param motherId : mother box ID if any, NO_ID if the box to create has no mother.
	 * \param maxSceneWidth : the max scene width in ms.
	 * \param minBound : the min bound for the box length in ms.
	 * \param maxBound : the max bound for the box length in ms. NO_BOUND if the max bound is not used (+infinity).
	 *
	 * \return the newly created box ID
	 */
	unsigned int addBox(int boxBeginPos, int boxLength, unsigned int motherId, int minBound, int maxBound);

	/*!
	 * Changes min bound and max bound for the box length.
	 *
	 * \param boxId : the box to change the bounds.
	 * \param minBound : the min bound for the box length in ms.
	 * \param maxBound : the max bound for the box length in ms. NO_BOUND if the max bound is not used (+infinity)
	 */
	void changeBoxBounds(unsigned int boxId, int minBound, int maxBound);

	/*!
	 * Removes a box from the CSP : removes the relation implicating it and the
	 * box's variables.
	 *
	 * \param boxId : the ID of the box to remove.
	 */
	void removeBox(unsigned int boxId);

	void setBoxOptionalArgument(unsigned int boxId, std::string key, std::string value);

	void removeBoxOptionalArgument(unsigned int boxId, std::string key);

	std::map<std::string, std::string> getBoxOptionalArguments(unsigned int boxId);

	/*!
	 * Adds a AntPostRelation between two controlPoints.
	 *
	 * \param boxId1 : the ID of the first box
	 * \param controlPoint1 : the index of the point in the first box to put in relation
	 * \param boxId2 : the ID of the second box
	 * \param controlPoint2 : the index of the point in the second box to put in relation
	 * \param type : the relation type
	 * \param movedBoxes : empty vector, will be filled with the ID of the boxes moved by this new relation
	 *
	 * \return the newly created relation id (NO_ID if the creation is impossible).
	 */
	unsigned int addTemporalRelation(unsigned int boxId1, unsigned int controlPoint1,
			unsigned int boxId2, unsigned int controlPoint2, TemporalRelationType type,
			std::vector<unsigned int>& movedBoxes);

	/*!
	 * Adds a AntPostRelation between two controlPoints, with min bound and max bound for relation length.
	 *
	 * \param boxId1 : the ID of the first box.
	 * \param controlPoint1 : the index of the point in the first box to put in relation.
	 * \param boxId2 : the ID of the second box.
	 * \param controlPoint2 : the index of the point in the second box to put in relation.
	 * \param type : the relation type.
	 * \param minBound : the min bound for the relation length in ms.
	 * \param maxBound : the max bound for the relation length in ms. NO_BOUND if the max bound is not used (+infinity).
	 * \param movedBoxes : empty vector, will be filled with the ID of the boxes moved by this new relation.
	 *
	 * \return the newly created relation id (NO_ID if the creation is impossible).
	 */
	unsigned int addTemporalRelation(unsigned int boxId1, unsigned int controlPoint1, unsigned int boxId2,
			unsigned int controlPoint2, TemporalRelationType type, int minBound, int maxBound,
			std::vector<unsigned int>& movedBoxes);

	/*!
	 * Changes min bound and max bound for the relation length.
	 *
	 * \param relationId : the relation to change the bounds.
	 * \param minBound : the min bound for the box relation in ms.
	 * \param maxBound : the max bound for the box relation in ms. NO_BOUND if the max bound is not used (+infinity).
	 * \param movedBoxes : empty vector, will be filled with the ID of the boxes moved by this new relation.
	 */
	void changeTemporalRelationBounds(unsigned int relationId, int minBound, int maxBound, std::vector<unsigned int>& movedBoxes);

	/*!
	 * Checks if a relation exists between the two given control points.
	 *
	 * \param boxId1 : the id of the first box.
	 * \param controlPoint1Index : the index of the control point in the first box.
	 * \param boxId2 : the id of the second box.
	 * \param controlPoint2Index : the index of the control point in the second box.
	 *
	 * \return true if a relation exists between the two given control points.
	 */
	bool isTemporalRelationExisting(unsigned int boxId1, unsigned int controlPoint1Index, unsigned int boxId2, unsigned int controlPoint2Index);

	/*!
	 * Removes the temporal relation using given id.
	 *
	 * \param relationId : the ID of the relation to remove.
	 */
	void removeTemporalRelation(unsigned int relationId);

	/*!
	 * Gets the Id of the first box linked with the given relation id.
	 *
	 * \param relationId : the id of the relation.
	 *
	 * \return the id of the first box.
	 */
	unsigned int getRelationFirstBoxId(unsigned int relationId);

	/*!
	 * Gets the index of the control point linked with the given relation id in the first box.
	 *
	 * \param relationId : the id of the relation.
	 *
	 * \return the index of the control point.
	 */
	unsigned int getRelationFirstCtrlPointIndex(unsigned int relationId);

	/*!
	 * Gets the Id of the second box linked with the given relation id.
	 *
	 * \param relationId : the id of the relation.
	 *
	 * \return the id of the second box.
	 */
	unsigned int getRelationSecondBoxId(unsigned int relationId);

	/*!
	 * Gets the index of the control point linked with the given relation id in the second box.
	 *
	 * \param relationId : the id of the relation.
	 *
	 * \return the index of the control point.
	 */
	unsigned int getRelationSecondCtrlPointIndex(unsigned int relationId);

	int getRelationMinBound(unsigned int relationId);
	int getRelationMaxBound(unsigned int relationId);

	/*!
	 * Calculates the optimal system after an editing operation
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box edited
	 * \param x : new begin value of the box
	 * \param y : new end value of the box
	 * \param maxSceneWidth : the max scene width
	 * \param movedBoxes : empty vector, will be filled with the ID of the boxes moved by this resolution
	 * \param maxModification : (optional) if provided, the system will refuse modification (in length or date) greater than this value.
	 *
	 * \return true if the move is allowed or false if the move is forbidden
	 */
	bool performBoxEditing(unsigned int boxId, int x, int y, std::vector<unsigned int>& movedBoxes, unsigned int maxModification = NO_MAX_MODIFICATION);

	/*!
	 * Gets the begin value of the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the begin value of the box matching the given ID
	 */
	int getBoxBeginTime(unsigned int boxId);

	/*!
	 * Gets the end value of the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the end value of the box matching the given ID
	 */
	int getBoxEndTime(unsigned int boxId);

	/*!
	 * Gets the end duration of the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the duration value of the box matching the given ID
	 */
	int getBoxDuration(unsigned int boxId);

	/*!
	 * Gets the number of controlPoints in the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the number of controlPoints in the box matching the given ID
	 */
	int getBoxNbCtrlPoints(unsigned int boxId);

	/*!
	 * Gets the index of the first control point (the left one).
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param id : the ID of the box
	 *
	 * \return the index of the first control point (the left one)
	 */
	int getBoxFirstCtrlPointIndex(unsigned int boxId);

	/*!
	 * Gets the index of the first control point (the right one).
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the index of the first control point (the right one)
	 */
	int getBoxLastCtrlPointIndex(unsigned int boxId);

	/*!
	 * Sets the message to send (via Network) when the given controlPoint is reached.
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box containing this controlPoint.
	 * \param controlPointIndex : the index of the point to set message.
	 * \param messageToSend : vector of message to send.
	 * \param muteState : (optional) true if the messages must be stored but not sent.
	 */
	void setCtrlPointMessagesToSend(unsigned int boxId, unsigned int controlPointIndex, std::vector<std::string> messageToSend, bool muteState = false);

	/*!
	 * Gets the message to send (via Network) when the given controlPoint is reached, in a vector given as parameter.
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box containing this controlPoint.
	 * \param controlPointIndex : the index of the control point to set message.
	 * \param messages : vector to fill with all messages that will be sent (the result)
	 */
	void getCtrlPointMessagesToSend(unsigned int boxId, unsigned int controlPointIndex, std::vector<std::string>& messages);

	/*!
	 * Removes the message to send (via Network) when the given controlPoint is reached.
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param id : the ID of the box
	 * \param controlPointIndex : the index of the point to remove message
	 */
	void removeCtrlPointMessagesToSend(unsigned int boxId, unsigned int controlPointIndex);

	/*!
	 * Sets the control point mute state. If muted, the messages related to this control point will not be send.
	 *
	 * \param boxId : the ID of the box.
	 * \param controlPointIndex : the index of the control point.
	 * \param mute : the mute state to set (true or false).
	 */
	void setCtrlPointMutingState(unsigned int boxId, unsigned int controlPointIndex, bool mute);

	/*!
	 * Gets the control point mute state.
	 *
	 * \param boxId : the ID of the box.
	 * \param controlPointIndex : the index of the control point.
	 *
	 * \return the mute state (true or false).
	 */
	bool getCtrlPointMutingState(unsigned int boxId, unsigned int controlPointIndex);

	/*!
	 * Sets the box content mute state. If muted, the process inside will not be launched.
	 *
	 * \param boxId : the ID of the box.
	 * \param mute : the mute state to set (true or false).
	 */
	void setProcessMutingState(unsigned int boxId, bool mute);

	/*!
	 * Gets the box content mute state.
	 *
	 * \param boxId : the ID of the box.
	 *
	 * \return the mute state (true or false).
	 */
	bool getProcessMutingState(unsigned int boxId);

	/*!
	 * Sets all control points and content the same mute state.
	 *
	 * \param boxId : the ID of the box.
	 * \param mute : the mute state to set (true or false).
	 */
	void setBoxMutingState(unsigned int boxId, bool mute);




	//CURVES ////////////////////////////////////////////////////////////////////////////////////

	/*!
	 * Adds an address in a box to create a curve.
	 * When this address exists in box start messages and box end messages, a curve for this
	 * address will be sent. By default, the curve is a linear interpolation.
	 *
	 * For example, if the box (id 1) start messages contains "/deviceName/volume 0" and the box end messages
	 * contains "/deviceName/volume 100", curvesAddCurves(1, "/deviceName/volume") will create a curve
	 * which will be sent during the box execution. If curvesSetSections() is not called, this curves
	 * will be a linear interpolation between 0 and 100: /volume 0, /volume 1, ..., /volume 100 will be
	 * sent to the device "deviceName".
	 *
	 * If this address does not exist in box start messages or box end messages, the curve will not be
	 * sent, but the information will still remain in the box. When both start and end messages
	 * will have this address again, a curve will be generate automatically.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : the curve address.
	 */
	void addCurve(unsigned int boxId, const std::string & address);

	/*!
	 * Removes the address from the box, and so the matching curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : the curve address.
	 */
	void removeCurve(unsigned int boxId, const std::string & address);

	/*!
	 * Removes all curve address from the box.
	 *
	 * \param boxId : the Id of the box.
	 */
	void clearCurves(unsigned int boxId);

	/*!
	 * Gets all the curves address stored in a box.
	 *
	 * \param boxId : the Id of the box.
	 *
	 * \return all curves addresses in a vector.
	 */
	std::vector<std::string> getCurvesAddress(unsigned int boxId);

	/*!
	 * Changes the sample rate of a curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 * \param nbSamplesBySec : new sample rate.
	 */
	void setCurveSampleRate(unsigned int boxId, const std::string & address, unsigned int nbSamplesBySec);

	/*!
	 * Gets the sample rate of a curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 *
	 * \return the sample rate (0 if this address is not present as a curve).
	 */
	unsigned int getCurveSampleRate(unsigned int boxId, const std::string & address);

	/*!
	 * Changes the avoid redundancy information of a curve.
	 * If set to true, the sample rate will not be respect if two consecutive values are identical. Only the first
	 * will be sent.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 * \param avoidRedundancy : new redundancy information.
	 */
	void setCurveRedundancy(unsigned int boxId, const std::string & address, bool redundancy);

	/*!
	 * Gets the avoid redundancy information of a curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 *
	 * \return the avoid redundancy information.
	 */
	bool getCurveRedundancy(unsigned int boxId, const std::string & address);

	void setCurveMuteState(unsigned int boxId, const std::string & address, bool muteState);
	bool getCurveMuteState(unsigned int boxId, const std::string & address);


	/*!
	 * Gets all arguments type of a given string. Useful too for getting the address information of a message.
	 *
	 * For example : "/deviceName/freq 100 200." will be cut into :
	 *
	 * </deviceName/freq INT FLOAT>
	 *
	 * \param stringToParse : the string to parse.
	 * \param result : the result vector.
	 */
	void getCurveArgTypes(std::string stringToParse, std::vector<std::string>& result);

	/*!
	 * Sets sections informations for sending more complicated curves.
	 *
	 * A curve start and end values are determined by the matching address information
	 * in the box start and end messages.
	 *
	 * Between, it is possible to add sections, with a percent values for the time,
	 * y values, section type and coeff.
	 *
	 * The argNb is the index of the arg to modify the curve.
	 * (for example start message: "/deviceName/freq 100 200." and end message "/deviceName/freq 250 0."
	 * => arg 0 is for the curves starting by 100 and finishing by 250,
	 * arg 1 is for the curves starting by 200. and finishing by 0.).
	 *
	 * xPercents and yValues vectors must have the same size.
	 *
	 * sectionType and coeff vectors must have the same size.
	 *
	 * and sectionType and coeff vectors must have a size equal to (XPercent vector size + 1).
	 *
	 * For example, with a start message "/deviceName/volume 0" and a end message "/deviceName/volume 0" (in box 1)
	 *
	 * curvesSetSections(1, "/deviceName/volume", 0, <50>, <100>, <CURVES_POW CURVES_POW>, <1 1>).
	 *
	 * This is what will be sent to "deviceName" : /volume 0, /volume 1, ... /volume 99, /volume 100, /volume 99, ..., /volume 1, /volume 0.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 * \param argNb : arg index.
	 * \param xPercents :[0 < % < 100]
	 * \param yValues
	 * \param sectionType : CURVES_POW,...
	 * \param coeff (for CURVES_POW: 0 to 0.9999 => sqrt, 1 => linear, 1 to infinity => pow).
	 *
	 * \return true if the new information about section is correct and correctly set.
	 */
	bool setCurveSections(unsigned int boxId, std::string address, unsigned int argNb,
				const std::vector<float>& xPercents, const std::vector<float>& yValues, const std::vector<short>& sectionType, const std::vector<float>& coeff);

	bool getCurveSections(unsigned int boxId, std::string address, unsigned int argNb,
							    std::vector<float> & percent,  std::vector<float> & y,  std::vector<short> & sectionType,  std::vector<float> & coeff);

	/*!
	 * Gets the curve values at it will be sent, at the previously set sampleRate.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 * \param argNb : the arg index to get the curve value result.
	 * (for example start message: "/deviceName/freq 100 200." and end message "/deviceName/freq 250 0."
	 * => arg 0 is for the curves starting by 100 and finishing by 250,
	 * arg 1 is for the curves starting by 200. and finishing by 0.).
	 * \param result : the result vector.
	 *
	 * \return false if the curve could not be compute, or if the argument is a string.
	 */
	bool getCurveValues(unsigned int boxId, const std::string & address, unsigned int argNb, std::vector<float>& result);



	/*!
	 * Adds a new triggerPoint in CSP.
	 *
	 * \return the created trigger ID
	 */
	unsigned int addTriggerPoint(unsigned int containingBoxId);

	/*!
	 * Removes the triggerPoint from the CSP.
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger to be removed.
	 */
	void removeTriggerPoint(unsigned int triggerId);

	/*!
	 * Sets the controlPoint to link with triggerPoint (given by ID).
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger to be linked with the controlPoint.
	 * \param boxId : the ID of the box containing the controlPoint to link.
	 * \param controlPointIndex : the controlPoint index in the box.
	 *
	 * \return false if the controlPoint is already linked to another triggerPoint.
	 */
	bool assignCtrlPointToTriggerPoint(unsigned int triggerId, unsigned int boxId, unsigned int controlPointIndex);

	/*!
	 * Removes the controlPoint linked with the triggerPoint (given by ID).
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger to be unlinked with its controlPoint.
	 */
	void freeTriggerPointFromCtrlPoint(unsigned int triggerId);

	/*!
	 * Sets the triggerPoint (given by ID) message.
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger.
	 * \param triggerMessage : the trigger message
	 */
	void setTriggerPointMessage(unsigned int triggerId, std::string triggerMessage);

	/*!
	 * Gets the triggerPoint (given by ID) message.
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger.
	 *
	 * \return the trigger message
	 */
	std::string getTriggerPointMessage(unsigned int triggerId);

	/*!
	 * Gets the id of the box linked to the given trigger point.
	 *
	 * \param triggerId : the trigger Id.
	 *
	 * \return the id of the box linked to the trigger point,
	 * NO_ID if the trigger point is not linked to a box.
	 */
	unsigned int getTriggerPointRelatedBoxId(unsigned int triggerId);

	/*!
	 * Gets the index of the control point linked to the given trigger point.
	 *
	 * \param triggerId : the trigger Id.
	 *
	 * \return the index of the control point linked to the trigger point,
	 * NO_ID if the trigger point is not linked to a control point.
	 */
	unsigned int getTriggerPointRelatedCtrlPointIndex(unsigned int triggerId);

	/*!
	 * Sets the triggerPoint mute state.
	 *
	 * \param triggerId : the ID of the trigger to set the mute state.
	 * \param muteState : mute state ; true if the trigger must be mutable, false if not.
	 */
	void setTriggerPointMutingState(unsigned int triggerId, bool muteState);

	/*!
	 * Gets the triggerPoint mute state.
	 *
	 * \param triggerId : the ID of the trigger to test the mute state.
	 *
	 * \return true if the trigger is muted.
	 */
	bool getTriggerPointMutingState(unsigned int triggerId);

	/*!
	 * Fills the given vector with all the boxes ID used in the editor.
	 * Useful after a load.
	 *
	 * \param boxesID : the vector to fill with all boxes ID used.
	 */
	void getBoxesId(std::vector<unsigned int>& boxesID);

	/*!
	 * Fills the given vector with all the relations ID used in the editor.
	 * Useful after a load.
	 *
	 * \param relationsId : the vector to fill with all relations ID used.
	 */
	void getRelationsId(std::vector<unsigned int>& relationsID);

	/*!
	 * Fills the given vector with all the triggers ID used in the editor.
	 * Useful after a load.
	 *
	 * \param triggersID : the vector to fill with all triggers ID used.
	 */
	void getTriggersPointId(std::vector<unsigned int>& triggersID);




	//Execution ///////////////////////////////////////////////////////////////////////

	void setGotoValue(unsigned int gotoValue);
	unsigned int getGotoValue();

	/*!
	 * Plays the scenario (compiles and runs it).
	 *
	 * \param gotoInformation : (optional) time to start the execution.
	 *
	 * \return true if the ECOMachine is now running.
	 */
	bool play();

	/*!
	 * Stops the ECOMachine.
	 *
	 * \return true if the ECOMachine will actually stop.
	 */
	bool stop();

	void pause(bool pauseValue);

	bool isPaused();

	/*!
	 * Tests if the ECOMachine is actually running.
	 *
	 * \return true if the ECOMachine is running. False if not.
	 */
	bool isRunning();

	/*!
	 * Compiles the editor to set the ECOMachine.
	 * Play() must be preferably called except if
	 * the compilation and the run must be separate.
	 *
	 * \param gotoInformation : (optional) time to start the compilation.
	 */
	void compile();

	/*!
	 * Runs the ECOMachine.
	 * Must be called after a call of the compile() function. Play() must be preferably called except if
	 * the compilation and the run must be separate.
	 *
	 * \param gotoInformation : (optional) time to start the run.
	 *
	 * \return true if the ECOMachine is now running.
	 */
	bool run();


	/*!
	 * Gets the ECOMachine current execution time in milliseconds.
	 *
	 * \return the execution time in milliseconds.
	 */
	unsigned int getCurrentExecutionTime();

	/*!
	 * Changes the execution speed.
	 *
	 * \param factor: the new speed factor.
	 */
	void setExecutionSpeedFactor(float factor);

	float getExecutionSpeedFactor();

	/*!
	 * Gets the progression of a process (in percent).
	 *
	 * \param processId: the id of the process.
	 * \return the progression inpercent.
	 */
	float getProcessProgression(unsigned int boxId);

	/*!
	 * Ignores the waited trigger points for one step in the ECOMachine.
	 * The ECOMachine must be in running state.
	 */
	void ignoreTriggerPointOnce();




	//Network //////////////////////////////////////////////////////////////////////////////////////////////




	/*!
	 * Same behavior as receiveNetworkMessage function, but must be used by a user to simulate a network message reception.
	 * For example to trigger a trigger point manually.
	 *
	 * Tells the ECOMachine that a network message was received and need to be handled.
	 *
	 * \param netMessage : the network message received.
	 */
	void simulateNetworkMessageReception(const std::string & netMessage);

	/*!
	 * Fills the given vectors with all the loaded network plugins and the matching listening ports.
	 *
	 * \param pluginsName : will be filled with all loaded plugins names.
	 * \param listeningPort : will be filled with matching listening port.
	 */
	void getLoadedNetworkPlugins(std::vector<std::string>& pluginsName, std::vector<unsigned int>& listeningPort);

	/*!
	 * Adds a network device.
	 *
	 * \param deviceName : the name to give to this device.
	 * \param pluginToUse : plugin to use with this device (the plugin name could be retrieved with
	 * the networkGetAllLoadedPlugins function).
	 * \param deviceIp : the ip of the network device.
	 * \param devicePort : the port of the network device.
	 */
	void addNetworkDevice(const std::string & deviceName, const std::string & pluginToUse, const std::string & deviceIp, const std::string & devicePort);

	/*!
	 * Removes a network device.
	 *
	 * \param deviceName : device name to remove from the network controller.
	 */
	void removeNetworkDevice(const std::string & deviceName);

	/*!
	 * Sends a network message.
	 * The message must look like this :
	 * /deviceName/address1/address2/... param1 param2...
	 *
	 * The message "/address1/address2/... param1 param2..." will be sent using the device deviceName (with matching port, ip and plugin).
	 *
	 * \param stringToSend : the string to send using network.
	 */
	void sendNetworkMessage(const std::string & stringToSend);

	/*!
	 * Fills the given vectors with all network devices name, and matching information about if they could send namespace request.
	 *
	 * \param devicesName : will be filled with all network devices names.
	 * \param couldSendNamespaceRequest : will be filled with information about if the matching device could
	 * send namespace request.
	 */
	void getNetworkDevicesName(std::vector<std::string>& devicesName, std::vector<bool>& couldSendNamespaceRequest);

	/*!
	 * Sends a network snapshot request.
	 * The address must look like this :
	 * /deviceName/address1/address2/
	 *
	 * The snapshot request will be on  "/address1/address2/" using the device deviceName (with matching port, ip and plugin).
	 *
	 * \param address : information to where getting the snapshot information.
	 *
	 * \return the snapshot as a string vector.
	 */
	std::vector<std::string> requestNetworkSnapShot(const std::string & address);

	/*!
	 * Sends a network namespace request.
	 * The address must look like this :
	 * /deviceName/address1/address2/
	 *
	 * The namespace request will be on  "/address1/address2/" using the device deviceName (with matching port, ip and plugin).
	 *
	 * The namespace information is given by filling matching vectors.
	 *
	 * \param address : information to where getting the namespace information.
	 * \param nodes : will be filled with nodes information.
	 * \param leaves : will be filled with leaves information.
	 * \param attributs : will be filled with attributs information.
	 * \param attributsValue : will be filled with the matching attributs values.
	 *
	 * \return the result of the namespace request. TIMEOUT_EXCEEDED (time out exceed), ANSWER_RECEIVED (answer received).
	 */
	int requestNetworkNamespace(const std::string & address,
						 std::vector<std::string>& nodes, std::vector<std::string>& leaves,
						 std::vector<std::string>& attributs, std::vector<std::string>& attributsValue);







	//Store and load ////////////////////////////////////////////////////////////////////////////////////

	/*!
	 * Stores both engines.
	 *
	 * \param fileName : the fileName to store the engines.
	 */
	void store(std::string fileName);

	/*!
	 * Loads both engines.
	 *
	 * \param fileName : the fileName to load.
	 */
	void load(std::string fileName);

	/*!
	 * Loads both engines.
	 *
	 * \param fileName : the fileName to load.
	 * \param crossAction : callback, called during the execution when a control point is crossed.
	 * This callback gives information about the box id, and the control point index.
	 * \param triggerAction : callback, called during the execution, when a trigger point has a new state.
	 * This callback gives information about its waiting state, the trigger id, the box id, the control point index
	 * and the waited network message.
	 */
	void load(std::string fileName,
			void(*crossAction)(unsigned int, unsigned int, std::vector<unsigned int>),
			void(*triggerAction)(bool, unsigned int, unsigned int, unsigned int, std::string));

	void storeUsingAntoineFormat(std::string fileName);

	void loadUsingAntoineFormat(std::string fileName);

	void loadUsingAntoineFormat(std::string fileName,
								void(*crossAction)(unsigned int, unsigned int, std::vector<unsigned int>),
								void(*triggerAction)(bool, unsigned int, unsigned int, unsigned int, std::string));

	/*!
	 * Prints on standard output both engines. Useful only for debug purpose.
	 */
	void print();




	friend void receiveNetworkMessageCallBack(void*, std::string, std::string, std::string&);
	friend void triggerPointStateCallBack(void*, bool, unsigned int, unsigned int, unsigned int, std::string);
	friend void  executionFinishedCallBack(void* arg);

	/*!
	 * Gets the execution machine. Must not be called outside the library. This function is public only for debug purpose.
	 *
	 * \return : the execution machine.
	 */
	ECOMachine* getExecutionMachine();

	/*!
	 * Gets the editor. Must not be called outside the library. This function is public only for debug purpose.
	 *
	 * \return : the editor.
	 */
	Editor* getEditor();

	void printExecutionInLinuxConsole();

private:
	EnginesPrivate* m_implementation;

	void initializeObjects(unsigned int maxSceneWidth, std::string plugginsLocation);

	/*!
	 * Tells the ECOMachine that a network message was received and need to be handled.
	 *
	 * \param netMessage : the network message received.
	 */
	bool receiveNetworkMessage(std::string netMessage);

};

#endif /*ENGINES_HPP_*/
