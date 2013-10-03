/*
 * Temporary class to get i-score working with Score and Modular
 * Copyright © 2013, Théo de la Hogue
 *
 * License: This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef __SCORE_ENGINE_H__
#define __SCORE_ENGINE_H__

#include "TTScoreAPI.h"
#include "TTModular.h"

/*!
 * \file Engine.h
 * \author Théo de la Hogue, based on Engine.hpp written by Raphael Marczak (LaBRI) for the libIscore.
 * \date 2013
 *
 * \brief This file contains the only class a user have to instantiate and manipulate for all edition and execution manipulation.
 *
 */

#include <string>
#include <map>
#include <vector>

#include <QColor>
#include <QPointF>

/** a temporary type dedicated to pass time value (date, duration, ...) */
typedef unsigned int TimeValue;

/** a temporary type dedicated to pass bound value */
typedef int BoundValue;

/** a temporary type dedicated to retreive a TimeProcess (this is related to Box notion) */
typedef unsigned int TimeProcessId;

/** a temporary type dedicated to retreive a TimeEvent attached to a TimeProcess (this is related to ControlPoint notion) */
typedef unsigned int TimeEventIndex;

#define BEGIN_CONTROL_POINT_INDEX 1     // TODO : replace by StartEventIndex
#define END_CONTROL_POINT_INDEX 2       // TODO : replace by EndEventIndex

/** a temporary type dedicated to retreive an Interval TimeProcess (this is related to Relation notion) */
typedef unsigned int IntervalId;

/** a temporary type dedicated to retreive a process with an Conditioned TimeEvent inside (this is related to TriggerPoint notion) */
typedef unsigned int ConditionedProcessId;

/** a class used to cache TTObject and some observers */
class EngineCacheElement {

public:
    TTObjectBasePtr object;
    unsigned int    index;
    std::string     name;
    
    EngineCacheElement();
    ~EngineCacheElement();
    
};
typedef EngineCacheElement* EngineCacheElementPtr;

/** a temporary type that contains an id and a cached element */
typedef std::pair<unsigned int, EngineCacheElementPtr> EngineCacheKey;
typedef	EngineCacheKey*	EngineCacheKeyPtr;

/** a temporary type to define a map to store and retreive a cached element relative to an id */
typedef std::map<unsigned int, EngineCacheElementPtr> EngineCacheMap;
typedef	EngineCacheMap*	EngineCacheMapPtr;

typedef std::map<unsigned int, EngineCacheElementPtr>::iterator EngineCacheMapIterator;

#define NO_BOUND -1

#define NO_ID 0
#define ROOT_BOX_ID 1

#define NO_MAX_MODIFICATION 0

#define CURVE_POW 1

#define SCENARIO_SIZE 600000

/// Allen relations types
enum AllenType { ALLEN_BEFORE, ALLEN_AFTER, ALLEN_EQUALS, ALLEN_MEETS, ALLEN_OVERLAPS, ALLEN_DURING, ALLEN_STARTS, ALLEN_FINISHES, ALLEN_NONE };

enum TemporalRelationType { ANTPOST_ANTERIORITY, ANTPOST_POSTERIORITY };

enum TriggerType { TRIGGER_DEFAULT, TRIGGER_START_TEMPO_CHANGE, TRIGGER_END_TEMPO_CHANGE};


const std::string ALLEN_BEFORE_STR = "before";
const std::string ALLEN_AFTER_STR = "after";
const std::string ALLEN_EQUALS_STR = "equals";
const std::string ALLEN_MEETS_STR = "meets";
const std::string ALLEN_OVERLAPS_STR = "overlaps";
const std::string ALLEN_DURING_STR = "during";
const std::string ALLEN_STARTS_STR = "starts";
const std::string ALLEN_FINISHES_STR = "finishes";

const std::string ANTPOST_ANTERIORITY_STR = "anteriority";
const std::string ANTPOST_POSTERIORITY_STR = "posteriority";

