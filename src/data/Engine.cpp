/*
 * Temporary class to get i-score working with Score and Modular
 * Copyright © 2013, Théo de la Hogue
 *
 * License: This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#include "Engine.h"

#include <stdio.h>
#include <math.h>
#include <QDebug>

using namespace std;

/*!
 * \file Engine.cpp
 * \author Théo de la Hogue, based on Engine.cpp written by Raphael Marczak (LaBRI) for the libIscore.
 * \date 2012-2013
 */

EngineCacheElement::EngineCacheElement()
{
    ;
}

EngineCacheElement::~EngineCacheElement()
{
    ;
}

Engine::Engine(void(*timeEventStatusAttributeCallback)(ConditionedProcessId, bool),
               void(*timeProcessSchedulerRunningAttributeCallback)(TimeProcessId, bool),
               void(*transportDataValueCallback)(TTSymbol&, const TTValue&),
               void (*networkDeviceNamespaceCallback)(TTSymbol&),
               void (*networkDeviceConnectionError)(TTSymbol&, TTSymbol&),
               std::string pathToTheJamomaFolder)
{
    m_TimeEventStatusAttributeCallback = timeEventStatusAttributeCallback;
    m_TimeProcessSchedulerRunningAttributeCallback = timeProcessSchedulerRunningAttributeCallback;
    m_TransportDataValueCallback = transportDataValueCallback;
    m_NetworkDeviceNamespaceCallback = networkDeviceNamespaceCallback;
    m_NetworkDeviceConnectionError = networkDeviceConnectionError;
    
    m_nextTimeProcessId = 1;
    m_nextIntervalId = 1;
    m_nextConditionedProcessId = 1;
    
    iscore = TTSymbol("i-score");
    
    if (!pathToTheJamomaFolder.empty()){
        
        initModular(pathToTheJamomaFolder.c_str());
        initScore(pathToTheJamomaFolder.c_str());
    }
    else {
        
        initModular();
        initScore();
    }
}

void Engine::initModular(const char* pathToTheJamomaFolder)
{
    TTErr    err;
    TTValue  args, out;
    
    TTLogMessage("\n*** Initialisation of Modular environnement ***\n");
    ////////////////////////////////////////////////////////////////////////

    // this initializes the Modular framework and loads protocol plugins
    TTModularInit(pathToTheJamomaFolder);
    
    // create application manager
    m_applicationManager = TTObject("ApplicationManager");

    
    TTLogMessage("\n*** Creation of i-score application ***\n");
    ////////////////////////////////////////////////////////////////////////////////
    
    // create a local application called "i-score" and get it back
    err = m_applicationManager.send("ApplicationInstantiateLocal", iscore, out);
    
    if (err) {
        TTLogError("Error : can't create i-score application \n");
        return;
    }
    else
        m_iscore = out[0];
    
    // set the application in debug mode
    m_iscore.set("debug", YES);
    
    // create a sender to send message to any application
    m_sender = TTObject("Sender");
    
    registerIscoreToProtocols();
}

void Engine::registerIscoreToProtocols()
{
    TTObject    aProtocol;
    TTErr       err;
    TTValue     none, out;
    
    
    TTLogMessage("\n*** Enable Minuit communication ***\n");
    ////////////////////////////////////////////////////////////////////////
    
    // create a Minuit protocol unit
	try
	{
	    err = m_applicationManager.send("ProtocolInstantiate", "Minuit", out);
	    
	    if (err) {
	        TTLogError("Error : can't create Minuit protocol unit \n");
	        return;
	    }
	    else
	        aProtocol = out[0];
	    
	    // register i-score to the Minuit protocol
	    aProtocol.send("ApplicationRegister", iscore, out);
	    
	    // select i-score to set its Minuit protocol parameters
	    aProtocol.send("ApplicationSelect", iscore, out);
	    aProtocol.set("port", MINUIT_INPUT_PORT);
	    aProtocol.set("ip", "127.0.0.1");

	    // launch Minuit protocol communication
	    // launch Minuit protocol communication
    	err = aProtocol.send("Run", none, out);
    
    	if (err) {
        	out.toString();
        	TTSymbol errorInfo = TTSymbol(TTString(out[0]));
        	m_NetworkDeviceConnectionError(iscore, errorInfo);
    	}

		m_workingProtocols.push_back("Minuit");
	}
	catch(TTException& e)
	{
		TTLogError("Error : could not instantiate Minuit protocol: ");
		TTLogError(e.getReason());
	}
    
    TTLogMessage("\n*** Enable OSC communication ***\n");
    ////////////////////////////////////////////////////////////////////////
    
    // create a OSC protocol unit
	try
	{
	    err = m_applicationManager.send("ProtocolInstantiate", "OSC", out);
	    
	    if (err) {
	        TTLogError("Error : can't create OSC protocol unit \n");
	        return;
	    }

	    aProtocol = out[0];
	    
	    // register i-score to the OSC protocol
	    aProtocol.send("ApplicationRegister", iscore, out);
	    
	    // select i-score to set its OSC protocol parameters
	    aProtocol.send("ApplicationSelect", iscore, out);
	    aProtocol.set("port", OSC_INPUT_PORT);
	    aProtocol.set("ip", "127.0.0.1");
	    
	    // launch Minuit protocol communication
    	err = aProtocol.send("Run", none, out);
    
    	if (err) {
        	out.toString();
        	TTSymbol errorInfo = TTSymbol(TTString(out[0]));
        	m_NetworkDeviceConnectionError(iscore, errorInfo);
    	}
		
		m_workingProtocols.push_back("OSC");
	}
	catch(TTException& e)
	{
		TTLogError("Error : could not instantiate OSC protocol: ");
		TTLogError(e.getReason());
	}
    
    TTLogMessage("\n*** Enable MIDI communication ***\n");
    ////////////////////////////////////////////////////////////////////////
    
    // create a OSC protocol unit
	try 
	{
	    err = m_applicationManager.send("ProtocolInstantiate", "MIDI", out);
    
	    if (err) {
	        TTLogError("Error : can't create MIDI protocol unit \n");
	        return;
	    }
	    
	    aProtocol = out[0];
    
	    // launch Minuit protocol communication
    	err = aProtocol.send("Run", none, out);
    
    	if (err) {
        	out.toString();
        	TTSymbol errorInfo = TTSymbol(TTString(out[0]));
        	m_NetworkDeviceConnectionError(iscore, errorInfo);
    	}
		
		m_workingProtocols.push_back("MIDI");
	}
	catch(TTException& e)
	{
		TTLogError("Error : could not instantiate MIDI protocol: ");
		TTLogError(e.getReason());
	}
}

void Engine::initScore(const char* pathToTheJamomaFolder)
{   
    TTValue     args, out;
    
    TTLogMessage("\n*** Initialisation of Score environnement ***\n");
    /////////////////////////////////////////////////////////////////////
    
    // Init the Score library (passing the folder path where all the dylibs are)
    TTScoreInit(pathToTheJamomaFolder);
    
    
    TTLogMessage("\n*** Creating an empty Scenario ***\n");
    /////////////////////////////////////////////////////////////////////
    
    // Create an empty Scenario
    m_mainScenario = TTObject("Scenario");
    m_mainScenario.set("name", TTSymbol("Main"));

    // Store the main scenario (so ROOT_BOX_ID is 1)
    TTAddress address("/Main");
    cacheTimeProcess(m_mainScenario, address, m_mainScenario);
}

void Engine::dumpAddressBelow(TTNodePtr aNode)
{
    TTList returnedChildren;
    TTAddress anAddress;
    
    // fill a TTList with all children (because we use * (wilcard) for the name and the instance)
    aNode->getChildren(S_WILDCARD, S_WILDCARD, returnedChildren);
    
    // for each child
    for (returnedChildren.begin(); returnedChildren.end(); returnedChildren.next()) {
        // get a node from the TTList
        aNode = TTNodePtr((TTPtr)returnedChildren.current()[0]);
        
        // compute his absolute address
        aNode->getAddress(anAddress);
        
        std::cout << "   " << anAddress.string() << std::endl;
        
        // dump all addresses below this node (recursive call)
        dumpAddressBelow(aNode);
    }
}

Engine::~Engine()
{
    // Clear all the EngineCacheMaps
    // note : this should be useless because all elements are removed by the maquette
    clearTimeProcess();
    clearInterval();
    clearConditionedProcess();
    clearTimeCondition();
    
    TTValue out;
    
    TTLogMessage("\n*** Release protocols ***\n");
    ///////////////////////////////////////////////
    
    // Get protocol names
    m_applicationManager.get("protocolNames", out);
    for (TTElementIter it = out.begin() ; it != out.end() ; it++) {
        TTSymbol name = TTElement(*it);
        m_applicationManager.send("ProtocolRelease", name, out);
    }
    
    TTLogMessage("\n*** Release applications ***\n");
    //////////////////////////////////////////////////
    
    // Get registered application names
    m_applicationManager.get("applicationNames", out);
    for (TTElementIter it = out.begin() ; it != out.end() ; it++) {
        TTSymbol name = TTElement(*it);
        m_applicationManager.send("ApplicationRelease", name, out);
    }
}

TimeProcessId Engine::cacheTimeProcess(TTObject& timeProcess, TTAddress& anAddress, TTObject& subScenario)
{
    TimeProcessId id;
    EngineCacheElementPtr e;
    
    e = new EngineCacheElement();
    e->object = timeProcess;
    e->address = anAddress;
    e->subScenario = subScenario;
    
    TTValue out, args = TTValue(e->address, e->object);
    m_iscore.send("ObjectRegister", args, out);
    
    id = m_nextTimeProcessId;
    m_timeProcessMap[id] = e;
    m_nextTimeProcessId++;
    
    cacheStartCallback(id);
    cacheEndCallback(id);
    
    return id;
}

TTObject& Engine::getTimeProcess(TimeProcessId boxId)
{
    return m_timeProcessMap[boxId]->object;
}

TTAddress& Engine::getAddress(TimeProcessId boxId)
{
    return m_timeProcessMap[boxId]->address;
}

TTObject& Engine::getSubScenario(TimeProcessId boxId)
{
    return m_timeProcessMap[boxId]->subScenario;
}

TimeProcessId Engine::getParentId(TimeProcessId boxId)
{
    EngineCacheMapIterator it;
    TimeProcessId   parentId = NO_ID;
    TTObject        parentScenario;
    TTValue         v;
    
    m_timeProcessMap[boxId]->object.get("container", v);
    parentScenario = v[0];
    
    // retreive the id of the scenario
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        if (it->second->subScenario == parentScenario) {

            parentId = it->first;
            break;
        }
    }
    
    return parentId;
}

void Engine::getChildrenId(TimeProcessId boxId, vector<TimeProcessId>& childrenId)
{
    EngineCacheMapIterator it;
    TTObject    subScenario = getSubScenario(boxId);
    TTObject    parentScenario;
    TTValue     v;
    
    // Get all boxes id that have the subScenario as parent
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        if (it->first == boxId)
            continue;
        
        it->second->object.get("container", v);
        parentScenario = v[0];
        
        if (parentScenario != nullptr && parentScenario == subScenario)
            childrenId.push_back(it->first);
    }
}

void Engine::uncacheTimeProcess(TimeProcessId boxId)
{
    EngineCacheElementPtr e = m_timeProcessMap[boxId];
    
    uncacheStartCallback(boxId);
    uncacheEndCallback(boxId);
    
    TTValue out;
    m_iscore.send("ObjectUnregister", e->address, out);
    
    delete e;
    m_timeProcessMap.erase(boxId);
}

void Engine::clearTimeProcess()
{
    EngineCacheMapIterator it;
    
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        // don't remove the root time process (the main scenario)
        if (it->first != ROOT_BOX_ID) {
            
            uncacheStartCallback(it->first);
            uncacheEndCallback(it->first);
            delete it->second;
        }
    }
    
    // don't clear the m_timeProcessMap (because it is not empty)
    m_startCallbackMap.clear();
    m_endCallbackMap.clear();
    
    // set the next id to 2 because the main scenario is registered with the 1 id
    m_nextTimeProcessId = 2;
}

IntervalId Engine::cacheInterval(TTObject& timeProcess)
{
    TimeProcessId id;
    EngineCacheElementPtr e;
    
    e = new EngineCacheElement();
    e->object = timeProcess;
    
    id = m_nextIntervalId;
    m_intervalMap[id] = e;
    m_nextIntervalId++;
    
    return id;
}

TTObject& Engine::getInterval(IntervalId relationId)
{
    return m_intervalMap[relationId]->object;
}

void Engine::uncacheInterval(IntervalId relationId)
{
    EngineCacheElementPtr e = m_intervalMap[relationId];
    
    delete e;
    m_intervalMap.erase(relationId);
}

void Engine::clearInterval()
{
    EngineCacheMapIterator it;
    
    for (it = m_intervalMap.begin(); it != m_intervalMap.end(); ++it)
        delete it->second;
    
    m_intervalMap.clear();
    
    m_nextIntervalId = 1;
}

