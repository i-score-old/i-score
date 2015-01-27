/*
 * Temporary class to get i-score working with Score and Modular
 * Copyright © 2013, Théo de la Hogue
 *
 * License: This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#ifndef __SCORE_ENGINE_H__
#define __SCORE_ENGINE_H__

#include "TTScore.h"
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

/** a type dedicated to pass time value (date, duration, ...) */
typedef unsigned int TimeValue;

/** a type dedicated to pass bound value */
typedef int BoundValue;

/** a type dedicated to retreive a TimeBox */
typedef unsigned int TimeBoxId;

/** a type dedicated to retreive a TimeEvent attached to a TimeBox */
typedef unsigned int TimeEventIndex;

#define BEGIN_CONTROL_POINT_INDEX 1     // TODO : replace by StartEventIndex
#define END_CONTROL_POINT_INDEX 2       // TODO : replace by EndEventIndex

/** a type dedicated to retreive an Interval (this is related to Relation notion) */
typedef unsigned int IntervalId;

/** a type dedicated to retreive a TimeBox with an Conditioned TimeEvent inside (this is related to TriggerPoint notion) */
typedef unsigned int ConditionedTimeBoxId;

/** a type dedicated to retreive a Condition */
typedef unsigned int TimeConditionId;

/** a class used to cache TTObject and some observers */
class EngineCacheElement {

public:
    TTObject        object;
    unsigned int    index;
    TTAddress       address;
    TTObject        subScenario;
    TTObject        loop;
    
    EngineCacheElement();
    ~EngineCacheElement();
    
};
typedef EngineCacheElement* EngineCacheElementPtr;

/** a type that contains an id and a cached element */
typedef std::pair<unsigned int, EngineCacheElementPtr> EngineCacheKey;
typedef	EngineCacheKey*	EngineCacheKeyPtr;

/** a type to define a map to store and retreive a cached element relative to an id */
typedef std::map<unsigned int, EngineCacheElementPtr> EngineCacheMap;
typedef	EngineCacheMap*	EngineCacheMapPtr;

typedef std::map<unsigned int, EngineCacheElementPtr>::iterator EngineCacheMapIterator;

/** a type to define a map to store and retreive the triggerIds associated with a conditionId */
typedef std::map<unsigned int, std::list<unsigned int>> EngineConditionsMap;

/** a map used to remember the namespace file path for each device */
typedef std::map<std::string, std::string> EngineFilesMap;

#define NO_BOUND -1

#define NO_ID 0
#define ROOT_BOX_ID 1

#define NO_MAX_MODIFICATION 0

#define CURVE_POW 1

/// define part dedicated for debugging
#define iscoreEngineDebug if (accessApplicationLocalDebug)

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
    std::vector<std::string> m_workingProtocols;						/// The protocols that were successfully enabled.
    TTSymbol            iscore;                                         /// application name
    
    TTSymbol            m_lastProjectFilePath;                          /// the last project file path
    EngineFilesMap      m_namespaceFilesPath;                           /// the last namespace file used for each device
    
    TTObject            m_mainScenario;                                 /// The top scenario
    
    unsigned int        m_nextTimeBoxId;                                /// the next Id to give to any created time process
    unsigned int        m_nextIntervalId;                               /// the next Id to give to any created interval
    unsigned int        m_nextConditionedTimeBoxId;                     /// the next Id to give to any created time process
    
    EngineCacheMap      m_timeBoxMap;                                   /// All time boxes (e.g. automation + sub scenario + loop and some observers) stored using an unique id
    EngineCacheMap      m_intervalMap;                                  /// All interval processes and some observers stored using an unique id
    EngineCacheMap      m_timeConditionMap;                             /// All condition stored using an unique id
    EngineCacheMap      m_conditionedTimeBoxMap;                        /// All conditioned time box with an conditioned event stored using an unique id
    
    EngineConditionsMap m_conditionsMap;                                /// All conditions Ids (in i-score point of view) mapped to corresponding triggers id

    EngineCacheMap      m_startCallbackMap;                             /// All callback to observe when a time process starts stored using a time process id
    EngineCacheMap      m_endCallbackMap;                               /// All callback to observe when a time process ends stored using a time process id
    
    EngineCacheMap      m_statusCallbackMap;                            /// All callback to observe time event status stored using using a trigger id
    EngineCacheMap      m_readyCallbackMap;                             /// All callback to observe time condition ready state stored using using a trigger id

    TTObject            m_applicationManager;                           /// #TTApplicationManager to enable communication with any distant application using any protocol
    TTObject            m_iscore;                                       /// #TTApplication dedicated to i-score
    TTObject            m_sender;                                       /// #TTSender to send message to any application
    TTObject            m_namespaceObserver;                            /// #TTCallback to be notified when a node is created in learn mode
    
	void (*m_TimeEventStatusAttributeCallback)(ConditionedTimeBoxId, bool);         // allow to notify the Maquette if a triggerpoint is pending
    void (*m_TimeProcessSchedulerRunningAttributeCallback)(TimeBoxId, bool);        // allow to notify the Maquette if a box is running or not
    void (*m_TransportDataValueCallback)(TTSymbol&, const TTValue&);                // allow to notify the Maquette if the transport features have been used remotly (via OSC messages for example)
    void (*m_NetworkDeviceNamespaceCallback)(TTSymbol&);                            // allow to notify the Maquette if a device's namespace have changed (see in setDeviceLearn)
    void (*m_NetworkDeviceConnectionError)(TTSymbol&, TTSymbol&);                   // allow to notify the Maquette if a device connection failed