const TimeProcessId BOX_NOT_CREATED = NO_ID;
const IntervalId RELATION_NOT_CREATED = NO_ID;
const TimeProcessId NO_MOTHER = NO_ID;

/// Different types of variables linked to the boxes
enum VariableType { BEGIN_VAR_TYPE = 1, LENGTH_VAR_TYPE = 100, INTERVAL_VAR_TYPE = 50, VPOS_VAR_TYPE = 1, VSIZE_VAR_TYPE = 100 };

/// The available binary relations types
enum BinaryRelationType { EQ_RELATION = 0, NQ_RELATION = 1, LQ_RELATION = 2, LE_RELATION = 3, GQ_RELATION = 4, GR_RELATION = 5 };

/// The three temporal relations in Boxes
enum RelationType {ALLEN = 0, ANTPOST = 1, INTERVAL = 2, BOUNDING = 3};

/// define part dedicated for debugging
#define iscoreEngineDebug if (getLocalApplicationDebug)

/*!
 * \class Engine
 *
 * \author Théo de la Hogue, based on Engine class written by Raphael Marczak (LaBRI) for the libIscore.
 * \date 2012-2013
 *
 * \brief Main class which contains all functions for edition and execution.
 *
 * This class is the only class the user have to instance and manipulate.
 * It contains all functions for edition and execution.
 */
class Engine
{
    
private:
    
    TTTimeProcessPtr    m_mainScenario;                                 /// The top scenario
    
    unsigned int        m_nextTimeProcessId;                            /// the next Id to give to any created time process
    unsigned int        m_nextIntervalId;                               /// the next Id to give to any created interval
    unsigned int        m_nextConditionedProcessId;                     /// the next Id to give to any created time process
    
    EngineCacheMap      m_timeProcessMap;                               /// All automation or scenario time process and some observers stored using an unique id
    EngineCacheMap      m_intervalMap;                                  /// All interval time process and some observers stored using an unique id
    EngineCacheMap      m_timeConditionMap;                             /// All condition stored using an unique id
    EngineCacheMap      m_conditionedProcessMap;                        /// All conditioned time process with an conditioned event stored using an unique id
    
    EngineCacheMap      m_runningCallbackMap;                           /// All callback to observe time process running state stored using a time process id
    
    EngineCacheMap      m_readyCallbackMap;                             /// All callback to observe time event ready state stored using using a trigger id
    EngineCacheMap      m_triggerDataMap;                               /// All TTData to expose conditioned event on the network stored using using a trigger id

    TTObjectBasePtr     m_dataPlay;                                     /// A Modular TTData to expose Play transport service
    TTObjectBasePtr     m_dataStop;                                     /// A Modular TTData to expose Stop transport service
    TTObjectBasePtr     m_dataPause;                                    /// A Modular TTData to expose Pause transport service
    TTObjectBasePtr     m_dataRewind;                                   /// A Modular TTData to expose Rewind transport service
    TTObjectBasePtr     m_dataStartPoint;                               /// A Modular TTData to expose StartPoint transport service
    TTObjectBasePtr     m_dataSpeed;                                    /// A Modular TTData to expose Speed transport service
    
	void (*m_TimeEventReadyAttributeCallback)(ConditionedProcessId, bool);
    void (*m_TimeProcessSchedulerRunningAttributeCallback)(TimeProcessId, bool);
    void (*m_TransportDataValueCallback)(TTSymbol&, const TTValue&);

public:

    Engine(void(*timeEventReadyAttributeCallback)(ConditionedProcessId, bool),
           void(*timeProcessSchedulerRunningAttributeCallback)(TimeProcessId, bool),
           void(*transportDataValueCallback)(TTSymbol&, const TTValue&));
    
    void initModular();
    void initScore();
    
    void dumpAddressBelow(TTNodePtr aNode);
    
    ~Engine();
    
    // Id management //////////////////////////////////////////////////////////////////
    
    TimeProcessId       cacheTimeProcess(TTTimeProcessPtr timeProcess, const std::string & name);
    TTTimeProcessPtr    getTimeProcess(TimeProcessId boxId);
    void                uncacheTimeProcess(TimeProcessId boxId);
    void                clearTimeProcess();
    