ConditionedProcessId Engine::cacheConditionedProcess(TimeProcessId timeProcessId, TimeEventIndex controlPointId)
{
    TTObject                timeProcess = getTimeProcess(timeProcessId);
    TTObject                timeEvent;
    ConditionedProcessId    id;
    EngineCacheElementPtr   e;
    TTValue                 v;
    
    // Get start or end time event
    if (controlPointId == BEGIN_CONTROL_POINT_INDEX)
        timeProcess.get("startEvent", v);
    else
        timeProcess.get("endEvent", v);
    
    timeEvent = v[0];
    
    // Create a new engine cache element
    e = new EngineCacheElement();
    e->object = timeProcess;
    e->index = controlPointId;
    
    id = m_nextConditionedProcessId;
    m_conditionedProcessMap[id] = e;
    m_nextConditionedProcessId++;
    
    // We cache an observer on time event status attribute
    cacheStatusCallback(id, controlPointId);
    
    return id;
}

TTObject& Engine::getConditionedProcess(ConditionedProcessId triggerId, TimeEventIndex& controlPointId)
{
    controlPointId = m_conditionedProcessMap[triggerId]->index;
    
    return m_conditionedProcessMap[triggerId]->object;
}

void Engine::uncacheConditionedProcess(ConditionedProcessId triggerId)
{
    // Get the engine cache element
    EngineCacheElementPtr e = m_conditionedProcessMap[triggerId];
    
    // Uncache observer on time event status attribute
    uncacheStatusCallback(triggerId, e->index);
    
    // Delete the engine cache element
    delete e;
    m_conditionedProcessMap.erase(triggerId);
}

void Engine::clearConditionedProcess()
{
    EngineCacheMapIterator it;
    
    for (it = m_conditionedProcessMap.begin(); it != m_conditionedProcessMap.end(); ++it) {
        
        uncacheStatusCallback(it->first, it->second->index);
        
        delete it->second;
    }
    
    m_conditionedProcessMap.clear();
    m_statusCallbackMap.clear();
    
    m_nextConditionedProcessId = 1;
}

void Engine::cacheTimeCondition(ConditionedProcessId triggerId, TTObject& timeCondition, TTAddress& anAddress)
{
    EngineCacheElementPtr   e;
    TTValue                 args, out;
    
    e = new EngineCacheElement();
    e->object = timeCondition;
    e->index = triggerId;
    e->address = anAddress;
    
    if (e->address != kTTAdrsEmpty) {
        args = TTValue(e->address, e->object);
        m_iscore.send("ObjectRegister", args, out);
    }
    
    m_timeConditionMap[triggerId] = e;
    
    // We cache an observer on time condition ready attribute
    cacheReadyCallback(triggerId);
}

TTObject& Engine::getTimeCondition(ConditionedProcessId triggerId)
{
    return m_timeConditionMap[triggerId]->object;
}

void Engine::uncacheTimeCondition(ConditionedProcessId triggerId)
{
    EngineCacheElementPtr   e = m_timeConditionMap[triggerId];
    
    TTValue out;
    m_iscore.send("ObjectUnregister", e->address, out);
    
    // Uncache observer on time condition ready attribute
    uncacheReadyCallback(triggerId);
    
    delete e;
    m_timeConditionMap.erase(triggerId);
}

void Engine::clearTimeCondition()
{
    EngineCacheMapIterator it;
    
    for (it = m_timeConditionMap.begin(); it != m_timeConditionMap.end(); ++it)
        delete it->second;
    
    m_timeConditionMap.clear();
}

void Engine::cacheStartCallback(TimeProcessId boxId)
{
    EngineCacheElementPtr   e;
    TTValue                 baton;
    
    e = new EngineCacheElement();
    e->index = boxId;
    
    // create a TTCallback to observe when time process starts (using TimeProcessStartCallback)
    e->object = TTObject("callback");
    
    baton = TTValue(TTPtr(this), TTUInt32(boxId));
    e->object.set("baton", baton);
    e->object.set("function", TTPtr(&TimeProcessStartCallback));
    e->object.set("notification", TTSymbol("ProcessStarted"));
    
    // observe the "ProcessStarted" notification
    getTimeProcess(boxId).registerObserverForNotifications(e->object);

    m_startCallbackMap[boxId] = e;
}

void Engine::uncacheStartCallback(TimeProcessId boxId)
{
    EngineCacheElementPtr   e = m_startCallbackMap[boxId];
    TTValue                 v;
    
    // don't observe the "ProcessStarted" notification anymore
    getTimeProcess(boxId).unregisterObserverForNotifications(e->object);
    
    delete e;
    m_startCallbackMap.erase(boxId);
}

void Engine::cacheEndCallback(TimeProcessId boxId)
{
    EngineCacheElementPtr   e;
    TTValue                 baton;
    
    e = new EngineCacheElement();
    e->index = boxId;
    
    // create a TTCallback to observe when time process starts (using TimeProcessEndCallback)
    e->object = TTObject("callback");
    
    baton = TTValue(TTPtr(this), TTUInt32(boxId));
    e->object.set("baton", baton);
    e->object.set("function", TTPtr(&TimeProcessEndCallback));
    e->object.set("notification", TTSymbol("ProcessEnded"));
    
    // observe the "ProcessStarted" notification
    getTimeProcess(boxId).registerObserverForNotifications(e->object);
    
    m_endCallbackMap[boxId] = e;
}

void Engine::uncacheEndCallback(TimeProcessId boxId)
{
    EngineCacheElementPtr   e = m_endCallbackMap[boxId];
    TTValue                 v;
    
    // don't observe the "ProcessEnded" notification anymore
    getTimeProcess(boxId).registerObserverForNotifications(e->object);
    
    delete e;
    m_endCallbackMap.erase(boxId);
}

void Engine::cacheStatusCallback(ConditionedProcessId triggerId, TimeEventIndex controlPointId)
{
    EngineCacheElementPtr   e;
    TTObject    timeProcess = getConditionedProcess(triggerId, controlPointId);
    TTObject    timeEvent;
    TTValue     v, baton;
    
    // Create a new engine cache element
    e = new EngineCacheElement();
    e->index = triggerId;
    
    // get start or end time event
    if (controlPointId == BEGIN_CONTROL_POINT_INDEX)
        timeProcess.get("startEvent", v);
    else
        timeProcess.get("endEvent", v);
    
    timeEvent = v[0];
    
    // create a TTCallback to observe time event status attribute (using TimeEventStatusAttributeCallback)
    e->object = TTObject("callback");
    
    baton = TTValue(TTPtr(this), TTUInt32(triggerId));
    e->object.set("baton", baton);
    e->object.set("function", TTPtr(&TimeEventStatusAttributeCallback));
    e->object.set("notification", TTSymbol("EventStatusChanged"));
    
    // observe the "EventReadyChanged" notification
    timeEvent.registerObserverForNotifications(e->object);

    m_statusCallbackMap[triggerId] = e;
}

void Engine::uncacheStatusCallback(ConditionedProcessId triggerId, TimeEventIndex controlPointId)
{
    EngineCacheElementPtr   e = m_statusCallbackMap[triggerId];
    TTObject    timeProcess = getConditionedProcess(triggerId, controlPointId);
    TTObject    timeEvent;
    TTValue     v;
    
    // get start or end time event
    if (controlPointId == BEGIN_CONTROL_POINT_INDEX)
        timeProcess.get("startEvent", v);
    else
        timeProcess.get("endEvent", v);
    
    timeEvent = v[0];
    
    // don't observe the "EventStatusChanged" notification anymore
    timeEvent.unregisterObserverForNotifications(e->object);
    
    delete e;
    m_statusCallbackMap.erase(triggerId);
}

void Engine::cacheReadyCallback(ConditionedProcessId triggerId)
{
    EngineCacheElementPtr   e;
    TTObject    timeCondition = getTimeCondition(triggerId);
    TTValue     v, baton;
    
    // Create a new engine cache element
    e = new EngineCacheElement();
    e->index = triggerId;
    
    // create a TTCallback to observe time condition ready attribute (using TimeConditionReadyAttributeCallback)
    e->object = TTObject("callback");
    
    baton = TTValue(TTPtr(this));
    e->object.set("baton", baton);
    e->object.set("function", TTPtr(&TimeConditionReadyAttributeCallback));
    e->object.set("notification", TTSymbol("ConditionReadyChanged"));
    
    // observe the "ConditionReadyChanged" notification
    timeCondition.registerObserverForNotifications(e->object);
    
    m_readyCallbackMap[triggerId] = e;
}

void Engine::uncacheReadyCallback(ConditionedProcessId triggerId)
{
    EngineCacheElementPtr   e = m_readyCallbackMap[triggerId];
    TTObject    timeCondition = getTimeCondition(triggerId);
    
    // don't observe the "ConditionReadyChanged" notification anymore
    timeCondition.unregisterObserverForNotifications(e->object);
    
    delete e;
    m_readyCallbackMap.erase(triggerId);
}

void Engine::appendToCacheReadyCallback(ConditionedProcessId triggerId, ConditionedProcessId triggerIdToAppend)
{
    EngineCacheElementPtr   e = m_readyCallbackMap[triggerId];
    TTObject    timeCondition = getTimeCondition(triggerId);
    TTValue     baton, newBaton;
    TTBoolean   found = false;
    
    e->object.get("baton", baton);
    
    newBaton.append(baton[0]);
    for (TTUInt32 i = 1; i < baton.size(); i++)
    {
        if (ConditionedProcessId(baton[i]) == triggerIdToAppend)
            found = true;
        
        newBaton.append(baton[i]);
    }
    
    if (!found)
        newBaton.append(triggerIdToAppend);
    
    e->object.set("baton", newBaton);
}

void Engine::removeFromCacheReadyCallback(ConditionedProcessId triggerId, ConditionedProcessId triggerIdToRemove)
{
    EngineCacheElementPtr   e = m_readyCallbackMap[triggerId];
    TTObject    timeCondition = getTimeCondition(triggerId);
    TTValue     baton, newBaton;
    
    e->object.get("baton", baton);
    
    newBaton.append(baton[0]);
    for (TTUInt32 i = 1; i < baton.size(); i++)
         if (ConditionedProcessId(baton[i]) != triggerIdToRemove)
             newBaton.append(baton[i]);
    
    e->object.set("baton", newBaton);
}

// Edition ////////////////////////////////////////////////////////////////////////

TimeProcessId Engine::addBox(TimeValue boxBeginPos, TimeValue boxLength, const std::string & name, TimeProcessId motherId)
{
    TTObject        startEvent, endEvent;
    TTObject        timeProcess, subScenario;
    TimeProcessId   boxId;
    TTAddress       address;
    TTValue         args, out;
    
    // create a time event for the start into the mother scenario
    getSubScenario(motherId).send("TimeEventCreate", boxBeginPos, out);
    startEvent = out[0];

    // create a time event for the end into the mother scenario
    getSubScenario(motherId).send("TimeEventCreate", boxBeginPos+boxLength, out);
    endEvent = out[0];
    
    // create a new automation time process into the mother scenario
    args = TTValue(TTSymbol("Automation"), startEvent, endEvent);
    getSubScenario(motherId).send("TimeProcessCreate", args, out);
    timeProcess = out[0];
    timeProcess.set("name", TTSymbol(name));
    
    // create a new sub scenario time process into the mother scenario
    args = TTValue(TTSymbol("Scenario"), startEvent, endEvent);
    getSubScenario(motherId).send("TimeProcessCreate", args, out);
    subScenario = out[0];
    
    // cache it and get an unique id for this process
    if (motherId == ROOT_BOX_ID)
        address = kTTAdrsRoot.appendAddress(TTAddress(name.data()));
    else
        address = getAddress(motherId).appendAddress(TTAddress(name.data()));
    
    boxId = cacheTimeProcess(timeProcess, address, subScenario);
    
    iscoreEngineDebug TTLogMessage("TimeProcess %ld created at %ld ms for a duration of %ld ms\n", boxId, boxBeginPos, boxLength);
    
	return boxId;
}

void Engine::removeBox(TimeProcessId boxId)
{
    TTObject    timeProcess, subScenario;
    TTObject    parentScenario;
    TTObject    startEvent, endEvent;
    TTValue     out;
    TTErr       err;
    
    // retreive the time process using the boxId
    timeProcess = getTimeProcess(boxId);
    subScenario = getSubScenario(boxId);
    
    // get the parent scenario
    timeProcess.get("container", out);
    parentScenario = out[0];
    
    // remove the time process from the cache
    uncacheTimeProcess(boxId);
    
    // release the time process from the mother scenario
    parentScenario.send("TimeProcessRelease", timeProcess, out);
    startEvent = out[0];
    endEvent = out[1];

    // release the sub scenario from the mother scenario
    parentScenario.send("TimeProcessRelease", subScenario, out);
    
    // release start event from the mother scenario
    err = parentScenario.send("TimeEventRelease", startEvent, out);
    if (err)
        iscoreEngineDebug TTLogMessage("Box %ld cannot release his start event\n", boxId);
    
    // release end event from the mother scenario
    err = parentScenario.send("TimeEventRelease", endEvent, out);
    if (err)
        iscoreEngineDebug TTLogMessage("Box %ld cannot release his end event\n", boxId);
}