public:

    // to receive Minuit messages : used to receive query or answers from any other applications
    const int MINUIT_INPUT_PORT = 13579;
    
    // to receive OSC messages : used to control i-score
    const int OSC_INPUT_PORT = 13580;
    
    // to emit OSC messages : used by i-score to send feedbacks or notifications
    const int OSC_OUTPUT_PORT = 13581;

    Engine(void(*timeEventStatusAttributeCallback)(ConditionedTimeBoxId, bool),
           void(*automationSchedulerRunningAttributeCallback)(TimeBoxId, bool),
           void(*transportDataValueCallback)(TTSymbol&, const TTValue&),
           void (*networkDeviceNamespaceCallback)(TTSymbol&),
           void (*m_NetworkDeviceConnectionError)(TTSymbol&, TTSymbol&),
           std::string pathToTheJamomaFolder);
    
    void initModular(const char* pathToTheJamomaFolder = NULL);
    void initScore(const char* pathToTheJamomaFolder = NULL);
    
    void registerIscoreToProtocols();
    
    void dumpAddressBelow(TTNodePtr aNode);
    
    ~Engine();
	const std::vector<std::string>& workingProtocols() 
	{ return m_workingProtocols; }
    // Id management //////////////////////////////////////////////////////////////////
    
    TimeBoxId           cacheTimeBox(TTObject& automation, TTAddress& anAddress, TTObject& subScenario);
    TTObject&           getMainProcess(TimeBoxId boxId);
    TTObject&           getAutomation(TimeBoxId boxId);
    TTObject&           getSubScenario(TimeBoxId boxId);
    TTObject&           getLoop(TimeBoxId boxId);
    void                setLoop(TimeBoxId boxId, TTObject& loop);
    TTAddress&          getAddress(TimeBoxId boxId);
    void                uncacheTimeBox(TimeBoxId boxId);
    void                clearTimeBox();
    
    TimeBoxId           getParentId(TimeBoxId boxId);
    void                getChildrenId(TimeBoxId boxId, std::vector<TimeBoxId>& childrenId);
    
    IntervalId          cacheInterval(TTObject& interval);
    TTObject&           getInterval(IntervalId relationId);
    void                uncacheInterval(IntervalId relationId);
    void                clearInterval();
    
    ConditionedTimeBoxId cacheConditionedTimeBox(TimeBoxId boxId, TimeEventIndex controlPointId);
    TTObject&           getConditionedTimeProcess(ConditionedTimeBoxId triggerId, TimeEventIndex& controlPointId);
    void                uncacheConditionedTimeBox(ConditionedTimeBoxId triggerId);
    void                clearConditionedTimeBox();
    
    void                cacheTimeCondition(ConditionedTimeBoxId triggerId, TTObject& timeCondition, TTAddress& anAddress = kTTAdrsEmpty);
    TTObject&           getTimeCondition(ConditionedTimeBoxId triggerId);
    void                uncacheTimeCondition(ConditionedTimeBoxId triggerId);
    void                clearTimeCondition();
    
    void                cacheStartCallback(TimeBoxId boxId);
    void                uncacheStartCallback(TimeBoxId boxId);
    
    void                cacheEndCallback(TimeBoxId boxId);
    void                uncacheEndCallback(TimeBoxId boxId);
    
    void                cacheStatusCallback(ConditionedTimeBoxId triggerId, TimeEventIndex controlPointId);
    void                uncacheStatusCallback(ConditionedTimeBoxId triggerId, TimeEventIndex controlPointId);
    
    void                cacheReadyCallback(ConditionedTimeBoxId triggerId);
    void                uncacheReadyCallback(ConditionedTimeBoxId triggerId);
    void                appendToCacheReadyCallback(ConditionedTimeBoxId triggerId, ConditionedTimeBoxId triggerIdToAppend);
    void                removeFromCacheReadyCallback(ConditionedTimeBoxId triggerId, ConditionedTimeBoxId triggerIdToRemove);
    
    void                cacheTriggerDataCallback(ConditionedTimeBoxId triggerId, TimeBoxId boxId);
    void                uncacheTriggerDataCallback(ConditionedTimeBoxId triggerId);
        
	// Edition ////////////////////////////////////////////////////////////////////////
    
    /*!
	 * Get the next time box ID to prepare a name.
	 *
	 * \return the next time box ID.
	 */
	TimeBoxId getNextTimeBoxId();
    
	/*!
	 * Adds a new box in the CSP.
	 *
	 * \param boxBeginPos : the begin value in ms.
	 * \param boxLength : the length value in ms.
     * \param name : the name of the box
	 * \param motherId : mother box ID (default : root scenario)
	 *
	 * \return the newly created box ID.
	 */
	TimeBoxId addBox(TimeValue boxBeginPos, TimeValue boxLength, const std::string & name, TimeBoxId motherId = ROOT_BOX_ID);
    
	/*!
	 * Removes a box from the CSP : removes the relation implicating it and the
	 * box's variables.
	 *
	 * \param boxId : the ID of the box to remove.
	 */
	void removeBox(TimeBoxId boxId);
    
	/*!
	 * Adds a AntPostRelation between two controlPoints.
	 *
	 * \param boxId1 : the ID of the first box
	 * \param controlPoint1 : the index of the point in the first box to put in relation
	 * \param boxId2 : the ID of the second box
	 * \param controlPoint2 : the index of the point in the second box to put in relation
	 * \param movedBoxes : empty vector, will be filled with the ID of the boxes moved by this new relation
	 *
	 * \return the newly created relation id (NO_ID if the creation is impossible).
	 */
	IntervalId addTemporalRelation(TimeBoxId boxId1, TimeEventIndex controlPoint1,
                                     TimeBoxId boxId2, TimeEventIndex controlPoint2,
                                     std::vector<TimeBoxId>& movedBoxes);
    
    /*!
	 * Removes the temporal relation using given id.
	 *
	 * \param relationId : the ID of the relation to remove
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
	void changeTemporalRelationBounds(IntervalId relationId, BoundValue minBound, BoundValue maxBound, std::vector<TimeBoxId>& movedBoxes);
    
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
	bool isTemporalRelationExisting(TimeBoxId boxId1, TimeEventIndex controlPoint1, TimeBoxId boxId2, TimeEventIndex controlPoint2);
    
	/*!
	 * Gets the Id of the first box linked with the given relation id.
	 *
	 * \param relationId : the id of the relation.
	 *
	 * \return the id of the first box.
	 */
	TimeBoxId getRelationFirstBoxId(IntervalId relationId);
    
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
	TimeBoxId getRelationSecondBoxId(IntervalId relationId);
    
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
	bool performBoxEditing(TimeBoxId boxId, TimeValue start, TimeValue end, std::vector<TimeBoxId>& movedBoxes);
    
    /*!
	 * Gets the name of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the name of the box matching the given ID
	 */
    std::string getBoxName(TimeBoxId boxId);
    
    /*!
	 * Gets the vertical position of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the vertical position of the box matching the given ID
	 */
    unsigned int getBoxVerticalPosition(TimeBoxId boxId);
    
    /*!
	 * Gets the vertical position of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \param newPosition : the new vertical position of the box matching the given ID
	 */
    void setBoxVerticalPosition(TimeBoxId boxId, unsigned int newPosition);
    
    /*!
	 * Gets the vertical size of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the vertical size of the box matching the given ID
	 */
    unsigned int getBoxVerticalSize(TimeBoxId boxId);

    /*!
	 * Sets the vertical size of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \param newSize : the new vertical size of the box matching the given ID
	 */
    void setBoxVerticalSize(TimeBoxId boxId, unsigned int newSize);
    
    /*!
	 * Gets the color of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the color of the box matching the given ID
	 */
    QColor getBoxColor(TimeBoxId boxId);
    
    /*!
	 * Sets the color of the box matching the given ID
	 *
	 *
	 * \param boxId : the ID of the box
	 *
	 * \param newColor : the color of the box matching the given ID
	 */
    void setBoxColor(TimeBoxId boxId, QColor newColor);
    
    /*!
     * Sets the mute state of the box (process) matching the given ID
     *
     *
     * \param boxId : the ID of the box
     *
     * \param muteState : the muteState of the box matching the given ID
     */
    void setBoxMuteState(TimeBoxId boxId, bool muteState);
    
    /*!
     * Get the mute state of the box (process) matching the given ID
     *
     *
     * \param boxId : the ID of the box
     *
     * \return the muteState of the box matching the given ID
     */
    bool getBoxMuteState(TimeBoxId boxId);

    /*!
     * Sets the name of the box matching the given ID
     *
     *
     * \param boxId : the ID of the box
     *
     * \param newColor : the name of the box matching the given ID
     */
    void setBoxName(TimeBoxId boxId, std::string name);

	/*!
	 * Gets the begin value of the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the begin value of the box matching the given ID
	 */
	TimeValue getBoxBeginTime(TimeBoxId boxId);
    
	/*!
	 * Gets the end value of the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the end value of the box matching the given ID
	 */
	TimeValue getBoxEndTime(TimeBoxId boxId);
    
	/*!
	 * Gets the end duration of the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the duration value of the box matching the given ID
	 */
	TimeValue getBoxDuration(TimeBoxId boxId);
    
	/*!
	 * Gets the number of controlPoints in the box matching the given ID
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the number of controlPoints in the box matching the given ID
	 */
	int getBoxNbCtrlPoints(TimeBoxId boxId);
    
	/*!
	 * Gets the index of the first control point (the left one).
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param id : the ID of the box
	 *
	 * \return the index of the first control point (the left one)
	 */
	TimeEventIndex getBoxFirstCtrlPointIndex(TimeBoxId boxId);
    
	/*!
	 * Gets the index of the first control point (the right one).
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box
	 *
	 * \return the index of the first control point (the right one)
	 */
	TimeEventIndex getBoxLastCtrlPointIndex(TimeBoxId boxId);
    
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
	void setCtrlPointMessagesToSend(TimeBoxId boxId, TimeEventIndex controlPointIndex, std::vector<std::string> messageToSend, bool muteState = false);
    
	/*!
	 * Gets the message to send (via Network) when the given controlPoint is reached, in a vector given as parameter.
	 *
	 * Throws OutOfBoundException if the ID is not matching any box.
	 *
	 * \param boxId : the ID of the box containing this controlPoint.
	 * \param controlPointIndex : the index of the control point to set message.
	 * \param messages : vector to fill with all messages that will be sent (the result)
	 */
	void getCtrlPointMessagesToSend(TimeBoxId boxId, TimeEventIndex controlPointIndex, std::vector<std::string>& messages);
    
	/*!
	 * Sets the control point mute state. If muted, the messages related to this control point will not be send.
	 *
	 * \param boxId : the ID of the box.
	 * \param controlPointIndex : the index of the control point.
	 * \param mute : the mute state to set (true or false).
	 */
	void setCtrlPointMutingState(TimeBoxId boxId, TimeEventIndex controlPointIndex, bool mute);
    
    /*!
	 * Gets the control point mute state.
	 *
	 * \param boxId : the ID of the box.
	 * \param controlPointIndex : the index of the control point.
	 * \return the mute state (true or false).
	 */
	bool getCtrlPointMutingState(TimeBoxId boxId, TimeEventIndex controlPointIndex);
    
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
	void addCurve(TimeBoxId boxId, const std::string & address);
    
	/*!
	 * Removes the address from the box, and so the matching curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : the curve address.
	 */
	void removeCurve(TimeBoxId boxId, const std::string & address);
    
	/*!
	 * Removes all curve address from the box.
	 *
	 * \param boxId : the Id of the box.
	 */
	void clearCurves(TimeBoxId boxId);
    
	/*!
	 * Gets all the curves address stored in a box.
	 *
	 * \param boxId : the Id of the box.
	 *
	 * \return all curves addresses in a vector.
	 */
	std::vector<std::string> getCurvesAddress(TimeBoxId boxId);
    
	/*!
	 * Changes the sample rate of a curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 * \param nbSamplesBySec : new sample rate.
	 */
	void setCurveSampleRate(TimeBoxId boxId, const std::string & address, unsigned int nbSamplesBySec);
    
	/*!
	 * Gets the sample rate of a curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 *
	 * \return the sample rate (0 if this address is not present as a curve).
	 */
	unsigned int getCurveSampleRate(TimeBoxId boxId, const std::string & address);
    
	/*!
	 * Changes the avoid redundancy information of a curve.
	 * If set to true, the sample rate will not be respect if two consecutive values are identical. Only the first
	 * will be sent.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 * \param avoidRedundancy : new redundancy information.
	 */
	void setCurveRedundancy(TimeBoxId boxId, const std::string & address, bool redundancy);
    
	/*!
	 * Gets the avoid redundancy information of a curve.
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 *
	 * \return the avoid redundancy information.
	 */
	bool getCurveRedundancy(TimeBoxId boxId, const std::string & address);
    
	void setCurveMuteState(TimeBoxId boxId, const std::string & address, bool muteState);
	bool getCurveMuteState(TimeBoxId boxId, const std::string & address);
    
    /*!
	 * Changes the recording information of a curve.
	 * If set to true, the curve will record it self during the next execution of the box
	 *
	 * \param boxId : the Id of the box.
	 * \param address : curve address.
	 * \param record : new record information.
	 */
    void setCurveRecording(TimeBoxId boxId, const std::string & address, bool record);
    
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
	bool setCurveSections(TimeBoxId boxId, std::string address, unsigned int argNb,
                          const std::vector<float>& xPercents, const std::vector<float>& yValues, const std::vector<short>& sectionType, const std::vector<float>& coeff);
    
	bool getCurveSections(TimeBoxId boxId, std::string address, unsigned int argNb,
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
	bool getCurveValues(TimeBoxId boxId, const std::string & address, unsigned int argNb, std::vector<float>& result);
    
	/*!
	 * Adds a new triggerPoint to a box.
     *
     * \param boxId : the Id of the box.
	 *
	 * \return the created trigger ID
	 */
	ConditionedTimeBoxId addTriggerPoint(TimeBoxId boxId, TimeEventIndex controlPointIndex);
    
	/*!
	 * Removes the triggerPoint from a box.
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger to be removed.
	 */
	void removeTriggerPoint(ConditionedTimeBoxId triggerId);

    /*!
     * Mix multiple triggerPoints into one TimeCondition.
     *
     * \return the created condition ID
     */
    TimeConditionId createCondition(std::vector<ConditionedTimeBoxId> triggerIds);

    /*!
     * Add a trigger point to the condition.
     *
     * \param conditionId : the ID of the condition
     * \param triggerId : the ID of the trigger to add
     */
    void attachToCondition(TimeConditionId conditionId, ConditionedTimeBoxId triggerId);

    /*!
     * Remove a trigger point from the condition.
     *
     * \param conditionId : the ID of the condition
     * \param triggerId : the ID of the trigger to remove
     */
    void detachFromCondition(TimeConditionId conditionId, ConditionedTimeBoxId triggerId);

    /*!
     * Delete the specified TimeCondition.
     *
     * \param conditionId : the ID of the condition to delete
     */
    void deleteCondition(TimeConditionId conditionId);

    void getConditionTriggerIds(TimeConditionId conditionId, std::vector<TimeBoxId>& triggerIds);
    
    // Sick of useless doc
    void setConditionMessage(TimeConditionId conditionId, std::string disposeMessage);

    // Sick of useless doc
    std::string getConditionMessage(TimeConditionId conditionId);

	/*!
	 * Sets the triggerPoint (given by ID) message.
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger.
	 * \param triggerMessage : the trigger message
	 */
	void setTriggerPointMessage(ConditionedTimeBoxId triggerId, std::string triggerMessage);
    
	/*!
	 * Gets the triggerPoint (given by ID) message.
	 *
	 * Throws OutOfBoundException if the ID is not matching any triggerPoint.
	 *
	 * \param triggerId : the ID of the trigger.
	 *
	 * \return the trigger message
	 */
	std::string getTriggerPointMessage(ConditionedTimeBoxId triggerId);

    // Sick of useless doc
    void setTriggerPointDefault(ConditionedTimeBoxId triggerId, bool dflt);

    // Sick of useless doc
    bool getTriggerPointDefault(ConditionedTimeBoxId triggerId);
    
	/*!
	 * Gets the id of the box linked to the given trigger point.
	 *
	 * \param triggerId : the trigger Id.
	 *
	 * \return the id of the box linked to the trigger point,
	 * NO_ID if the trigger point is not linked to a box.
	 */
	TimeBoxId getTriggerPointRelatedBoxId(ConditionedTimeBoxId triggerId);
    
	/*!
	 * Gets the index of the control point linked to the given trigger point.
	 *
	 * \param triggerId : the trigger Id.
	 *
	 * \return the index of the control point linked to the trigger point,
	 * NO_ID if the trigger point is not linked to a control point.
	 */
	TimeBoxId getTriggerPointRelatedCtrlPointIndex(ConditionedTimeBoxId triggerId);
    
	/*!
	 * Fills the given vector with all the boxes ID used in the editor.
	 * Useful after a load.
	 *
	 * \param boxesID : the vector to fill with all boxes ID used.
	 */
	void getBoxesId(std::vector<TimeBoxId>& boxesID);
    
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
	void getTriggersPointId(std::vector<ConditionedTimeBoxId>& triggersID);

    /*!
     * Fills the given vector with all the conditions ID used in the editor.
     * Useful after a load.
     *
     * \param conditionsID : the vector to fill with all conditions ID used.
     */
    void getConditionsId(std::vector<TimeConditionId>& conditionsID);
    
    /*!
     * Enable loop on a box
     *
     * \param boxId : the box to loop
     */
    bool enableLoop(TimeBoxId boxId);
    
    /*!
     * Disable loop on a box
     *
     * \param boxId : the box to not loop
     */
    bool disableLoop(TimeBoxId boxId);
    
    /*!
     * Is there a loop for a box ?
     *
     * \param boxId : the box to not loop
     * \return true if there is a loop for the box
     */
    bool isLoop(TimeBoxId boxId);
    
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
    
    /*!
	 * Sets scenario time offset to start from this date on the next play
     *
     * \param timeOffset: new timeOffset value
	 *
	 */
	void setTimeOffset(TimeValue timeOffset, bool mute = NO);
    
    /*!
	 * Gets the time offset where the scenario will start from on the next play
     *
     * \return time offset value
	 *
	 */
	TimeValue getTimeOffset();
    
	/*!
	 * Plays a box.
	 *
     * \param boxId: the id of the process (default : the main scenario).
	 * \return true if the box starts fine.
	 */
	bool play(TimeBoxId boxId = ROOT_BOX_ID);
    
    /*!
	 * Tests if the main scenario is actually running.
	 *
     * \param boxId: the id of the process (default : the main scenario).
	 * \return true if the main scnerio is running. False if not.
	 */
	bool isPlaying(TimeBoxId boxId = ROOT_BOX_ID);

	/*!
	 * Stops a box.
	 *
     * \param boxId: the id of the process (default : the main scenario).
	 * \return true if the box will actually stop.
	 */
	bool stop(TimeBoxId boxId = ROOT_BOX_ID);
    
    /*!
	 * Pause a box.
     *
     * \param pause: new pause value
     * \param boxId: the id of the process (default : the main scenario).
	 *
	 */
	void pause(bool pauseValue, TimeBoxId boxId = ROOT_BOX_ID);
 
    /*!
	 * Tests if a box is actually paused.
	 *
     * \param boxId: the id of the process (default : the main scenario).
	 * \return true if the main scnerio is paused. False if not.
	 */
	bool isPaused(TimeBoxId boxId = ROOT_BOX_ID);
    
	/*!
	 * Gets box current execution time (default : the main scenario).
	 *
     * \param boxId: the id of the process.
	 * \return the execution time in milliseconds.
	 */
	TimeValue getCurrentExecutionDate(TimeBoxId boxId = ROOT_BOX_ID);
    
    /*!
	 * Gets box current execution position (default : the main scenario).
	 *
     * \param boxId: the id of the process.
	 * \return the execution position (normalized [0::1]).
	 */
	float getCurrentExecutionPosition(TimeBoxId boxId = ROOT_BOX_ID);
    
	/*!
	 * Changes the execution speed of a box (default : the main scenario).
	 *
	 * \param factor: the new speed factor.
     * \param boxId: the id of the process.
	 */
	void setExecutionSpeedFactor(float factor, TimeBoxId boxId = ROOT_BOX_ID);
    
    /*!
	 * Gets the execution speed of a box (default : the main scenario).
	 *
     * \param boxId: the id of the process.
	 * \return the speed factor.
	 */
	float getExecutionSpeedFactor(TimeBoxId boxId = ROOT_BOX_ID);
    
    
	//Network //////////////////////////////////////////////////////////////////////////////////////////////
    
	/*!
	 * Trigger a trigger point manually.
	 *
	 * Tells the ECOMachine that a trigger point is trggered.
	 *
	 * \param triggerId : the id of the trigger point to trigger.
	 */
	void trigger(ConditionedTimeBoxId triggerId);
    
    /*!
	 * Trigger several trigger point manually.
	 *
	 * Tells the ECOMachine that a trigger point is trggered.
	 *
	 * \param triggerIds : a vector of trigger point id to trigger.
	 */
	void trigger(std::vector<ConditionedTimeBoxId> triggerIds);
    
	/*!
	 * Adds a network device.
	 *
	 * \param deviceName : the name to give to this device.
	 * \param pluginToUse : plugin to use with this device (the plugin name could be retrieved with
	 * the networkGetAllLoadedPlugins function).
	 * \param deviceIp : the ip of the network device (for Minuit and OSC plugin)
	 * \param devicePort : the port of the network device (for Minuit and OSC plugin)
     * \param isInputPort : true for input, false for output (for MIDI plugin)
     * \param stringPort : the port name (for MIDI plugin)
	 */
    void addNetworkDevice(const std::string & deviceName, const std::string & pluginToUse, const std::string & deviceIp, const unsigned int & destinationPort, const unsigned int & receptionPort = 0, const bool isInputPort = false, const std::string & stringPort = "");
    
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
	 * Fills the given vectors with all protocol names.
	 *
	 * \param allProtocolNames : will be filled with all protocol names.
	 */
    void getProtocolNames(std::vector<std::string>& allProtocolNames);
    
	/*!
	 * Fills the given vectors with all network devices name.
	 *
	 * \param allDeviceNames : will be filled with all network devices names.
	 */
	void getNetworkDevicesName(std::vector<std::string>& allDeviceNames);
    
    bool isNetworkDeviceRequestable(const std::string deviceName);
    
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
     * Set an attribute value.
     *
     * \param address : the object's address. ex : /deviceName/address1/address2/
     * \param attribute : the attribute's name we request. ex : "service", "rangeBounds"...etc
     * \param attributeValue : the attribute values.
     *
     * \return True(1) or false(0) if the set have failed or not.
     */
    int setObjectAttributeValue(const std::string & address, const std::string & attribute, std::string & value);

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
     * Sends a request to get the type of an object.
     *
     * \param address : the object's address. ex : /deviceName/address1/address2/
     * \param Priority : will be filled with the priority.
     *
     * \return True(1) or false(0) if the request failed or not.
     */
    int requestObjectPriority(const std::string & address, unsigned int & nodeType);

    /*!
     * Sends a request to get the children nodes of an object.
     *
     * \param address : the object's address. ex : /deviceName/address1/address2/
     * \param children : will be filled with the children' addresses.
     *
     * \return True(1) or false(0) if the request failed or not.
     */
    int requestObjectChildren(const std::string & address, std::vector<std::string>& children);

    /*!
     * Refresh the namespace by rebuilding the mirror (Minuit protocol case) or reloading the namesapce from the last project file (OSC protocol case)
     *
     * \param deviceName : the device name to rebuild
     * \param address : the object's address
     * \return 0 if no error, else 1.
     */
    bool rebuildNetworkNamespace(const std::string& deviceName, const std::string& address = "/");
    
    /*!
     * Load a namespace from a namespace file
     *
     * \param deviceName : the devine name to setup
     * \param filepath : the path to a namespace file
     */
    bool loadNetworkNamespace(const std::string &deviceName, const std::string &filepath);
    
    /*!
     * append a new address to a network device
     *
     * \param address : the address to add
     * \param service : service provided by the address (parameter, message, return)
     * \param type : type of the value handled by the address (none, generic, boolean, decimal, integer, string, array)
     * \param priority : the priority order of this address
     * \param description : description of the address
     * \param range : for numericla value it is the min and the max, for string value it could be an enumeration
     * \param clipmode : the behavior of the value at min or max bounds (none, low, high, both, wrap, fold)
     * \param tags : any words
     */
    int appendToNetWorkNamespace(const std::string & address, const std::string & service = "parameter", const std::string & type = "generic", const std::string & priority = "0", const std::string & description = "", const std::string & range = "0. 1.", const std::string & clipmode = "none", const std::string & tags = "");
    
    /*!
     * remove an new address from a network device
     *
     * \param address : the address to remove
     */
    int removeFromNetWorkNamespace(const std::string & address);

    /*!
     * Gets an integer parameter associated to the protocol and the device.
     *
     * \param protocol : the protocol's name. ex : Minuit
     * \param device : the device's name. ex: MinuitDevice1
     * \param parameter : the parameter asked. ex: port
     * \param integer : the integer returned. ex: 8002
     * \return 0 if no error, else 1.
     */
    bool getDeviceIntegerParameter(const std::string device, const std::string protocol, const std::string parameter, unsigned int &integer);

    bool getDeviceIntegerVectorParameter(const std::string device, const std::string protocol, const std::string parameter, std::vector<int> &integerVect);
    /*!
     * Gets a string parameter associated to the protocol and the device.
     *
     * \param protocol : the protocol's name. ex : Minuit
     * \param device : the device's name. ex: MinuitDevice1
     * \param parameter : the parameter asked. ex: ip
     * \param string : the integer returned. ex: 192.168.1.1
     * \return 0 if no error, else 1.
     */
    bool getDeviceStringParameter(const std::string device, const std::string protocol, const std::string parameter, std::string &string);

    /*!
     * Gets the protocol name associated to the device.
     *
     * \param device : the device's name. ex: MinuitDevice1
     * \param protocol : the protocol name returned. ex: Minuit
     * \return 0 if no error, else 1.
     */
    bool getDeviceProtocol(std::string deviceName, std::string &protocol);

    /*!
     * Sets the device's name.
     *
     * \param device : the device's name. ex: MinuitDevice1
     * \param newName : the newName. ex: MyMinuit
     * \return 0 if no error, else 1.
     */
    bool setDeviceName(std::string deviceName, std::string newName);

    /*!
     * Sets the device's port.
     *
     * \param device : the device's name. ex: MinuitDevice1
     * \param destinationPort : the port value. ex: 8002
     * \param receptionPort : the port value. ex: 8002 (useful for OSC plugin)
     * \return 0 if no error, else 1.
     */
    bool setDevicePort(std::string deviceName, int destinationPort, int receptionPort = 0);

    /*!
     * Sets the device's local host.
     *
     * \param device : the device's name. ex: MinuitDevice1
     * \param port : the local host value. ex: 192.168.1.1
     * \return 0 if no error, else 1.
     */
    bool setDeviceLocalHost(std::string deviceName, std::string localHost);

    /*!
     * Sets the device's protocol. (considering a device has only one)
     *
     * \param device : the device's name. ex: MinuitDevice1
     * \param port : the protocol name. ex: OSC
     * \return 0 if no error, else 1.
     */
    bool setDeviceProtocol(std::string deviceName, std::string protocol);
    
    /*!
     * Sets the device in learn mode.
     *
     * \param device : the device's name. ex: MinuitDevice1
     * \param newLearn : a boolean to set the device in learn mode
     * \return 0 if no error, else 1.
     */
    bool setDeviceLearn(std::string deviceName, bool newLearn);
    
    /*!
     * Sets the device in learn mode.
     *
     * \param device : the device's name. ex: MinuitDevice1
     * \return the learn status of the device.
     */
    bool getDeviceLearn(std::string deviceName);
    
    /*!
     * Scan the Network using a protocol to get all available devices
     *
     * \param protocol : the protocol to scan
     * \param scanResult : a vector containing options for scanning (ex : for MIDI protocol pass "inputs" or "outputs", for OSC protocol ?)
     * \param scanResult : a vector containing all available devices
     * \return true if the scan succeed
     */
    bool protocolScan(const std::string & protocol, std::vector<std::string>&& scanOptions, std::vector<std::string>& scanResult);

	//Store and load ////////////////////////////////////////////////////////////////////////////////////
    
	/*!
	 * Store Engine.
	 *
	 * \param filepath : the filepath to use.
     * \return 1 if the storage succeed
	 */
	int store(std::string filepath);
    
	/*!
	 * Load Engine.
	 *
	 * \param filepath : the filepath to use.
     * \return 1 if the load succeed
	 */
	int load(std::string filepath);
    void buildEngineCaches(TTObject& scenario, TTAddress& scenarioAddress);
    void buildConditionedTimeBoxCache(TimeBoxId boxId, TTObject& startEvent, TTObject& endEvent, std::map<TTObjectBasePtr, TimeConditionId> TTCondToID);
    
	/*!
	 * Prints on standard output both engines. Useful only for debug purpose.
	 */
	void print();
    void printExecutionInLinuxConsole();
    
    friend void TimeEventStatusAttributeCallback(const TTValue& baton, const TTValue& value);
    friend void TimeConditionReadyAttributeCallback(const TTValue& baton, const TTValue& value);
    friend void AutomationStartCallback(const TTValue& baton, const TTValue& value);
    friend void AutomationEndCallback(const TTValue& baton, const TTValue& value);
    friend void TriggerReceiverValueCallback(const TTValue& baton, const TTValue& value);
    friend void NamespaceCallback(const TTValue& baton, const TTValue& value);
    
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

/** time event status attribute callback
 @param	baton			an EnginePtr and a ConditionedTimeBoxId
 @param	value			a time event
 @return                an error code */
void TimeEventStatusAttributeCallback(const TTValue& baton, const TTValue& value);

/** time condition ready attribute callback
 @param	baton			an EnginePtr and a ConditionedTimeBoxId
 @param	value			a #TTBoolean new ready state
 @return                an error code */
void TimeConditionReadyAttributeCallback(const TTValue& baton, const TTValue& value);

/** Callback used each time a process starts
 @param	baton			an EnginePtr and a TimeBoxId
 @param	value			nothing
 @return                an error code */
void AutomationStartCallback(const TTValue& baton, const TTValue& value);

/** Callback used each time a process ends
 @param	baton			an EnginePtr and a TimeBoxId
 @param	value			nothing
 @return                an error code */
void AutomationEndCallback(const TTValue& baton, const TTValue& value);

/** Callback used for namespace observation
 @param	baton			an EnginePtr and an application name
 @param	value			...
 @return                an error code */
void NamespaceCallback(const TTValue& baton, const TTValue& value);

#endif // __SCORE_ENGINE_H__