    IntervalId          cacheInterval(TTTimeProcessPtr timeProcess);
    TTTimeProcessPtr    getInterval(IntervalId relationId);
    void                uncacheInterval(IntervalId relationId);
    void                clearInterval();
    
    ConditionedProcessId cacheConditionedProcess(TimeProcessId timeProcessId, TimeEventIndex controlPointId);
    TTTimeProcessPtr    getConditionedProcess(ConditionedProcessId triggerId, TimeEventIndex& controlPointId);
    void                uncacheConditionedProcess(ConditionedProcessId triggerId);
    void                clearConditionedProcess();
    
    void                cacheTimeCondition(ConditionedProcessId triggerId, TTTimeConditionPtr timeCondition);
    TTTimeConditionPtr  getTimeCondition(ConditionedProcessId triggerId);
    void                uncacheTimeCondition(ConditionedProcessId triggerId);
    void                clearTimeCondition();
    
    void                cacheRunningCallback(TimeProcessId boxId);
    void                uncacheRunningCallback(TimeProcessId boxId);
    
    void                cacheReadyCallback(ConditionedProcessId triggerId, TimeEventIndex controlPointId);
    void                uncacheReadyCallback(ConditionedProcessId triggerId, TimeEventIndex controlPointId);
    
    void                cacheTriggerDataCallback(ConditionedProcessId triggerId, TimeProcessId boxId);
    void                uncacheTriggerDataCallback(ConditionedProcessId triggerId);
        
	// Edition ////////////////////////////////////////////////////////////////////////
    
	/*!
	 * Adds a new box in the CSP.
	 *
	 * \param boxBeginPos : the begin value in ms.
	 * \param boxLength : the length value in ms.
     * \param name : the name of the box
	 * \param motherId : mother box ID if any, NO_ID if the box to create has no mother.
	 *
	 * \return the newly created box ID.
	 */
	TimeProcessId addBox(TimeValue boxBeginPos, TimeValue boxLength, const std::string & name, TimeProcessId motherId);
    
	/*!
	 * Removes a box from the CSP : removes the relation implicating it and the
	 * box's variables.
	 *
	 * \param boxId : the ID of the box to remove.
	 */
	void removeBox(TimeProcessId boxId);
    
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
	IntervalId addTemporalRelation(TimeProcessId boxId1, TimeEventIndex controlPoint1,
                                     TimeProcessId boxId2, TimeEventIndex controlPoint2, TemporalRelationType type,
                                     std::vector<TimeProcessId>& movedBoxes);
    
    /*!
	 * Removes the temporal relation using given id.
	 *
	 * \param relationId : the ID of the relation to remove.
	 */
	void removeTemporalRelation(IntervalId relationId);
    
	/*!
	 * Changes min bound and max bound for the relation length.
	 *
	 * \param relationId : the relation to change the bounds.
	 * \param minBound : the min bound for the box relation in ms.
	 * \param maxBound : the max bound for the box relation in ms. NO_BOUND if the max bound is not used (+infinity).
	 * \param movedBoxes : empty vector, will be filled with the ID of the boxes moved by this new relation.
	 */
	void changeTemporalRelationBounds(IntervalId relationId, BoundValue minBound, BoundValue maxBound, std::vector<TimeProcessId>& movedBoxes);
    
	/*!
	 * Checks if a relation exists between the two given control points.
	 *
	 * \param boxId1 : the id of the first box.
	 * \param controlPoint1 : the index of the control point in the first box.
	 * \param boxId2 : the id of the second box.
	 * \param controlPoint2 : the index of the control point in the second box.
	 *
	 * \return true if a relation exists between the two given control points.
	 */
	bool isTemporalRelationExisting(TimeProcessId boxId1, TimeEventIndex controlPoint1, TimeProcessId boxId2, TimeEventIndex controlPoint2);
    