IntervalId Engine::addTemporalRelation(TimeProcessId boxId1,
                                       TimeEventIndex controlPoint1,
                                       TimeProcessId boxId2,
                                       TimeEventIndex controlPoint2,
                                       TemporalRelationType /*type*/,
                                       vector<TimeProcessId>& movedBoxes)
{
    TTObject    timeProcess;
    TTObject    tp1, tp2;
    TTObject    startEvent, endEvent;
    TTObject    startScenario, endScenario;
    IntervalId  relationId;
    TTValue     args, out;
    TTErr       err;
    EngineCacheMapIterator  it;
    
    // get the events from the given box ids and pass them to the time process
    tp1 = getTimeProcess(boxId1);
    tp2 = getTimeProcess(boxId2);
    
    if (controlPoint1 == BEGIN_CONTROL_POINT_INDEX)
        tp1.get("startEvent", out);
    else
        tp1.get("endEvent", out);
    
    startEvent = out[0];
    
    if (controlPoint2 == BEGIN_CONTROL_POINT_INDEX)
        tp2.get("startEvent", out);
    else
        tp2.get("endEvent", out);
    
    endEvent = out[0];
    
    // get start and end events container
    startEvent.get("container", out);
    startScenario = out[0];
    
    endEvent.get("container", out);
    endScenario = out[0];
    
    // can't create a relation between to events of 2 differents scenarios
    if (startScenario != endScenario)
        return NO_ID;

    // create a new interval time process into the main scenario
    args = TTValue(TTSymbol("Interval"), startEvent, endEvent);
    err = startScenario.send("TimeProcessCreate", args, out);
    
    // an error can append if the start is after the end
    if (!err) {
        
        timeProcess = out[0];
        
        // set the time process rigid
        timeProcess.set("rigid", true);

        // cache it and get an unique id for this process
        relationId = cacheInterval(timeProcess);
    
        // return the entire timeProcessMap except the first process !!! (this is bad but it is like former engine)
        it = m_timeProcessMap.begin();
        it++;
        for (; it != m_timeProcessMap.end(); ++it)
            movedBoxes.push_back(it->first);
    
        return relationId;
    }
    else 
        return NO_ID;
}

void Engine::removeTemporalRelation(IntervalId relationId)
{
    TTObject    timeProcess;
    TTObject    startEvent, endEvent;
    TTObject    parentScenario;
    TTValue     out;
    TTErr       err;
    
    // retreive the interval using the relationId
    timeProcess = getInterval(relationId);
    
    // get the parent scenario
    timeProcess.get("container", out);
    parentScenario = out[0];
    
    // release the time process from the mother scenario
    parentScenario.send("TimeProcessRelease", timeProcess, out);
    startEvent = out[0];
    endEvent = out[1];
    
    // release start event from the mother scenario
    err = parentScenario.send("TimeEventRelease", startEvent, out);
    if (err)
        iscoreEngineDebug TTLogMessage("Relation %ld cannot release his start event\n", relationId);

    // release end event from the mother scenario
   err = parentScenario.send("TimeEventRelease", endEvent, out);
    if (err)
        iscoreEngineDebug TTLogMessage("Relation %ld cannot release his end event\n", relationId);

    // remove the interval from the cache
    uncacheInterval(relationId);
}

void Engine::changeTemporalRelationBounds(IntervalId relationId, BoundValue minBound, BoundValue maxBound, vector<TimeProcessId>& movedBoxes)
{
    TTObject    timeProcess = getInterval(relationId);
    TTUInt32    durationMin;
    TTUInt32    durationMax;
    TTValue     args, out;
    EngineCacheMapIterator  it;

    // filtering NO_BOUND (-1) and negative value because we use unsigned int
    if (minBound == NO_BOUND)
        durationMin = 0;
    else if (minBound < 0)
        durationMin = abs(minBound);
    else
        durationMin = minBound;
    
    if (maxBound == NO_BOUND)
        durationMax = 0;
    else if (maxBound < 0)
        durationMax = abs(maxBound);
    else
        durationMax = maxBound;
    
    // NOTE : sending 0 0 means the relation is not rigid
    // NOTE : it is also possible to use the "rigid" attribute to swicth between those two states
    args = TTValue(durationMin, durationMax);
    timeProcess.send("Limit", args, out);
    
    // return the entire timeProcessMap except the first process !!! (this is bad but it is like former engine)
    it = m_timeProcessMap.begin();
    it++;
    for (; it != m_timeProcessMap.end(); ++it)
        movedBoxes.push_back(it->first);
}

bool Engine::isTemporalRelationExisting(TimeProcessId boxId1, TimeEventIndex controlPoint1, TimeProcessId boxId2, TimeEventIndex controlPoint2)
{
    TTValue     v1, v2, v;
    IntervalId  found = NO_ID;
    EngineCacheMapIterator  it;
    
    // Get the events from the given box ids and pass them to the time process
    if (controlPoint1 == BEGIN_CONTROL_POINT_INDEX)
        getTimeProcess(boxId1).get("startEvent", v1);
    else
        getTimeProcess(boxId1).get("endEvent", v1);
    
    if (controlPoint2 == BEGIN_CONTROL_POINT_INDEX)
        getTimeProcess(boxId2).get("startEvent", v2);
    else
        getTimeProcess(boxId2).get("endEvent", v2);
    
    // Look into the interval map to retreive an interval with the same events
    for (it = m_intervalMap.begin(); it != m_intervalMap.end(); ++it) {
        
        it->second->object.get("startEvent", v);
        
        if (v == v1) {
            
            it->second->object.get("endEvent", v);
            
            if (v == v2) {
                found = it->first;
                break;
            }
        }
    }
    
	return found != NO_ID;
}

TimeProcessId Engine::getRelationFirstBoxId(IntervalId relationId)
{
    TTValue         out, v;
    TimeProcessId   found = NO_ID;
    EngineCacheMapIterator  it;
    
    // get the start event of the interval
	getInterval(relationId).get("startEvent", out);
    
    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        it->second->object.get("startEvent", v);
        
        if (out == v) {
            found = it->first;
            break;
        }
        
        it->second->object.get("endEvent", v);
        
        if (out == v) {
            found = it->first;
            break;
        }
    }
    
    return found;
}

TimeEventIndex Engine::getRelationFirstCtrlPointIndex(IntervalId relationId)
{
    TTValue         out, v;
    TimeEventIndex  ctrlPointId = NO_ID;
    EngineCacheMapIterator  it;
    
    // get the start event of the interval
	getInterval(relationId).get("startEvent", out);

    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        it->second->object.get("startEvent", v);
        
        if (out == v) {
            ctrlPointId = BEGIN_CONTROL_POINT_INDEX;
            break;
        }
        
        it->second->object.get("endEvent", v);
        
        if (out == v) {
            ctrlPointId = END_CONTROL_POINT_INDEX;
            break;
        }
    }
    
    return ctrlPointId;
}

TimeProcessId Engine::getRelationSecondBoxId(IntervalId relationId)
{
    TTValue         out, v;
    TimeProcessId   found = NO_ID;
    EngineCacheMapIterator  it;
    
    // Get the end event of the interval
	getInterval(relationId).get("endEvent", out);
    
    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        it->second->object.get("startEvent", v);
        
        if (out == v) {
            found = it->first;
            break;
        }
        
        it->second->object.get("endEvent", v);
        
        if (out == v) {
            found = it->first;
            break;
        }
    }
    
    return found;
}

TimeEventIndex Engine::getRelationSecondCtrlPointIndex(IntervalId relationId)
{
    TTValue         out, v;
    TimeEventIndex  ctrlPointId = NO_ID;
    EngineCacheMapIterator  it;
    
    // get the start event of the interval
	getInterval(relationId).get("endEvent", out);
    
    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        it->second->object.get("startEvent", v);
        
        if (out == v) {
            ctrlPointId = BEGIN_CONTROL_POINT_INDEX;
            break;
        }
        
        it->second->object.get("endEvent", v);
        
        if (out == v) {
            ctrlPointId = END_CONTROL_POINT_INDEX;
            break;
        }
    }
    
    return ctrlPointId;
}

BoundValue Engine::getRelationMinBound(IntervalId relationId)
{
    TTValue v;
    
    getInterval(relationId).get("durationMin", v);
    
    return v[0];
}

BoundValue Engine::getRelationMaxBound(IntervalId relationId)
{
    TTValue v;
    
    getInterval(relationId).get("durationMax", v);
    
    return v[0];
}

bool Engine::performBoxEditing(TimeProcessId boxId, TimeValue start, TimeValue end, vector<TimeProcessId>& movedBoxes)
{
    TTValue args, out;
    TTErr   err;
    EngineCacheMapIterator  it;
    
    args = TTValue(start, end);
    err = getTimeProcess(boxId).send("Move", args, out);

    // return the entire timeProcessMap except the first process !!! (this is bad but it is like former engine)
    it = m_timeProcessMap.begin();
    it++;
    for (; it != m_timeProcessMap.end(); ++it)
        movedBoxes.push_back(it->first);
    
    return !err;
}

std::string Engine::getBoxName(TimeProcessId boxId)
{
    TTSymbol    name;
    
	getTimeProcess(boxId).get("name", name);
    
    // format name replacing '_' by '.'
    TTString s_toParse = name.c_str();
    std::replace(s_toParse.begin(), s_toParse.end(), '_', ' ');
    
    return s_toParse.c_str();
}

unsigned int Engine::getBoxVerticalPosition(TimeProcessId boxId)
{
    TTValue v;
    
	getTimeProcess(boxId).get("verticalPosition", v);
    
    return v[0];
}

void Engine::setBoxVerticalPosition(TimeProcessId boxId, unsigned int newPosition)
{
	getTimeProcess(boxId).set("verticalPosition", TTUInt32(newPosition));
}

unsigned int Engine::getBoxVerticalSize(TimeProcessId boxId)
{
    TTValue v;
    
	getTimeProcess(boxId).get("verticalSize", v);
    
    return v[0];
}

void Engine::setBoxVerticalSize(TimeProcessId boxId, unsigned int newSize)
{
    getTimeProcess(boxId).set("verticalSize", TTUInt32(newSize));
}

QColor Engine::getBoxColor(TimeProcessId boxId)
{
    TTValue v;
    QColor  color;
    
	getTimeProcess(boxId).get("color", v);
    
    color = QColor(v[0], v[1], v[2]);
    
    return color;
}

void Engine::setBoxColor(TimeProcessId boxId, QColor newColor)
{
    TTValue v;
    
    v = newColor.red();
    v.append(newColor.green());
    v.append(newColor.blue());
    
	getTimeProcess(boxId).set("color", v);
}

void Engine::setBoxMuteState(TimeProcessId boxId, bool muteState)
{
    getTimeProcess(boxId).set("mute", muteState);
}

bool Engine::getBoxMuteState(TimeProcessId boxId)
{
    TTValue v;
    
    getTimeProcess(boxId).get("mute", v);
    
    return TTBoolean(v[0]);
}

void Engine::setBoxName(TimeProcessId boxId, string name)
{
    TTSymbol oldName, newName;
    
    getTimeProcess(boxId).get("name", oldName);
    
    // format name replacing ' ' by '_'
    TTString s_toParse = name;
    std::replace(s_toParse.begin(), s_toParse.end(), ' ', '_');
    
    newName = TTSymbol(s_toParse);
    
    // filter repetitions
    if (newName != oldName) {
        
        // rename the time process into the i-score application
        TTValue effectiveName, args(getTimeProcess(boxId), newName);
        m_iscore.send("ObjectRename", args, effectiveName);
        
        // rename the time process object with the effective registration name
        getTimeProcess(boxId).set("name", effectiveName);
    }
}

TimeValue Engine::getBoxBeginTime(TimeProcessId boxId)
{
    TTValue     v;
    TimeValue   startDate;

	getTimeProcess(boxId).get("startDate", v);
    startDate = v[0];
    
    return startDate;
}

TimeValue Engine::getBoxEndTime(TimeProcessId boxId)
{
    TTValue     v;
    TimeValue   endDate;

	getTimeProcess(boxId).get("endDate", v);
    endDate = v[0];
    
    return endDate;
}

TimeValue Engine::getBoxDuration(TimeProcessId boxId)
{
    TTValue v;
    
	getTimeProcess(boxId).get("duration", v);
    
    return v[0];
}

int Engine::getBoxNbCtrlPoints(TimeProcessId boxId)
{
    TTValue v;
    
	getTimeProcess(boxId).get("intermediateEvents", v);
    
    return v.size() + 2; // because there is always a start and an end event too
}

TimeEventIndex Engine::getBoxFirstCtrlPointIndex(TimeProcessId /*boxId*/)
{
	return BEGIN_CONTROL_POINT_INDEX;
}

TimeEventIndex Engine::getBoxLastCtrlPointIndex(TimeProcessId /*boxId*/)
{
	return END_CONTROL_POINT_INDEX;
}

void Engine::setCtrlPointMessagesToSend(TimeProcessId boxId, TimeEventIndex controlPointIndex, std::vector<std::string> messageToSend, bool /*muteState*/)
{
    TTValue     out;
    TTObject    event;
    TTObject    state;
    TTUInt32    i;

    // get the start or end event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        getTimeProcess(boxId).get("startEvent", out);
    else
        getTimeProcess(boxId).get("endEvent", out);
    
    event = out[0];
    
    // get the state of the event
    event.get("state", out);
    state = out[0];
    
    // clear the state
    state.send("Clear");
    
    // parse each incoming string into < directory:/address, value >
    for (i = 0; i < messageToSend.size(); i++) {
        
        TTValue v = TTString(messageToSend[i]);
        v.fromString();
        
        TTSymbol aSymbol = v[0];
		TTAddress anAddress = toTTAddress(aSymbol.string().data());
        v[0] = anAddress;
        
        // append a line to the state
        state.send("Append", v, out);
    }
    
    // flatten the state to increase the speed of the recall
    state.send("Flatten");
    
    // don't update curve for the root box because it is a Scenario and not an Automation
    if (boxId != ROOT_BOX_ID) {
    
        // update all curves (for automation process only)
        getTimeProcess(boxId).send("CurveUpdate");
    }
}