	/*!
	 * Gets the Id of the first box linked with the given relation id.
	 *
	 * \param relationId : the id of the relation.
	 *
	 * \return the id of the first box.
	 */
	TimeProcessId getRelationFirstBoxId(IntervalId relationId);
    
	/*!
	 * Gets the index of the control point linked with the given relation id in the first box.
	 *
	 * \param relationId : the id of the relation.
	 *
	 * \return the index of the control point.
	 */
	TimeEventIndex getRelationFirstCtrlPointIndex(IntervalId relationId);
    
	/*!
	 * Gets the Id of the second box linked with the given relation id.
	 *
	 * \param relationId : the id of the relation.
	 *
	 * \return the id of the second box.
	 */
	TimeProcessId getRelationSecondBoxId(IntervalId relationId);
    
	/*!
	 * Gets the index of the control point linked with the given relation id in the second box.
	 *
	 * \param relationId : the id of the relation.
	 *
	 * \return the index of the control point.
	 */
	TimeEventIndex getRelationSecondCtrlPointIndex(IntervalId relationId);
    
	BoundValue getRelationMinBound(IntervalId relationId);
	BoundValue getRelationMaxBound(IntervalId relationId);
    
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
	 *
	 * \return true if the move is allowed or false if the move is forbidden
	 */
	bool performBoxEditing(TimeProcessId boxId, TimeValue start, TimeValue end, std::vector<TimeProcessId>& movedBoxes);
    
    /*!
	 * Gets the name of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the name of the box matching the given ID
	 */
    std::string getBoxName(TimeProcessId boxId);
    
    /*!
	 * Gets the vertical position of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the vertical position of the box matching the given ID
	 */
    unsigned int getBoxVerticalPosition(TimeProcessId boxId);
    
    /*!
	 * Gets the vertical position of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \param newPosition : the new vertical position of the box matching the given ID
	 */
    void setBoxVerticalPosition(TimeProcessId boxId, unsigned int newPosition);
    
    /*!
	 * Gets the vertical size of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the vertical size of the box matching the given ID
	 */
    unsigned int getBoxVerticalSize(TimeProcessId boxId);

    /*!
	 * Sets the vertical size of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \param newSize : the new vertical size of the box matching the given ID
	 */
    void setBoxVerticalSize(TimeProcessId boxId, unsigned int newSize);
    
    /*!
	 * Gets the color of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the color of the box matching the given ID
	 */
    QColor getBoxColor(TimeProcessId boxId);
    
    /*!
	 * Sets the color of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \param newColor : the color of the box matching the given ID
	 */
    void setBoxColor(TimeProcessId boxId, QColor newColor);
    
    
	/*!
	 * Gets the begin value of the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the begin value of the box matching the given ID
	 */
	TimeValue getBoxBeginTime(TimeProcessId boxId);
    
	/*!
	 * Gets the end value of the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the end value of the box matching the given ID
	 */
	TimeValue getBoxEndTime(TimeProcessId boxId);
    
	/*!
	 * Gets the end duration of the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the duration value of the box matching the given ID
	 */
	TimeValue getBoxDuration(TimeProcessId boxId);
    
	/*!
	 * Gets the number of controlPoints in the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the number of controlPoints in the box matching the given ID
	 */
	int getBoxNbCtrlPoints(TimeProcessId boxId);
    
	/*!
	 * Gets the index of the first control point (the left one).
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param id : the ID of the box
	 *
	 * \return the index of the first control point (the left one)
	 */
	TimeEventIndex getBoxFirstCtrlPointIndex(TimeProcessId boxId);
    
	/*!
	 * Gets the index of the first control point (the right one).
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the index of the first control point (the right one)
	 */
	TimeEventIndex getBoxLastCtrlPointIndex(TimeProcessId boxId);
    
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
	void setCtrlPointMessagesToSend(TimeProcessId boxId, TimeEventIndex controlPointIndex, std::vector<std::string> messageToSend, bool muteState = false);
    
	/*!
	 * Gets the message to send (via Network) when the given controlPoint is reached, in a vector given as parameter.
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box containing this controlPoint.
	 * \param controlPointIndex : the index of the control point to set message.
	 * \param messages : vector to fill with all messages that will be sent (the result)
	 */
	void getCtrlPointMessagesToSend(TimeProcessId boxId, TimeEventIndex controlPointIndex, std::vector<std::string>& messages);
    
	/*!
	 * Sets the control point mute state. If muted, the messages related to this control point will not be send.
	 *
	 * \param boxId : the ID of the box.
	 * \param controlPointIndex : the index of the control point.
	 * \param mute : the mute state to set (true or false).
	 */
	void setCtrlPointMutingState(TimeProcessId boxId, TimeEventIndex controlPointIndex, bool mute);
    
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
	void addCurve(TimeProcessId boxId, const std::string & address);
    
	/*!
	 * Removes the address from the box, and so the matching curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : the curve address.
	 */
	void removeCurve(TimeProcessId boxId, const std::string & address);
    
	/*!
	 * Removes all curve address from the box.
	 *
	 * \param boxId : the Id of the box.
	 */
	void clearCurves(TimeProcessId boxId);
    
	/*!
	 * Gets all the curves address stored in a box.
	 *
	 * \param boxId : the Id of the box.
	 *
	 * \return all curves addresses in a vector.
	 */
	std::vector<std::string> getCurvesAddress(TimeProcessId boxId);
    
	/*!
	 * Changes the sample rate of a curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 * \param nbSamplesBySec : new sample rate.
	 */
	void setCurveSampleRate(TimeProcessId boxId, const std::string & address, unsigned int nbSamplesBySec);
    
	/*!
	 * Gets the sample rate of a curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 *
	 * \return the sample rate (0 if this address is not present as a curve).
	 */
	unsigned int getCurveSampleRate(TimeProcessId boxId, const std::string & address);
    
	/*!
	 * Changes the avoid redundancy information of a curve.
	 * If set to true, the sample rate will not be respect if two consecutive values are identical. Only the first
	 * will be sent.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 * \param avoidRedundancy : new redundancy information.
	 */
	void setCurveRedundancy(TimeProcessId boxId, const std::string & address, bool redundancy);
    
	/*!
	 * Gets the avoid redundancy information of a curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 *
	 * \return the avoid redundancy information.
	 */
	bool getCurveRedundancy(TimeProcessId boxId, const std::string & address);
    
	void setCurveMuteState(TimeProcessId boxId, const std::string & address, bool muteState);
	bool getCurveMuteState(TimeProcessId boxId, const std::string & address);
    
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
	bool setCurveSections(TimeProcessId boxId, std::string address, unsigned int argNb,
                          const std::vector<float>& xPercents, const std::vector<float>& yValues, const std::vector<short>& sectionType, const std::vector<float>& coeff);
    