void Engine::getCtrlPointMessagesToSend(TimeProcessId boxId, TimeEventIndex controlPointIndex, std::vector<std::string>& messages)
{
    TTValue     out;
    TTObject    event;
    TTObject    state;
    TTBoolean   flattened;
    TTListPtr   lines = nullptr;
    TTDictionaryBasePtr aLine;
    TTAddress   address;
    std::string s;
    
    // get the start or end event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        getTimeProcess(boxId).get("startEvent", out);
    else
        getTimeProcess(boxId).get("endEvent", out);
    
    event = out[0];
    
    // get the state of the event
    event.get("state", out);
    state = out[0];
    
    // check if the state is flattened
    state.get("flattened", out);
    flattened = out[0];
    
    if (!flattened)
        state.send("Flatten");
    
    // get the state lines
    state.get("flattenedLines", out);
    lines = TTListPtr((TTPtr)out[0]);
    
    if (lines) {
        // edit each line address into a "directory/address value" string
        for (lines->begin(); lines->end(); lines->next()) {
            
            aLine = TTDictionaryBasePtr((TTPtr)lines->current()[0]);
            
            // get the target address
            aLine->lookup(kTTSym_target, out);
            address = out[0];
            
            // get value
            aLine->getValue(out);
            out.toString();
            
            // edit string
            s = toNetworkTreeAddress(address);
            s += " ";
            s += TTString(out[0]).c_str();
            
            messages.push_back(s);
        }
    }
}

void Engine::setCtrlPointMutingState(TimeProcessId boxId, TimeEventIndex controlPointIndex, bool mute)
{
    TTValue     out;
    TTObject    event;
    
    // get the start or end event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        getTimeProcess(boxId).get("startEvent", out);
    else
        getTimeProcess(boxId).get("endEvent", out);
    
    event = out[0];
    
    event.set("mute", mute);
}

bool Engine::getCtrlPointMutingState(TimeProcessId boxId, TimeEventIndex controlPointIndex)
{
    TTValue     out;
    TTObject    event;
    
    // get the start or end event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        getTimeProcess(boxId).get("startEvent", out);
    else
        getTimeProcess(boxId).get("endEvent", out);
    
    event = out[0];
    
    event.get("mute", out);
    
    return TTBoolean(out[0]);
}

//CURVES ////////////////////////////////////////////////////////////////////////////////////

void Engine::addCurve(TimeProcessId boxId, const std::string & address)
{
    TTValue out;
    
    // add the curve addresses into the automation time process
    getTimeProcess(boxId).send("CurveAdd", toTTAddress(address), out);
}

void Engine::removeCurve(TimeProcessId boxId, const std::string & address)
{
    TTValue out;
    
    // remove the curve addresses of the automation time process
    getTimeProcess(boxId).send("CurveRemove", toTTAddress(address), out);
}

void Engine::clearCurves(TimeProcessId boxId)
{
    // clear all the curves of the automation time process
    getTimeProcess(boxId).send("Clear");
}

std::vector<std::string> Engine::getCurvesAddress(TimeProcessId boxId)
{
    std::vector<std::string> curveAddresses;
    TTValue out;
    
    // get the curve addresses of the automation time process
    getTimeProcess(boxId).get("curveAddresses", out);
    
    // copy the addresses into the vector
    for (TTUInt32 i = 0; i < out.size(); i++)
        curveAddresses.push_back(toNetworkTreeAddress(out[i]));
    
	return curveAddresses;
}

void Engine::setCurveSampleRate(TimeProcessId boxId, const std::string & address, unsigned int nbSamplesBySec)
{
    TTObject    curve;
    TTValue     objects;
    TTUInt32    i;
    TTErr       err;
    
    // get curve object at address
    err = getTimeProcess(boxId).send("CurveGet", toTTAddress(address), objects);
    
    if (!err) {
        
        // set each indexed curve
        for (i = 0; i < objects.size(); i++) {
            
            curve = objects[i];
            
            curve.set("sampleRate", nbSamplesBySec);
        }
    }
}

unsigned int Engine::getCurveSampleRate(TimeProcessId boxId, const std::string & address)
{
    TTObject    curve;
    TTValue     out, objects;
    TTErr       err;
    
    // get curve object at address
    err = getTimeProcess(boxId).send("CurveGet", toTTAddress(address), objects);
    
    if (!err) {
        
        curve = objects[0];
        
        curve.get("sampleRate", out);
        
        return TTUInt32(out[0]);
    }
    
	return 0;
}

void Engine::setCurveRedundancy(TimeProcessId boxId, const std::string & address, bool redundancy)
{
    TTObject    curve;
    TTValue     objects;
    TTUInt32    i;
    TTErr       err;
    
    // get curve object at address
    err = getTimeProcess(boxId).send("CurveGet", toTTAddress(address), objects);
    
    if (!err) {
        
        // set each indexed curve
        for (i = 0; i < objects.size(); i++) {
            
            curve = objects[i];
            
            curve.set("redundancy", redundancy);
        }
    }
}

bool Engine::getCurveRedundancy(TimeProcessId boxId, const std::string & address)
{
    TTObject    curve;
    TTValue     out, objects;
    TTErr       err;
    
    // get curve object at address
    err = getTimeProcess(boxId).send("CurveGet", toTTAddress(address), objects);
    
    if (!err) {
        
        // get first indexed curve only
        curve = objects[0];
        
        curve.get("redundancy", out);
        
        return TTBoolean(out[0]);
    }
    
	return false;
}

void Engine::setCurveMuteState(TimeProcessId boxId, const std::string & address, bool muteState)
{
    TTObject    curve;
    TTValue     objects;
    TTUInt32    i;
    TTErr       err;
    
    // get curve object at address
    err = getTimeProcess(boxId).send("CurveGet", toTTAddress(address), objects);
    
    if (!err) {
        
        // set each indexed curve
        for (i = 0; i < objects.size(); i++) {
        
            curve = objects[i];
        
            curve.set("active", !muteState);
        }
    }
}

bool Engine::getCurveMuteState(TimeProcessId boxId, const std::string & address)
{
    TTObject    curve;
    TTValue     out, objects;
    TTErr       err;
    
    // get curve object at address
    err = getTimeProcess(boxId).send("CurveGet", toTTAddress(address), objects);
    
    if (!err) {
        
        // get first indexed curve only
        curve = objects[0];
        
        curve.get("active", out);
        
        return !TTBoolean(out[0]);
    }
    
	return false;
}

void Engine::setCurveRecording(TimeProcessId boxId, const std::string & address, bool record)
{
    TTValue args, out;
    
    // enable/disable recording
    args = TTValue(toTTAddress(address), record);
    
    getTimeProcess(boxId).send("CurveRecord", args, out);
}

bool Engine::setCurveSections(TimeProcessId boxId, std::string address, unsigned int /*argNb*/, const std::vector<float> & percent, const std::vector<float> & y, const std::vector<short> & /*sectionType*/, const std::vector<float> & coeff)
{
    TTObject    curve;
    TTValue     parameters, objects;
    TTUInt32    i, nbPoints = coeff.size();
    TTErr       err;
    
    // get curve object at address
    err = getTimeProcess(boxId).send("CurveGet", toTTAddress(address), objects);
    
    if (!err) {
        
        // edit parameters as : x1 y1 b1 x2 y2 b2
        parameters.resize(nbPoints * 3);
        
        // DEBUG
        // TTLogMessage("coeff[ ");
        
        for (i = 0; i < parameters.size(); i = i+3) {
            
            parameters[i] = TTFloat64(percent[i/3] / 100.);
            parameters[i+1] = TTFloat64(y[i/3]);
            
            // from the second point
            TTFloat64 c = coeff[i/3];
            
            // DEBUG
            // TTLogMessage("%f ", c);
            
            parameters[i+2] = c * c * c * c;
        }
        
        // DEBUG
        // TTLogMessage("]\n");
        
        // set first indexed curve only
        curve = objects[0];
        
        // set a curve parameters
        err = curve.set("functionParameters", parameters);
    }
    
    return err == kTTErrNone;
}

bool Engine::getCurveSections(TimeProcessId boxId, std::string address, unsigned int /*argNb*/,
                              std::vector<float> & percent,  std::vector<float> & y,  std::vector<short> & sectionType,  std::vector<float> & coeff)
{
    TTObject    curve;
    TTValue     parameters, objects;
    TTUInt32    i;
    TTErr       err;
    
    // get curve object at address
    err = getTimeProcess(boxId).send("CurveGet", toTTAddress(address), objects);
    
    if (!err) {
        
        // get first indexed curve only
        curve = objects[0];

        // get a curve parameters
        err = curve.get("functionParameters", parameters);
        
        if (!err) {

            // edit percent, y, sectionType and coeff from v : x1 y1 b1 x2 y2 b2 . . .
            for (i = 0; i < parameters.size(); i = i+3) {
                
                percent.push_back(TTFloat64(parameters[i]) * 100.);
                y.push_back(TTFloat64(parameters[i+1]));
                sectionType.push_back(1);
                coeff.push_back(sqrt(sqrt(TTFloat64(parameters[i+2]))));
            }
        }
    }
    
    return err == kTTErrNone;
}

bool Engine::getCurveValues(TimeProcessId boxId, const std::string & address, unsigned int /*argNb*/, std::vector<float>& result)
{
    TTObject    curve;
    TTValue     out, duration, curveValues;
    TTErr       err;
    
    // get curve object at address
    err = getTimeProcess(boxId).send("CurveGet", toTTAddress(address), out);

    if (!err) {
        
        // get first indexed curve only
        curve = out[0];
        
        // get time process duration
        getTimeProcess(boxId).get("duration", duration);
    
        // sample the curve
        err = curve.send("Sample", duration, curveValues);
    
        // copy the curveValues into the result vector
        for (TTUInt32 i = 0; i < curveValues.size(); i++)
            result.push_back(TTFloat64(curveValues[i]));
    }
    
	return err == kTTErrNone;
}

ConditionedProcessId Engine::addTriggerPoint(TimeProcessId containingBoxId, TimeEventIndex controlPointIndex)
{
    TTObject    timeEvent;
    TTObject    timeCondition;
    TTObject    parentScenario;
    ConditionedProcessId triggerId;
    TTValue     args, out;
    TTString    instance;
    TTAddress   address;
    
    // get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        getTimeProcess(containingBoxId).get("startEvent", out);
    else
        getTimeProcess(containingBoxId).get("endEvent", out);
    
    timeEvent = out[0];
    
    // get the parent scenario
    getTimeProcess(containingBoxId).get("container", out);
    parentScenario = out[0];
    
    // create a TTTimeCondition
    parentScenario.send("TimeConditionCreate", args, out);
    timeCondition = out[0];
    
    // add the event to the condition with no associated expression
    timeCondition.send("EventAdd", timeEvent, out);
    
    // we cache the time process and the event index instead of the event itself
    triggerId = cacheConditionedProcess(containingBoxId, controlPointIndex);
    
     // cache the condition and register it into the namespace
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        address = getAddress(containingBoxId).appendAddress(TTAddress("/start"));
    else
        address = getAddress(containingBoxId).appendAddress(TTAddress("/end"));
    
    cacheTimeCondition(triggerId, timeCondition, address);

    // note : see in setTriggerPointMessage to see how the expression associated to an event is edited
    
	return triggerId;
}

void Engine::removeTriggerPoint(ConditionedProcessId triggerId)
{
    // check existence before because they could have been destroyed in deleteCondition
    if (m_conditionedProcessMap.find(triggerId) == m_conditionedProcessMap.end())
        return;
    
    TTValue     out;
    TTObject    parentScenario;
    
    // get the parent scenario
    getTimeCondition(triggerId).get("container", out);
    parentScenario = out[0];
    
    // release the time condition
    parentScenario.send("TimeConditionRelease", getTimeCondition(triggerId), out);
    
    // uncache
    uncacheConditionedProcess(triggerId);
    
    uncacheTimeCondition(triggerId);
}

TimeConditionId Engine::createCondition(std::vector<ConditionedProcessId> triggerIds)
{
    // create an id for the condition and cache it (from ConditionedProcessId because it is mixed with timeConditionMap)
    std::vector<ConditionedProcessId>::iterator it = triggerIds.begin();
    TimeConditionId conditionId = m_nextConditionedProcessId++;
    cacheTimeCondition(conditionId, getTimeCondition(*it));
    appendToCacheReadyCallback(conditionId, *it);
    
    for(++it ; it != triggerIds.end() ; ++it)
        attachToCondition(conditionId, *it);
    
    return conditionId;
}

void Engine::attachToCondition(TimeConditionId conditionId, ConditionedProcessId triggerId)
{
    TimeEventIndex  idx = BEGIN_CONTROL_POINT_INDEX;                // Because a condition is always at the start of a box
    TTObject        timeEvent;
    TTObject        parentScenario;
    TTValue         out;
    
    getConditionedProcess(triggerId, idx).get("startEvent", out);
    timeEvent = out[0];

    // should be different conditions before the merge
    if (getTimeCondition(triggerId) != getTimeCondition(conditionId)) {

        // save the expression
        std::string expr = getTriggerPointMessage(triggerId);
        
        // get parent scenario
        timeEvent.get("container", out);
        parentScenario = out[0];

        // release the other time condition
        parentScenario.send("TimeConditionRelease", getTimeCondition(triggerId), out);

        // add the event to the chosen time condition with the saved expression
        getTimeCondition(conditionId).send("EventAdd", timeEvent, out);

        // modify the cache
        m_timeConditionMap[triggerId]->object = getTimeCondition(conditionId);
        m_conditionsMap[conditionId].push_back(triggerId);
        
        appendToCacheReadyCallback(conditionId, triggerId);
    }
}

void Engine::detachFromCondition(TimeConditionId conditionId, ConditionedProcessId triggerId)
{
    // check existence before because they could have been destroyed in removeTriggerPoint
    if (m_conditionsMap.find(conditionId) == m_conditionsMap.end())
        return;
    
    if (m_conditionedProcessMap.find(triggerId) == m_conditionedProcessMap.end())
        return;
    
    TimeEventIndex  idx = BEGIN_CONTROL_POINT_INDEX;                 // Because a condition is always at the start of a box
    TTObject        timeEvent;
    TTObject        parentScenario;
    TTValue         args, out;
    
    getConditionedProcess(triggerId, idx).get("startEvent", out);
    timeEvent = out[0];

    // should be the same condition before the separation
    if (getTimeCondition(triggerId) == getTimeCondition(conditionId)) {

        // save the expression
        std::string expr = getTriggerPointMessage(triggerId);
        
        // get parent scenario
        timeEvent.get("container", out);
        parentScenario = out[0];

        // remove the event from the chosen time condition
        getTimeCondition(conditionId).send("EventRemove", timeEvent, out);

        // create a new TTTimeCondition
        parentScenario.send("TimeConditionCreate", timeEvent, out);
        getTimeCondition(triggerId) = out[0];

        // add the event to the new condition with the saved expression
        getTimeCondition(conditionId).send("EventAdd", timeEvent, out);
        setTriggerPointMessage(triggerId, expr);

        // modify cache
        m_timeConditionMap[triggerId]->object = getTimeCondition(triggerId);
        
        // théo : it is bad to modify the triggerIds cache here as detachFromCondition is used into a for loop in deleteCondition
        //m_conditionsMap[conditionId].remove(triggerId);
        
        removeFromCacheReadyCallback(conditionId, triggerId);
    }
}

void Engine::deleteCondition(TimeConditionId conditionId)
{
    std::list<ConditionedProcessId> & triggerIds = m_conditionsMap[conditionId];
    std::list<ConditionedProcessId>::iterator it;
    TTObject    parentScenario;
    TTValue     out;

    for (it = triggerIds.begin() ; it != triggerIds.end() ; ++it)
        detachFromCondition(conditionId, *it);
    
    // get parent scenario
    getTimeCondition(conditionId).get("container", out);
    parentScenario = out[0];

    // release the condition
    parentScenario.send("TimeConditionRelease", getTimeCondition(conditionId), out);

    // uncache the condition
    uncacheTimeCondition(conditionId);
    m_conditionsMap.erase(conditionId);
}

void Engine::getConditionTriggerIds(TimeConditionId conditionId, std::vector<TimeProcessId>& triggerIds)
{
    triggerIds.assign(m_conditionsMap[conditionId].begin(), m_conditionsMap[conditionId].end());
}

void Engine::setConditionMessage(TimeConditionId conditionId, std::string disposeMessage)
{
    getTimeCondition(conditionId).set("disposeMessage", TTSymbol(disposeMessage));
}

std::string Engine::getConditionMessage(TimeConditionId conditionId)
{
    TTSymbol expr;
    
    getTimeCondition(conditionId).get("disposeMessage", expr);

    return expr.c_str();
}

void Engine::setTriggerPointMessage(ConditionedProcessId triggerId, std::string triggerMessage)
{
    TimeEventIndex  controlPointIndex;
    TTObject        conditionedProcess = getConditionedProcess(triggerId, controlPointIndex);
    TTObject        timeEvent;
    TTValue         args, out;
    
    // get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        conditionedProcess.get("startEvent", out);
    else
        conditionedProcess.get("endEvent", out);
    
    timeEvent = out[0];
    
    // edit the expression associated to this event
    args = TTValue(timeEvent, TTSymbol(triggerMessage));
    getTimeCondition(triggerId).send("EventExpression", args, out);
}

std::string Engine::getTriggerPointMessage(ConditionedProcessId triggerId)
{
    TimeEventIndex  controlPointIndex;
    TTObject        conditionedProcess = getConditionedProcess(triggerId, controlPointIndex);
    TTObject        timeEvent;
    TTSymbol        expression;
    TTValue         out;
    
    // get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        conditionedProcess.get("startEvent", out);
    else
        conditionedProcess.get("endEvent", out);
    
    timeEvent = out[0];
    
    // Get the expression associated to this event
    if (!getTimeCondition(triggerId).send("ExpressionFind", timeEvent, out)) {
        
        expression = out[0];
        return expression.c_str();
    }
    else {
        
        string empty;
        return empty;
    }
}

//!\ Crappy copy
void Engine::setTriggerPointDefault(ConditionedProcessId triggerId, bool dflt)
{
    TimeEventIndex  controlPointIndex;
    TTObject        conditionedProcess = getConditionedProcess(triggerId, controlPointIndex);
    TTObject        timeEvent;
    TTValue         args, out;

    // get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        conditionedProcess.get("startEvent", out);
    else
        conditionedProcess.get("endEvent", out);
    
    timeEvent = out[0];

    // edit the default comportment associated to this event
    args = TTValue(timeEvent, dflt);
    getTimeCondition(triggerId).send("EventDefault", args, out);
}

//!\ Crappy copy
bool Engine::getTriggerPointDefault(ConditionedProcessId triggerId)
{
    TimeEventIndex  controlPointIndex;
    TTObject        conditionedProcess = getConditionedProcess(triggerId, controlPointIndex);
    TTObject        timeEvent;
    TTValue         args, out;
    TTBoolean       dflt = NO;
    
    // get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        conditionedProcess.get("startEvent", out);
    else
        conditionedProcess.get("endEvent", out);
    
    timeEvent = out[0];

    // get the default comportment associated to this event
    if (!getTimeCondition(triggerId).send("DefaultFind", timeEvent, out))
        dflt = out[0];
        
    return dflt;
}

TimeProcessId Engine::getTriggerPointRelatedBoxId(ConditionedProcessId triggerId)
{
    TimeEventIndex  controlPointIndex;
    TTObject        conditionedProcess = getConditionedProcess(triggerId, controlPointIndex);
    EngineCacheMapIterator  it;
    TimeProcessId   id = NO_ID;
    
    // look for the time process id into the time process map
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        if (it->second->object == conditionedProcess) {
            
            id = it->first;
            break;
        }
    }

    return id;
}

TimeEventIndex Engine::getTriggerPointRelatedCtrlPointIndex(ConditionedProcessId triggerId)
{
    return m_conditionedProcessMap[triggerId]->index;;
}

void Engine::getBoxesId(vector<TimeProcessId>& boxesID)
{
    EngineCacheMapIterator it;
    
    boxesID.clear();
    
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it)
        boxesID.push_back(it->first);
}

void Engine::getRelationsId(vector<IntervalId>& relationsID)
{
    EngineCacheMapIterator it;
    
    relationsID.clear();
    
    for (it = m_intervalMap.begin(); it != m_intervalMap.end(); ++it)
        relationsID.push_back(it->first);
}

void Engine::getTriggersPointId(vector<ConditionedProcessId>& triggersID)
{
    EngineCacheMapIterator it;
    
    triggersID.clear();
    
    for (it = m_conditionedProcessMap.begin(); it != m_conditionedProcessMap.end(); ++it)
        triggersID.push_back(it->first);
}

void Engine::getConditionsId(vector<TimeConditionId>& conditionsID)
{
    std::map<TimeConditionId, std::list<ConditionedProcessId>>::iterator it;

    conditionsID.clear();

    for(it = m_conditionsMap.begin() ; it != m_conditionsMap.end() ; ++it) {
        conditionsID.push_back(it->first);
    }
}

void Engine::setViewZoom(QPointF zoom)
{
    TTValue v(zoom.x(), zoom.y());
    
    m_mainScenario.set("viewZoom", v);
}

QPointF Engine::getViewZoom()
{
    QPointF zoom;
    TTValue out;
    
    m_mainScenario.get("viewZoom", out);
    
    zoom = QPointF(out[0], out[1]);
    
    return zoom;
}

void Engine::setViewPosition(QPointF position)
{
    TTValue v(position.x(), position.y());

    m_mainScenario.set("viewPosition", v);
}

QPointF Engine::getViewPosition()
{
    QPointF position;
    TTValue out;
    
    m_mainScenario.get("viewPosition", out);
    
    position = QPointF(out[0], out[1]);
    
    return position;
}

//Execution ///////////////////////////////////////////////////////////
void Engine::setTimeOffset(TimeValue timeOffset, bool mute)
{
    TTValue out, args(timeOffset, mute);

    // set the time process at time offset (an optionaly mute the output)
    m_mainScenario.send("Goto", args, out);
}

TimeValue Engine::getTimeOffset()
{
    TTValue     out;
    TTObject    scheduler;
    
    // TODO : TTTimeProcess should extend Scheduler class
    m_mainScenario.get("scheduler", out);
    
    scheduler = out[0];
    
    scheduler.get("offset", out);
    
    return TimeValue(TTFloat64(out[0]));
}

bool Engine::play(TimeProcessId processId)
{
    TTLogMessage("***************************************\n");
    TTLogMessage("Engine::play\n");
    
    // play a time process triggering its start event (this will also play other time processes attached to the start event)
    TTBoolean success = !getTimeProcess(processId).send("Start");
  
    return success;
}

bool Engine::isPlaying(TimeProcessId processId)
{
    TTValue     out;
    TTObject    scheduler;
    
    // TODO : TTTimeProcess should extend Scheduler class
    // get the scheduler object
    getTimeProcess(processId).get("scheduler", out);
    scheduler = out[0];
    
    scheduler.get("running", out);
    
    return TTBoolean(out[0]);
}

bool Engine::stop(TimeProcessId processId)
{
    // stop a time process its end event (this will also stop other time processes attached to the end event)
    TTBoolean success = !getTimeProcess(processId).send("End");
  
    TTLogMessage("Engine::stopped\n");
    TTLogMessage("***************************************\n");
    
    return success;
}

void Engine::pause(bool pauseValue, TimeProcessId processId)
{
    if (pauseValue) {
        TTLogMessage("---------------------------------------\n");
        getTimeProcess(processId).send("Pause");
        if (processId != ROOT_BOX_ID)
            getSubScenario(processId).send("Pause");
    }
    else {
        TTLogMessage("+++++++++++++++++++++++++++++++++++++++\n");
        getTimeProcess(processId).send("Resume");
        if (processId != ROOT_BOX_ID)
            getSubScenario(processId).send("Resume");
    }
}

bool Engine::isPaused(TimeProcessId processId)
{
    TTValue     out;
    TTObject    scheduler;
    
    // TODO : TTTimeProcess should extend Scheduler class
    // get the scheduler object
    getTimeProcess(processId).get("scheduler", out);
    scheduler = out[0];
    
    scheduler.get("paused", out);
    
    return TTBoolean(out[0]);
}

TimeValue Engine::getCurrentExecutionDate(TimeProcessId processId)
{
    TTValue     out;
    TTUInt32    time;
    
    // TODO : TTTimeProcess should extend Scheduler class
    getTimeProcess(processId).get("date", out);
    time = TTFloat64(out[0]);
    
    return time;
}

float Engine::getCurrentExecutionPosition(TimeProcessId processId)
{
    TTValue     out;
    TTFloat64   position;
    
    // TODO : TTTimeProcess should extend Scheduler class
    getTimeProcess(processId).get("position", out);
    position = TTFloat64(out[0]);
    
    return position;
}

void Engine::setExecutionSpeedFactor(float factor, TimeProcessId processId)
{
    // TODO : TTTimeProcess should extend Scheduler class
    getTimeProcess(processId).set("speed", TTFloat64(factor));
    if (processId != ROOT_BOX_ID)
        getSubScenario(processId).set("speed", TTFloat64(factor));
}

float Engine::getExecutionSpeedFactor(TimeProcessId processId)
{
    TTValue out;
    
    // TODO : TTTimeProcess should extend Scheduler class
    getTimeProcess(processId).get("speed", out);
    
    return TTFloat64(out[0]);
}

void Engine::trigger(ConditionedProcessId triggerId)
{
    TimeEventIndex  controlPointIndex;
    TTObject        conditionnedProcess = getConditionedProcess(triggerId, controlPointIndex);
    TTObject        timeCondition;
    TTObject        timeEvent;
    TTValue         out;
    
    // get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        conditionnedProcess.get("startEvent", out);
    else
        conditionnedProcess.get("endEvent", out);
    
    timeEvent = out[0];
    
    // get event condition
    timeEvent.get("condition", out);
    timeCondition = out[0];
    
    // tell the condition to trigger this event (and dispose the others)
    timeCondition.send("Trigger", timeEvent, out);
}