	bool getCurveSections(TimeProcessId boxId, std::string address, unsigned int argNb,
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
	bool getCurveValues(TimeProcessId boxId, const std::string & address, unsigned int argNb, std::vector<float>& result);
    
	/*!
	 * Adds a new triggerPoint in CSP.
	 *
	 * \return the created trigger ID
	 */
	ConditionedProcessId addTriggerPoint(TimeProcessId containingBoxId, TimeEventIndex controlPointIndex);
    
	/*!
	 * Removes the triggerPoint from the CSP.
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger to be removed.
	 */
	void removeTriggerPoint(ConditionedProcessId triggerId);
    
	/*!
	 * Sets the triggerPoint (given by ID) message.
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger.
	 * \param triggerMessage : the trigger message
	 */
	void setTriggerPointMessage(ConditionedProcessId triggerId, std::string triggerMessage);
    
	/*!
	 * Gets the triggerPoint (given by ID) message.
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger.
	 *
	 * \return the trigger message
	 */
	std::string getTriggerPointMessage(ConditionedProcessId triggerId);
    
	/*!
	 * Gets the id of the box linked to the given trigger point.
	 *
	 * \param triggerId : the trigger Id.
	 *
	 * \return the id of the box linked to the trigger point,
	 * NO_ID if the trigger point is not linked to a box.
	 */
	TimeProcessId getTriggerPointRelatedBoxId(ConditionedProcessId triggerId);
    
	/*!
	 * Gets the index of the control point linked to the given trigger point.
	 *
	 * \param triggerId : the trigger Id.
	 *
	 * \return the index of the control point linked to the trigger point,
	 * NO_ID if the trigger point is not linked to a control point.
	 */
	TimeProcessId getTriggerPointRelatedCtrlPointIndex(ConditionedProcessId triggerId);
    
	/*!
	 * Fills the given vector with all the boxes ID used in the editor.
	 * Useful after a load.
	 *
	 * \param boxesID : the vector to fill with all boxes ID used.
	 */
	void getBoxesId(std::vector<TimeProcessId>& boxesID);
    
	/*!
	 * Fills the given vector with all the relations ID used in the editor.
	 * Useful after a load.
	 *
	 * \param relationsId : the vector to fill with all relations ID used.
	 */
	void getRelationsId(std::vector<IntervalId>& relationsID);
    
	/*!
	 * Fills the given vector with all the triggers ID used in the editor.
	 * Useful after a load.
	 *
	 * \param triggersID : the vector to fill with all triggers ID used.
	 */
	void getTriggersPointId(std::vector<ConditionedProcessId>& triggersID);
    
    /*!
	 * Set the zoom factor of the view of the main scenario
	 * 
	 * \param zoom : the zoom factor as QPointF variable
	 */
	void setViewZoom(QPointF zoom);
    
    /*!
	 * Get the zoom factor of the view of the main scenario
	 *
	 * \return the zoom factor as a QPointF variable
	 */
	QPointF getViewZoom();
    
    /*!
	 * Set the position of the view of the main scenario
	 *
	 * \param zoom : the zoom factor as QPointF variable
	 */
	void setViewPosition(QPointF position);
    
    /*!
	 * Get the position of the view of the main scenario
	 *
	 * \return the position as a QPointF variable
	 */
	QPointF getViewPosition();
    
    
	//Execution ///////////////////////////////////////////////////////////////////////
    
	void setGotoValue(TimeValue gotoValue);
	TimeValue getGotoValue();
    
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
	 * Gets the ECOMachine current execution time in milliseconds.
	 *
	 * \return the execution time in milliseconds.
	 */
	TimeValue getCurrentExecutionTime();
    
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
	float getProcessProgression(TimeProcessId boxId);
    
    
	//Network //////////////////////////////////////////////////////////////////////////////////////////////
    
    /*!
     * Tells the Engine that a network message was received and need to be handled.
	 *
	 * \param netMessage : the network message received.
	 */
	bool receiveNetworkMessage(std::string netMessage);
    
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
     * \param nodeType : information about the node object type.
	 * \param nodes : will be filled with nodes information.
	 * \param leaves : will be filled with leaves information.
	 * \param attributs : will be filled with attributs information.
	 * \param attributsValue : will be filled with the matching attributs values.
	 *
	 * \return the result of the namespace request. TIMEOUT_EXCEEDED (time out exceed), ANSWER_RECEIVED (answer received).
	 */
	int requestNetworkNamespace(const std::string & address,
                                std::string & nodeType,
                                std::vector<std::string>& nodes, std::vector<std::string>& leaves,
                                std::vector<std::string>& attributs, std::vector<std::string>& attributsValue);

    /*!
     * Sends a request to get an attribute value.
     *
     * \param address : the object's address. ex : /deviceName/address1/address2/
     * \param attribute : the attribute's name we request. ex : "service", "rangeBounds"...etc
     * \param attributeValue : will be filled with the matching attribute values.
     *
     * \return True(1) or false(0) if the request failed or not.
     */
    int requestObjectAttributeValue(const std::string & address, const std::string & attribute, std::vector<std::string>& value);

    /*!
     * Sends a request to get the type of an object.
     *
     * \param address : the object's address. ex : /deviceName/address1/address2/
     * \param nodeType : will be filled with the type.
     *
     * \return True(1) or false(0) if the request failed or not.
     */
    int requestObjectType(const std::string & address, std::string & nodeType);

    /*!
     * Sends a request to get the children nodes of an object.
     *
     * \param address : the object's address. ex : /deviceName/address1/address2/
     * \param children : will be filled with the children' addresses.
     *
     * \return True(1) or false(0) if the request failed or not.
     */
    int requestObjectChildren(const std::string & address, std::vector<std::string>& children);

	//Store and load ////////////////////////////////////////////////////////////////////////////////////
    
	/*!
	 * Store Engine.
	 *
	 * \param fileName : the fileName to store the engines.
	 */
	void store(std::string fileName);
    
	/*!
	 * Load Engine.
	 *
	 * \param fileName : the fileName to load.
	 */
	void load(std::string fileName);
    
	/*!
	 * Prints on standard output both engines. Useful only for debug purpose.
	 */
	void print();
    void printExecutionInLinuxConsole();
    
	friend void TimeEventReadyAttributeCallback(TTPtr baton, const TTValue& value);
    friend void TimeProcessSchedulerRunningAttributeCallback(TTPtr baton, const TTValue& value);
    friend void TransportDataValueCallback(TTPtr baton, const TTValue& value);
    friend void TriggerReceiverValueCallback(TTPtr baton, const TTValue& value);
    
private:
    
    /*!
     * Convert directory/address into directory:/address
     *
     * \param networktreeAddress : an address managed by i-score
     * \return aTTAddress : an address managed by jamoma
     */
    TTAddress toTTAddress(std::string networktreeAddress);
    
    /*!
     * Convert directory:/address into directory/address
     *
     * \param aTTAddress : an address managed by jamoma
     * \return networktreeAddress : an address managed by i-score
     */
    std::string toNetworkTreeAddress(TTAddress aTTAddress);
};

typedef Engine* EnginePtr;

/** Any time event ready attribute callback
 @param	baton			a TTValuePtr containing an EnginePtr and an ConditionedProcessId
 @param	value			the time event ready state
 @return                an error code */
void TimeEventReadyAttributeCallback(TTPtr baton, const TTValue& value);

/** Any time process running state callback
 @param	baton			a TTValuePtr containing an EnginePtr and an TimeProcessId
 @param	value			the time process running state
 @return                an error code */
void TimeProcessSchedulerRunningAttributeCallback(TTPtr baton, const TTValue& value);

/** Any transport data value callback
 @param	baton			a TTValuePtr containing an EnginePtr and a TTDataPtr
 @param	value			the value of the data
 @return                an error code */
void TransportDataValueCallback(TTPtr baton, const TTValue& value);


// TODO : this should move into a TTModularAPI file
/** Create a TTData object
 @param	service			a symbol to tell if the data have to be a "parameter", a "message" or a "return"
 @param	TTValuePtr      a value pointer to return back
 @param valueCallback   a pointer to a void function(TTPtr baton, TTValue& value) to return the value back
 @param returnedData    a new data object
 @return                an error code if the creation fails */
TTErr TTModularCreateData(TTSymbol service, TTValuePtr baton, TTFunctionWithBatonAndValue valueCallback, TTObjectBasePtr *returnedData);

// TODO : this should move into a TTModularAPI file
/** Register a TTObject
 @param	address			the absolute address where to register the object
 @param	object          the object to register
 @return                an error code if the registration fails */
TTErr TTModularRegisterObject(TTAddress address, TTObjectBasePtr object);

// TODO : this should move into a TTModularAPI file
/** compare priority attribute of object's node
 @param	v1				a pointer to a value containing a pointer to a TTNode >
 @param	v2				a pointer to a value containing a pointer to a TTNode >
 @return				is the priority of v1 is smaller than v2 (except if equal 0) ? */
TTBoolean TTModularCompareNodePriority(TTValue& v1, TTValue& v2);

#endif // __SCORE_ENGINE_H__