void Engine::trigger(vector<ConditionedProcessId> triggerIds)
{
    vector<ConditionedProcessId>::iterator it;
    TTObject    lastTimeCondition;
    TTValue     events, out;
    
    // get all time events
    for (it = triggerIds.begin(); it != triggerIds.end(); ++it) {
        
        TimeEventIndex  controlPointIndex;
        TTObject        conditionedProcess = getConditionedProcess(*it, controlPointIndex);
        TTObject        timeEvent;
        TTObject        timeCondition;
        
        // get start or end time event
        if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
            conditionedProcess.get("startEvent", out);
        else
            conditionedProcess.get("endEvent", out);
        
        timeEvent = out[0];
        
        // get event condition
        timeEvent.get("condition", out);
        timeCondition = out[0];
        
        // check that all events are part of the same condition
        if (!lastTimeCondition.valid())
            lastTimeCondition = timeCondition;
        
        if (lastTimeCondition != timeCondition) {
            
            lastTimeCondition = TTObject();
            break;
        }
        
        events.append(timeEvent);
    }
    
    // if all events are part of the same time condition
    if (lastTimeCondition.valid())
        lastTimeCondition.send("Trigger", events, out);
}

void Engine::addNetworkDevice(const std::string & deviceName, const std::string & pluginToUse, const std::string & DeviceIp, const unsigned int & destinationPort, const unsigned int & receptionPort, const bool isInputPort, const std::string & stringPort)
{
    TTValue     args, none, out;
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication;
    TTObject    aProtocol;
    TTErr       err;
    
    // if the application doesn't already exist
    if (!accessApplication(applicationName)) {
        
        // create the application
        m_applicationManager.send("ApplicationInstantiateDistant", applicationName, out);
        anApplication = out[0];
        
        // check if the protocol has been loaded
        aProtocol = accessProtocol(TTSymbol(pluginToUse));
		if (aProtocol.valid()) {
            
            // stop the protocol
            aProtocol.send("Stop");
            
            // register the application to the protocol
            aProtocol.send("ApplicationRegister", applicationName, out);
            
            // select appliction before to set its parameter
            aProtocol.send("ApplicationSelect", applicationName, out);
            
            // prepare parameters depending of the protocol
            if (pluginToUse == "Minuit" || pluginToUse == "OSC") {
                
                args = TTValue(TTUInt16(destinationPort), TTUInt16(receptionPort));
                aProtocol.set("port", args);
                aProtocol.set("ip", TTSymbol(DeviceIp));
            }
            else if (pluginToUse == "MIDI") {
                
                if (isInputPort)
                    aProtocol.set("input", TTSymbol(stringPort));
                else
                    aProtocol.set("output", TTSymbol(stringPort));
            }
            
            // run the protocol
            err = aProtocol.send("Run", none, out);
            
            if (err) {
                out.toString();
                TTSymbol errorInfo = TTSymbol(TTString(out[0]));
                m_NetworkDeviceConnectionError(iscore, errorInfo);
            }
        }
        
        // set the priority, service, tags and rangeBounds attributes as a cached attributes
        args = kTTSym_priority;
        args.append(kTTSym_service);
        args.append(kTTSym_tags);
        args.append(kTTSym_rangeBounds);
        args.append(kTTSym_rangeClipmode);
        anApplication.set("cachedAttributes", args);
    }
}

void Engine::removeNetworkDevice(const std::string & deviceName)
{
    TTValue     v, out;
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication = accessApplication(applicationName);
    TTSymbol    protocolName;
    TTObject    aProtocol;
    
    // if the application exists
    if (anApplication.valid()) {
        
        // get the protocol name used by the application (we register distante application to 1 protocol only)
        protocolName = accessApplicationProtocolNames(applicationName)[0];
        aProtocol = accessProtocol(protocolName);
        
        // stop the protocol for this application
        aProtocol.send("Stop");
        
        // unregister the application to the protocol
        aProtocol.send("ApplicationUnregister", applicationName, out);
        
        // realease the application
        m_applicationManager.send("ApplicationRelease", applicationName, out);
    }
}

void Engine::sendNetworkMessage(const std::string & stringToSend)
{    
    TTValue out, data, v = TTString(stringToSend);
    v.fromString();
    
    TTSymbol aSymbol = v[0];
    TTAddress anAddress = toTTAddress(aSymbol.string().data());
    data.copyFrom(v, 1);
    
    m_sender.set(kTTSym_address, anAddress);
    m_sender.send(kTTSym_Send, data, out);
}

void Engine::getProtocolNames(std::vector<std::string>& allProtocolNames)
{
    TTValue     protocolNames;
    TTSymbol    name;
    
    // get all protocol names
    m_applicationManager.get("protocolNames", protocolNames);
    
    for (TTUInt8 i = 0; i < protocolNames.size(); i++) {
        
        name = protocolNames[i];
        allProtocolNames.push_back(name.c_str());
    }
}

void Engine::getNetworkDevicesName(std::vector<std::string>& allDeviceNames)
{
    TTValue     applicationNames;
    TTSymbol    name;
    
    // get all application name
    m_applicationManager.get("applicationNames", applicationNames);
    
    for (TTUInt8 i = 0; i < applicationNames.size(); i++) {
        
        name = applicationNames[i];
        
        // don't return iscore application name
        if (name == iscore)
            continue;
        
        allDeviceNames.push_back(name.c_str());
    }
}

bool Engine::isNetworkDeviceRequestable(const std::string deviceName)
{
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication = accessApplication(applicationName);
    TTSymbol    protocolName;
    TTObject    aProtocol;
    TTValue     v, out;
    TTBoolean   discover = NO;
    
    // if the application exists
    if (anApplication.valid()) {
        
        // get the protocol name used by the application (we register distante application to 1 protocol only)
        protocolName = accessApplicationProtocolNames(applicationName)[0];
        aProtocol = accessProtocol(protocolName);
        
        aProtocol.get("discover", out);
   
        discover = out[0];
    }
    
    return discover;
}

std::vector<std::string> Engine::requestNetworkSnapShot(const std::string & address)
{
    vector<string>      snapshot;
    TTAddress           anAddress = toTTAddress(address);
    TTNodeDirectoryPtr  aDirectory;
    TTNodePtr           aNode;
    TTObject            anObject;
    TTString            s;
    TTValue             v;
    
    // get the application directory
    aDirectory = accessApplicationDirectoryFrom(anAddress);
    
    if (aDirectory) {
    
        // get the node
        if (!aDirectory->getTTNode(anAddress, &aNode)) {
            
            // get object attributes
            anObject = aNode->getObject();
            
            if (anObject.valid()) {
                
                // in case of proxy data or mirror object
                if (anObject.name() == TTSymbol("Data") ||
                    (anObject.name() == kTTSym_Mirror && TTMirrorPtr(anObject.instance())->getName() == TTSymbol("Data")))
                {
                    // get the service attribute
                    anObject.get("service", v);
                    TTSymbol service = v[0];
                    
                    // ask the value only for parameter
                    if (service == kTTSym_parameter) {
                        
                        // get the value attribute
                        if (!anObject.get("value", v)) {
                            
                            v.toString();
                            s = TTString(v[0]);
                        
                            // append address value to the snapshot
                            snapshot.push_back(address + " " + s.data());
                        }
                    }
                }
            }
        }
    }
    
    return snapshot;
}

int
Engine::requestObjectAttributeValue(const std::string & address, const std::string & attribute, vector<string>& value)
{
    TTAddress           anAddress = toTTAddress(address);
    TTNodeDirectoryPtr  aDirectory;
    TTNodePtr           aNode;
    TTObject            anObject;
    TTString            s;
    TTValue             v;

    aDirectory = accessApplicationDirectoryFrom(anAddress);
    value.clear();

    if (!aDirectory)
        return 1;

    if (!aDirectory->getTTNode(anAddress, &aNode)) {

        // get object attributes
        anObject = aNode->getObject();

        if (anObject.valid()) {
            
            if (!anObject.get(TTSymbol(attribute), v)) {
                
                v.toString();
                s = TTString(v[0]);
                value.push_back(s.c_str());
                return 1;
            }
        }
    }
    return 0;
}

int
Engine::setObjectAttributeValue(const std::string & address, const std::string & attribute, std::string & value)
{
    TTAddress           anAddress = toTTAddress(address);
    TTNodeDirectoryPtr  aDirectory;
    TTNodePtr           aNode;
    TTObject            anObject;
    TTValue             v;
    
    aDirectory = accessApplicationDirectoryFrom(anAddress);
    value.clear();
    
    if (!aDirectory)
        return 1;
    
    if (!aDirectory->getTTNode(anAddress, &aNode)) {
        
        // set object attributes
        anObject = aNode->getObject();
        
        if (anObject.valid()) {
            
            v = TTString(value);
            v.fromString();
      
            if(!anObject.set(TTSymbol(attribute), v))
                return 1;
        }
    }
    return 0;
}

int
Engine::requestObjectType(const std::string & address, std::string & nodeType)
{
    TTNodeDirectoryPtr  aDirectory;
    TTAddress           anAddress = toTTAddress(address);
    TTSymbol            type;
    TTObject            anObject;
    TTNodePtr           aNode;

    nodeType = "none";
    aDirectory = accessApplicationDirectoryFrom(anAddress);

    if (!aDirectory)
        return 0;

    if (!aDirectory->getTTNode(anAddress, &aNode)) {

        anObject = aNode->getObject();
        
        if (anObject.valid()) {
            
            if (anObject.name() == kTTSym_Mirror)
                type = TTMirrorPtr(anObject.instance())->getName();
            else
                type = anObject.name();
            
            if (type != kTTSymEmpty) {
                
                nodeType = type.c_str();
                return 1;
            }
        }
    }
    return 0;
}

int
Engine::requestObjectPriority(const std::string &address, unsigned int &priority)
{
    TTNodeDirectoryPtr  aDirectory;
    TTAddress           anAddress = toTTAddress(address);
    TTObject            anObject;
    TTNodePtr           aNode;
    TTValue             v;

    aDirectory = accessApplicationDirectoryFrom(anAddress);

    if (!aDirectory)
        return 0;

    if (!aDirectory->getTTNode(anAddress, &aNode)) {

        anObject = aNode->getObject();

        if (anObject.valid()) {

            if (!anObject.get(kTTSym_priority, v))
                priority = v[0];
            else
                return 1;
        }
    }
    return 0;
}

int
Engine::requestObjectChildren(const std::string & address, vector<string>& children)
{
    TTNodeDirectoryPtr  aDirectory;
    TTAddress           anAddress = toTTAddress(address);
    TTNodePtr           aNode, childNode;
    TTList              nodeList;
    TTString            s;

    aDirectory = accessApplicationDirectoryFrom(anAddress);
    children.clear();

    if (!aDirectory)
        return 0;

    if (!aDirectory->getTTNode(anAddress, &aNode)) {

        aNode->getChildren(S_WILDCARD, S_WILDCARD, nodeList);
        nodeList.sort(&compareNodePriorityThenNameThenInstance);

        for (nodeList.begin(); nodeList.end(); nodeList.next()) {

            childNode = TTNodePtr(TTPtr(nodeList.current()[0]));

            s = childNode->getName().string();
            if (childNode->getInstance() != kTTSymEmpty) {
                s += ".";
                s += childNode->getInstance().string();
            }
            children.push_back(s.c_str());
        }
        return 1;
    }
    return 0;
}

bool
Engine::rebuildNetworkNamespace(const string &deviceName, const string &/*address*/)
{
    TTValue     v, none;
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication = accessApplication(applicationName);
    TTSymbol    protocolName;
    TTObject    aProtocol;
    
    // if the application exists
    if (anApplication.valid()) {
        
        // get the protocol name used by the application (we register distante application to 1 protocol only)
        protocolName = accessApplicationProtocolNames(applicationName)[0];
        
        // Minuit case : use discovery mechanism
        if (protocolName == TTSymbol("Minuit")) {
            
            // only if there is no namespace observation !
            if (m_namespaceObserver.valid())
                return 1;
            
            anApplication.send("DirectoryBuild");
            return 0;
        }
        // OSC case : reload the namespace from the last project file if exist
        else if (protocolName == TTSymbol("OSC")) {
            
            // can't refresh when learning
            if (getDeviceLearn(deviceName))
                return 1;
            
            if (m_namespaceFilesPath.find(deviceName) == m_namespaceFilesPath.end())
                return 1;
            
            TTSymbol namespaceFilePath = TTSymbol(m_namespaceFilesPath[deviceName]);
            
            // create a TTXmlHandler
            TTObject aXmlHandler(kTTSym_XmlHandler);
            
            // read the file to setup TTModularApplications
            aXmlHandler.set(kTTSym_object, anApplication);
            aXmlHandler.send(kTTSym_Read, namespaceFilePath, none);
            
            return 0;
        }
    }
    
    return 1;
}

bool
Engine::loadNetworkNamespace(const string &deviceName, const string &filepath)
{
    TTObject    anApplication = accessApplication(TTSymbol(deviceName));
    TTValue     out;
    TTErr       err;
    
    // create a TTXmlHandler
    TTObject aXmlHandler(kTTSym_XmlHandler);
    
    // read the file to setup an application
    aXmlHandler.set(kTTSym_object, anApplication);
    
    err = aXmlHandler.send(kTTSym_Read, TTSymbol(filepath), out);
    
    if (!err) {
        
        // stop OSC protocol
        m_applicationManager.send("ProtocolStop", TTSymbol("OSC"), out);
    
        // init the application
        anApplication.send("Init");
        
        // store the namespace file for this device
        m_namespaceFilesPath[deviceName] = filepath;
        
        // run OSC protocol
        m_applicationManager.send("ProtocolRun", TTSymbol("OSC"), out);
    }

    return err != kTTErrNone;
}

bool
Engine::getDeviceIntegerParameter(const string device, const string protocol, const string parameter, unsigned int &integer)
{
    TTSymbol    applicationName(device);
    TTObject    aProtocol = accessProtocol(TTSymbol(protocol));
    TTValue     out;
    TTErr       err;
    
    err = aProtocol.send("ApplicationSelect", applicationName, out);
    
    if (!err) {
        
        err = aProtocol.get(TTSymbol(parameter), out);
        
        if (!err) {
            
            integer = TTUInt32(out[0]);
            return 0;
        }
    }
    
    return 1;
}

bool
Engine::getDeviceIntegerVectorParameter(const string device, const string protocol, const string parameter, vector<int>& integerVect)
{
    TTSymbol    applicationName(device);
    TTObject    aProtocol = accessProtocol(TTSymbol(protocol));
    TTValue     out;
    TTErr       err;
    
    err = aProtocol.send("ApplicationSelect", applicationName, out);
    
    if (!err) {
        
        err = aProtocol.get(TTSymbol(parameter), out);
        
        if (!err) {
            
            for (TTUInt32 i = 0 ; i < out.size() ; i++)
                integerVect.push_back(TTUInt16(out[i]));
            
            return 0;
        }
    }
    
    return 1;
}

bool
Engine::getDeviceStringParameter(const string device, const string protocol, const string parameter, string &string)
{
    TTSymbol    applicationName(device);
    TTObject    aProtocol = accessProtocol(TTSymbol(protocol));
    TTValue     v, out;
    TTErr       err;
    
    err = aProtocol.send("ApplicationSelect", applicationName, out);
    
    if (!err) {
        
        err = aProtocol.get(TTSymbol(parameter), out);
        
        if (!err) {
            
            TTSymbol s = out[0];
            string = s.c_str();
            
            return 0;
        }
    }
    
    return 1;
}

bool
Engine::getDeviceProtocol(std::string deviceName, std::string &protocol)
{
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication = accessApplication(applicationName);
    TTSymbol    protocolName;
    
    // if the application exists
    if (anApplication.valid()) {
        
        // get the protocol name used by the application (we register distante application to 1 protocol only)
        protocolName = accessApplicationProtocolNames(applicationName)[0];
        protocol = protocolName.c_str();
        return 0;
    }
    return 1;
}

bool
Engine::setDeviceName(string deviceName, string newName)
{
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication = accessApplication(applicationName);
    TTSymbol    newApplicationName(newName);
    TTErr       err;
    
    err = anApplication.set("name", newApplicationName);
    
    return err != kTTErrNone;
}

bool
Engine::setDevicePort(string deviceName, int destinationPort, int receptionPort)
{
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication = accessApplication(applicationName);
    TTSymbol    protocolName;
    TTObject    aProtocol;
    TTValue     v, none, out;
    TTErr       err;
    
    // if the application exists
    if (anApplication.valid()) {
        
        // get the protocol name used by the application (we register distante application to 1 protocol only)
        protocolName = accessApplicationProtocolNames(applicationName)[0];
        aProtocol = accessProtocol(protocolName);
        
        err = aProtocol.send("ApplicationSelect", applicationName, out);
        
        if (!err) {
            
            aProtocol.send("Stop");
            
            v = TTValue(destinationPort, receptionPort);
            aProtocol.set("port", v);
            
            err = aProtocol.send("Run", none, out);
            
            if (err) {
                out.toString();
                TTSymbol errorInfo = TTSymbol(TTString(out[0]));
                m_NetworkDeviceConnectionError(applicationName, errorInfo);
            }
            
            if (!err)
                return 0;
        }
    }
    
    return 1;
}

bool
Engine::setDeviceLocalHost(string deviceName, string localHost)
{
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication = accessApplication(applicationName);
    TTSymbol    protocolName;
    TTObject    aProtocol;
    TTValue     v, none, out;
    TTErr       err;
    
    // if the application exists
    if (anApplication.valid()) {
        
        // get the protocol name used by the application (we register distante application to 1 protocol only)
        protocolName = accessApplicationProtocolNames(applicationName)[0];
        aProtocol = accessProtocol(protocolName);
        
        err = aProtocol.send("ApplicationSelect", applicationName, out);
        
        if (!err) {
            
            aProtocol.send("Stop");
            
            v = TTSymbol(localHost);
            aProtocol.set("ip", v);
            
            err = aProtocol.send("Run", none, out);
            
            if (err) {
                out.toString();
                TTSymbol errorInfo = TTSymbol(TTString(out[0]));
                m_NetworkDeviceConnectionError(applicationName, errorInfo);
            }
            
            if (!err)
                return 0;
        }
    }
    
    return 1;
}

bool
Engine::setDeviceProtocol(string deviceName, string protocol)
{
    string              oldProtocol,
                        localHost;
    unsigned int        port;

    //get protocol name
    if(getDeviceProtocol(deviceName,oldProtocol) != 0)
        return 1;

    //get ip
    if(getDeviceStringParameter(deviceName,oldProtocol,"ip",localHost) != 0)
        return 1;

    //get port
    if(getDeviceIntegerParameter(deviceName,oldProtocol,"port",port) != 0)
        return 1;

    removeNetworkDevice(deviceName);
    addNetworkDevice(deviceName,protocol,localHost,port);    

    return 0;
}

bool Engine::setDeviceLearn(std::string deviceName, bool newLearn)
{
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication = accessApplication(applicationName);
    
    // get the protocol name used by the application (we register distante application to 1 protocol only)
    TTSymbol    protocolName = accessApplicationProtocolNames(applicationName)[0];
    TTErr       err;
    TTValue     out;
    
    // depending of the protocol this application used
    if (protocolName == "Minuit")
        err = anApplication.send("DirectoryObserve", newLearn, out);
    else
        err = anApplication.set("learn", newLearn);
    
    // enable namespace observation
    if (newLearn && !m_namespaceObserver.valid()) {
        
        TTValue baton(TTPtr(this), applicationName);
        
        // create a TTCallback to observe when a node is created (using NamespaceCallback)
        m_namespaceObserver = TTObject("callback");
        
        m_namespaceObserver.set("baton", baton);
        m_namespaceObserver.set("function", TTPtr(&NamespaceCallback));
        
        accessApplicationDirectory(applicationName)->addObserverForNotifications(kTTAdrsRoot, m_namespaceObserver);
    }
    // disable namespace observation
    else if (!newLearn && m_namespaceObserver.valid()) {
        
        accessApplicationDirectory(applicationName)->removeObserverForNotifications(kTTAdrsRoot, m_namespaceObserver);
        
        m_namespaceObserver = TTObject();
    }
    
    return err != kTTErrNone;
}

bool Engine::getDeviceLearn(std::string deviceName)
{
    TTSymbol    applicationName(deviceName);
    TTObject    anApplication = accessApplication(applicationName);
    TTValue     v;
    
    anApplication.get("learn", v);
    
    return TTBoolean(v[0]);
}

bool Engine::protocolScan(const std::string & protocol, std::vector<std::string>&& scanOptions, std::vector<std::string>& scanResult)
{
    TTObject    aProtocol = (TTObjectBasePtr)accessProtocol(TTSymbol(protocol));
    TTValue     args, out;

    // prepare scan options value
    for (const auto& opt : scanOptions)
            args.append(TTSymbol(opt));
    
    TTErr err = aProtocol.send("Scan", args, out);
    
    for (TTUInt8 i = 0; i < out.size(); i++) {
        
        TTSymbol scanInfo = out[i];

        scanResult.push_back(scanInfo.c_str());
    }
    
    return err == kTTErrNone;
}

int Engine::requestNetworkNamespace(const std::string & address, std::string & nodeType, vector<string>& nodes, vector<string>& leaves, vector<string>& attributs, vector<string>& attributsValue)
{
    TTAddress           anAddress = toTTAddress(address);
    TTSymbol            type, service;
    TTNodeDirectoryPtr  aDirectory;
    TTNodePtr           aNode, childNode;
    TTMirrorPtr         aMirror;
    TTList              nodeList;
    TTString            s;
    TTValue             v;
    
    // get the application directory
    aDirectory = accessApplicationDirectoryFrom(anAddress);
    
    if (!aDirectory)
        return 0;
    
    // explore the directory at this address
    // notice the tree is already built (see in initModular)
    if (!aDirectory->getTTNode(anAddress, &aNode)) {
        
        // get object attributes
        aMirror = TTMirrorPtr(aNode->getObject().instance());
        if (aMirror) {
            
            type = aMirror->getName();
            
            if (type != kTTSymEmpty)
                nodeType = type.c_str();
            else
                nodeType = "none";
            
            if (type == TTSymbol("Data")) {
                
                // append a service attribute
                attributs.push_back("service");
                
                // get the value of the service attribute
                if (!aMirror->getAttributeValue(TTSymbol("service"), v))
                    service = v[0];
                else
                    service = TTSymbol("error");
                
                v.toString();
                s = TTString(v[0]);
                attributsValue.push_back(s.c_str());
                
                // for parameter data : ask the value
                if (service == kTTSym_parameter) {
                
                    // append the value attribute
                    attributs.push_back("value");
                
                    // get the value attribute
                    aMirror->getAttributeValue(TTSymbol("value"), v);
                    v.toString();
                    s = TTString(v[0]);
                    attributsValue.push_back(s.c_str());
                }
                
                // for any data : ask the rangeBounds
                // append the value attribute
                attributs.push_back("rangeBounds");
                
                // get the value of the rangeBounds attribute
                aMirror->getAttributeValue(TTSymbol("rangeBounds"), v);
                    
                v.toString();
                s = TTString(v[0]);
                attributsValue.push_back(s.c_str());
            }
            else if (type == TTSymbol("Container")) {
                
                // append a service attribute
                attributs.push_back("service");
                
                // get the value of the service attribute
                if (!aMirror->getAttributeValue(TTSymbol("service"), v))
                    service = v[0];
                else
                    service = TTSymbol("error");
                
                v.toString();
                s = TTString(v[0]);
                attributsValue.push_back(s.c_str());
            }
        }
        
        // TODO : get attributes value
        
        // get children
        aNode->getChildren(S_WILDCARD, S_WILDCARD, nodeList);
        
        // sort children
        nodeList.sort(&compareNodePriorityThenNameThenInstance);

        // sort children in leaves and nodes
        for (nodeList.begin(); nodeList.end(); nodeList.next()) {
            
            childNode = TTNodePtr(TTPtr(nodeList.current()[0]));
            
            // prepare name.instance to store
            s = childNode->getName().string();
            if (childNode->getInstance() != kTTSymEmpty) {
                s += ".";
                s += childNode->getInstance().string();
            }
            
            // depending on the mirror object type
            aMirror = TTMirrorPtr(childNode->getObject().instance());
            if (aMirror) {
                
                type = aMirror->getName();
                
                if (type == TTSymbol("Data"))
                    leaves.push_back(s.c_str());
                else
                    nodes.push_back(s.c_str());
                    
            }
            else
                nodes.push_back(s.c_str());
        }
        
        return 1;
    }
    
    return 0;
}

int Engine::appendToNetWorkNamespace(const std::string & address, const std::string & service, const std::string & type, const std::string & priority, const std::string & description, const std::string & range, const std::string & clipmode, const std::string & tags)
{
    TTAddress           anAddress = toTTAddress(address);
    TTObject            anApplication = accessApplication(anAddress.getDirectory());
    TTNodeDirectoryPtr  aDirectory;
    TTObject            anObject;
    TTString            s;
    TTValue             v, out;
    
    // get the application directory
    aDirectory = accessApplicationDirectoryFrom(anAddress);
    
    if (!aDirectory)
        return 0;
    
    // don't register anything into the local directory
    if (aDirectory == accessApplicationLocalDirectory)
        return 0;
    
    // create a proxy data
    v = TTValue(anAddress, TTSymbol(service));
    if (!anApplication.send("ProxyDataInstantiate", v, out)) {
        
        anObject = out[0];
        
        anObject.set("type", TTSymbol(type));
        
        v = TTString(priority);
        v.fromString();
        anObject.set("priority", v);
        
        anObject.set("description", TTSymbol(description));
        
        v = TTString(range);
        v.fromString();
        anObject.set("rangeBounds", v);
        
        anObject.set("rangeClipmode", TTSymbol(clipmode));
        
        v = TTString(tags);
        v.fromString();
        anObject.set("tags", v);
        
        // initialize the value with a default 0. value
        anObject.set("valueDefault", 0.);
        
        anObject.send("Init");
        
        return 1;
    }
    
    return 0;
}

int Engine::removeFromNetWorkNamespace(const std::string & address)
{
    TTAddress           anAddress = toTTAddress(address);
    TTNodeDirectoryPtr  aDirectory;
    TTNodePtr           aNode;
    TTObject            anObject;
    
    // get the application directory
    aDirectory = accessApplicationDirectoryFrom(anAddress);
    
    if (!aDirectory)
        return 0;
    
    // explore the directory at this address
    // notice the tree is already built (see in initModular)
    if (!aDirectory->getTTNode(anAddress, &aNode)) {
        
        aDirectory->TTNodeRemove(anAddress);
        
        return 1;
    }
    
    return 0;
}


// LOAD AND STORE

int Engine::store(std::string filepath)
{
    TTValue v, none;
    
    m_lastProjectFilePath = TTSymbol(filepath);
    
    // Create a TTXmlHandler
    TTObject aXmlHandler(kTTSym_XmlHandler);
    
    // Pass the application manager and the main scenario object
    v = TTValue(m_applicationManager, m_mainScenario);
    aXmlHandler.set(kTTSym_object, v);
    
    // Write
    TTErr err = aXmlHandler.send(kTTSym_Write, m_lastProjectFilePath, none);
    
    return err == kTTErrNone;
}

int Engine::load(std::string filepath)
{
    TTValue out;
    TTErr   err;
    
    // Check that all Engine caches have been properly cleared before
    if (m_timeProcessMap.size() > 1)
        TTLogMessage("Engine::load : m_timeProcessMap not empty before the loading\n");
    
    if (!m_intervalMap.empty())
        TTLogMessage("Engine::load : m_intervalMap not empty before the loading\n");
    
    if (!m_conditionedProcessMap.empty())
        TTLogMessage("Engine::load : m_conditionedProcessMap not empty before the loading\n");
    
    if (!m_timeConditionMap.empty())
        TTLogMessage("Engine::load : m_timeConditionMap not empty before the loading\n");
    
    m_lastProjectFilePath = TTSymbol(filepath);
    
    // Create a TTXmlHandler
    TTObject aXmlHandler(kTTSym_XmlHandler);
    
    // Read the file to setup m_applicationManager
    aXmlHandler.set(kTTSym_object, m_applicationManager);
    err = aXmlHandler.send(kTTSym_Read, m_lastProjectFilePath, out);
    
    if (!err) {
        
        // Read the file to setup m_mainScenario
        aXmlHandler.set(kTTSym_object, m_mainScenario);
        err = aXmlHandler.send(kTTSym_Read, m_lastProjectFilePath, out);
        
        if (!err) {
            
            // Rebuild all the EngineCacheMaps from the main scenario content
            buildEngineCaches(m_mainScenario, kTTAdrsRoot);
        }
    }
    
    return err == kTTErrNone;
}

void Engine::buildEngineCaches(TTObject& scenario, TTAddress& scenarioAddress)
{
    TTValue             v, objects, none;
    TTObject            timeProcess;
    TTObject            timeEvent;
    TTObject            timeCondition;
    TTObject            empty;
    TTSymbol            name;
    TimeProcessId       timeProcessId;
    IntervalId          relationId;
    ConditionedProcessId triggerId;
    TimeConditionId     timeConditionId;
    
    // temporary map from TTTimeConditionPtr to TimeConditionId
    std::map<TTObjectBasePtr, TimeConditionId> TTCondToID;

    // get all TTTimeConditions
    scenario.get("timeConditions", objects);

    // for all time conditions
    for (TTUInt32 i = 0 ; i < objects.size() ; ++i) {

        timeCondition = objects[i];

        // check if it's a condition for i-score (2-plus events)
        timeCondition.get("events", v);
        if (v.size() >= 2) {

            // get a unique ID for the condition
            timeConditionId = m_nextConditionedProcessId++;

            // cache it but don't register it
            cacheTimeCondition(timeConditionId, timeCondition);

            // fill the temporary map
            TTCondToID[timeCondition.instance()] = timeConditionId;
        }
    }
    
    // get all TTTimeProcesses
    scenario.get("timeProcesses", objects);
    
    // for all time process
    for (TTUInt32 i = 0; i < objects.size(); i++) {
        
        timeProcess = objects[i];
        
        // for each Automation process
        if (timeProcess.name() == TTSymbol("Automation")) {
            
            timeProcess.get(kTTSym_name, name);
            
            // cache it and get an unique id for this process
            TTAddress address = scenarioAddress.appendAddress(TTAddress(name));
            timeProcessId = cacheTimeProcess(timeProcess, address, empty);
            
            // if the Start event of the Automation process is conditioned
            timeProcess.get("startEvent", v);
            timeEvent = v[0];

            timeEvent.get("condition", v);
            timeCondition = v[0];
            
            if (timeCondition.valid()) {
            
                // cache the time process and the event index instead of the event itself
                triggerId = cacheConditionedProcess(timeProcessId, BEGIN_CONTROL_POINT_INDEX);

                // if it is a condition for i-score
                std::map<TTObjectBasePtr, TimeConditionId>::iterator it = TTCondToID.find(timeCondition.instance());
                if (it != TTCondToID.end()) {
                    
                    // cache the condition without registering it into the namespace
                    cacheTimeCondition(triggerId, timeCondition);

                    // add it in the conditions map
                    m_conditionsMap[it->second].push_back(triggerId);
                    
                    appendToCacheReadyCallback(it->second, triggerId);
                }
                else {
                    
                    // cache the condition and register it into the namespace
                    address = getAddress(timeProcessId).appendAddress(TTAddress("/start"));
                    cacheTimeCondition(triggerId, timeCondition, address);
                }
            }

            // if the End event of the Automation process is conditioned
            timeProcess.get("endEvent", v);
            timeEvent = v[0];
            
            timeEvent.get("condition", v);
            timeCondition = v[0];
            
            if (timeCondition.valid()) {
                
                // cache the time process and the event index instead of the event itself
                triggerId = cacheConditionedProcess(timeProcessId, END_CONTROL_POINT_INDEX);
            
                // note : for the moment we consider it is not possible to create conditions at the end of boxes
                
                // cache the condition and register it into the namespace
                address = getAddress(timeProcessId).appendAddress(TTAddress("/end"));
                cacheTimeCondition(triggerId, timeCondition, address);
            }
        }
        
        // for each Interval process
        else if (timeProcess.name() == TTSymbol("Interval")) {
            
            // Cache it and get an unique id for this process
            relationId = cacheInterval(timeProcess);
        }
        
        // for each Scenario process
        else if (timeProcess.name() == TTSymbol("Scenario")) {
            
            // get end and start events
            TTObject startSubScenario, endSubScenario;
            
            timeProcess.get("startEvent", v);
            startSubScenario = v[0];
            
            timeProcess.get("endEvent", v);
            endSubScenario = v[0];

            // retreive the time process with the same end and start events
            EngineCacheMapIterator it;
            TTAddress address;
            
            for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
                
                TTObject  start, end;
                
                it->second->object.get("startEvent", v);
                start = v[0];
                
                it->second->object.get("endEvent", v);
                end = v[0];
                
                // set the scenario as the subScenario related to this time process
                if (start == startSubScenario && end == endSubScenario) {
                    
                    it->second->subScenario = timeProcess;
                    address = it->second->address;
                    break;
                }
            }
            
            // Rebuild all the EngineCacheMaps from the sub scenario content
            buildEngineCaches(timeProcess, address);
        }
    }
}

// NETWORK
void Engine::print() {
#ifdef TODO_ENGINE
	m_executionMachine->getPetriNet()->print();
#endif
}

void Engine::printExecutionInLinuxConsole()
{
#ifdef TODO_ENGINE    
	std::vector<TimeProcessId> boxesId;
    
	getBoxesId(boxesId);
    
	bool mustDisplay = true;
    
	while(isPlaying()){
        
		if ((getCurrentExecutionTime()/60)%2 == 0) {
			if (mustDisplay) {
				std::system("clear");
				for (unsigned int i = 0; i < boxesId.size(); ++i) {
					unsigned int processPercent;
                    
					processPercent = getProcessPosition(boxesId[i]);
                    
					if ((processPercent > 0) && (processPercent < 99)) {
						std::cout << "[*";
                        
						for (unsigned j = 10; j <= processPercent; j += 10) {
							std::cout << "*";
						}
                        
						for (unsigned j = processPercent + 1; j <= 90; j += 10) {
							std::cout << ".";
						}
                        
						std::cout << "] -> (" << i << ") (" << processPercent << ")" << std::endl;
					}
				}
				mustDisplay = false;
			}
		} else {
			mustDisplay = true;
		}
		usleep(50);
	}
#endif
}

#if 0
#pragma mark -
#pragma mark Callback Methods
#endif

void TimeEventStatusAttributeCallback(const TTValue& baton, const TTValue& value)
{
    EnginePtr               engine;
    ConditionedProcessId    triggerId;
    TTObject                event, condition;
    TTValue                 v;
    TTSymbol                status;
    TTBoolean               ready;
	
	// unpack baton (engine, triggerId)
	engine = EnginePtr((TTPtr)baton[0]);
    triggerId = ConditionedProcessId(baton[1]);
	
	// Unpack data (event)
	event = value[0];
    
    // get status
    event.get("status", v);
    status = v[0];
    
    // get event condition
    event.get("condition", v);
    condition = v[0];
    
    if (condition.valid()) {
    
        // get condition ready state
        condition.get("ready", ready);
        
        iscoreEngineDebug {
            
            if (ready)
                TTLogMessage("TriggerPoint %ld is part of a ready condition\n", triggerId);
            else
                TTLogMessage("TriggerPoint %ld is part of a none ready condition\n", triggerId);
        }
    }
    else
        ready = true;
    
    if (engine->m_TimeEventStatusAttributeCallback != nullptr) {
        
        if (status == kTTSym_eventWaiting) {
            engine->m_TimeEventStatusAttributeCallback(triggerId, false);
        }
        else if (status == kTTSym_eventPending) {
            engine->m_TimeEventStatusAttributeCallback(triggerId, ready);
        }
        else if (status == kTTSym_eventHappened) {
            engine->m_TimeEventStatusAttributeCallback(triggerId, false);
        }
        else if (status == kTTSym_eventDisposed) {
            engine->m_TimeEventStatusAttributeCallback(triggerId, false);
        }
        
        iscoreEngineDebug {
            
            if (status == kTTSym_eventWaiting) {
                TTLogMessage("TriggerPoint %ld is waiting\n", triggerId);
            }
            else if (status == kTTSym_eventPending) {
                TTLogMessage("TriggerPoint %ld is pending\n", triggerId);
            }
            else if (status == kTTSym_eventHappened) {
                TTLogMessage("TriggerPoint %ld happened\n", triggerId);
            }
            else if (status == kTTSym_eventDisposed) {
                TTLogMessage("TriggerPoint %ld is disposed\n", triggerId);
            }
        }
    }
}

void TimeConditionReadyAttributeCallback(const TTValue& baton, const TTValue& value)
{
    EnginePtr               engine;
    ConditionedProcessId    triggerId;
    TTObject                condition;
    TTBoolean               ready = value[0];
	
	// unpack baton (engine, triggerId)
	engine = EnginePtr((TTPtr)baton[0]);
    
    // only update for condition with more than one case
    if (baton.size() > 2)
    {
        for (TTUInt32 i = 1; i < baton.size(); i++)
        {
            triggerId = ConditionedProcessId(baton[i]);
            engine->m_TimeEventStatusAttributeCallback(triggerId, ready);
            
            iscoreEngineDebug {
                
                if (ready)
                    TTLogMessage("TriggerPoint %ld is part of a ready condition\n", triggerId);
                else
                    TTLogMessage("TriggerPoint %ld is part of a none ready condition\n", triggerId);
            }
        }
    }
}

void TimeProcessStartCallback(const TTValue& baton, const TTValue& /*value*/)
{
    EnginePtr       engine;
    TimeProcessId   boxId;
	
	// unpack baton (engine, boxId)
	engine = EnginePtr((TTPtr)baton[0]);
    boxId = TTUInt32(baton[1]);
    
    iscoreEngineDebug 
        TTLogMessage("TimeProcess %ld starts at %ld ms\n", boxId, engine->getCurrentExecutionDate());
        
    if (engine->m_TimeProcessSchedulerRunningAttributeCallback != nullptr)
        engine->m_TimeProcessSchedulerRunningAttributeCallback(boxId, YES);

}

void TimeProcessEndCallback(const TTValue& baton, const TTValue& /*value*/)
{
    EnginePtr       engine;
    TimeProcessId   boxId;
	
	// unpack baton (engine, boxId)
	engine = EnginePtr((TTPtr)baton[0]);
    boxId = TTUInt32(baton[1]);
    
    iscoreEngineDebug
        TTLogMessage("TimeProcess %ld ends at %ld ms\n", boxId, engine->getCurrentExecutionDate());
    
    // update all process running state too
    if (engine->m_TimeProcessSchedulerRunningAttributeCallback != nullptr)
        engine->m_TimeProcessSchedulerRunningAttributeCallback(boxId, NO);
}

void NamespaceCallback(const TTValue& baton, const TTValue& value)
{
    EnginePtr   engine;
    TTSymbol    applicationName;
	TTUInt8     flag;
	
	// unpack baton (engine, applicationName)
	engine = EnginePtr((TTPtr)baton[0]);
    applicationName = baton[1];
    
    // Unpack value (anAddress, aNode, flag, anObserver)
	flag = value[2];
    
    if (flag == kAddressCreated)
        engine->m_NetworkDeviceNamespaceCallback(applicationName);
}

TTAddress Engine::toTTAddress(string networktreeAddress)
{
    TTSymbol            temp(networktreeAddress);
    TTAddress           address(temp);
    TTAddress           applicationName, anAddress;
    TTString            s;
    
    // split the address to get application name and then an address
    address.splitAt(0, applicationName, anAddress);
    
    // edit applicationName:/anAddress
    s = applicationName.string();
    s += S_DIRECTORY.string();
    
    if (anAddress != kTTAdrsEmpty)
        s += anAddress.string();
    
    return TTAddress(s);
}

std::string Engine::toNetworkTreeAddress(TTAddress aTTAddress)
{
    std::string s = aTTAddress.getDirectory().string().c_str();
    s += aTTAddress.normalize().string();
    
    return s;
}
