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

Engine::Engine(void(*timeEventReadyAttributeCallback)(ConditionedProcessId, bool),
               void(*timeProcessSchedulerRunningAttributeCallback)(TimeProcessId, bool),
               void(*transportDataValueCallback)(TTSymbol&, const TTValue&))
{
    m_TimeEventReadyAttributeCallback = timeEventReadyAttributeCallback;
    m_TimeProcessSchedulerRunningAttributeCallback = timeProcessSchedulerRunningAttributeCallback;
    m_TransportDataValueCallback = transportDataValueCallback;
    
    m_nextTimeProcessId = 1;
    m_nextIntervalId = 1;
    m_nextConditionedProcessId = 1;
    
    m_mainScenario = NULL;
    
    initModular();
    initScore();
}

void Engine::initModular()
{
    TTErr           err;
    TTValue         args, v;
    TTString        applicationName = "i-score";                  // TODO : declare as global variable
    TTObjectBasePtr iscore;                                       // TODO : declare as global variable
    TTString        configFile = "/usr/local/include/IScore/i-scoreConfiguration.xml";
    TTHashPtr       hashParameters;
    
    // this initializes the Modular framework and loads protocol plugins (in /usr/local/jamoma/extensions folder)
    TTModularInit();
    
    // create a local application named i-score
    TTModularCreateLocalApplication(applicationName, configFile);
    
    // get i-score application
    iscore = getLocalApplication;
    
    // set the application in debug mode
    iscore->setAttributeValue(TTSymbol("debug"), YES);
    
    // create TTData for tranport service and expose them
    
    // Play
    TTValuePtr batonPlay = new TTValue(TTPtr(this));
    batonPlay->append(TTSymbol("Play"));
    TTModularCreateData(kTTSym_message, batonPlay, &TransportDataValueCallback, &m_dataPlay);
    
    m_dataPlay->setAttributeValue(kTTSym_type, kTTSym_none);
    m_dataPlay->setAttributeValue(kTTSym_description, TTSymbol("start i-score execution"));
    
    TTModularRegisterObject(TTAddress("/Transport/Play"), m_dataPlay);
    
    // Stop
    TTValuePtr batonStop = new TTValue(TTPtr(this));
    batonStop->append(TTSymbol("Stop"));
    TTModularCreateData(kTTSym_message, batonStop, &TransportDataValueCallback, &m_dataStop);
    
    m_dataStop->setAttributeValue(kTTSym_type, kTTSym_none);
    m_dataStop->setAttributeValue(kTTSym_description, TTSymbol("stop i-score execution"));
    
    TTModularRegisterObject(TTAddress("/Transport/Stop"), m_dataStop);
    
    // Pause
    TTValuePtr batonPause = new TTValue(TTPtr(this));
    batonPause->append(TTSymbol("Pause"));
    TTModularCreateData(kTTSym_message, batonPause, &TransportDataValueCallback, &m_dataPause);
    
    m_dataPause->setAttributeValue(kTTSym_type, kTTSym_none);
    m_dataPause->setAttributeValue(kTTSym_description, TTSymbol("pause i-score execution"));
    
    TTModularRegisterObject(TTAddress("/Transport/Pause"), m_dataPause);
    
    // Rewind
    TTValuePtr batonRewind = new TTValue(TTPtr(this));
    batonRewind->append(TTSymbol("Rewind"));
    TTModularCreateData(kTTSym_message, batonRewind, &TransportDataValueCallback, &m_dataRewind);
    
    m_dataRewind->setAttributeValue(kTTSym_type, kTTSym_none);
    m_dataRewind->setAttributeValue(kTTSym_description, TTSymbol("return to the beginning of the scenario"));
    
    TTModularRegisterObject(TTAddress("/Transport/Rewind"), m_dataRewind);
    
    // StartPoint
    TTValuePtr batonStartPoint = new TTValue(TTPtr(this));
    batonStartPoint->append(TTSymbol("StartPoint"));
    TTModularCreateData(kTTSym_message, batonStartPoint, &TransportDataValueCallback, &m_dataStartPoint);
    
    m_dataStartPoint->setAttributeValue(kTTSym_type, kTTSym_integer);
    m_dataStartPoint->setAttributeValue(kTTSym_description, TTSymbol("set the time where to start i-score execution (in ms)"));
    
    TTModularRegisterObject(TTAddress("/Transport/StartPoint"), m_dataStartPoint);
    
    // Speed
    TTValuePtr batonSpeed = new TTValue(TTPtr(this));
    batonSpeed->append(TTSymbol("Speed"));
    TTModularCreateData(kTTSym_message, batonSpeed, &TransportDataValueCallback, &m_dataSpeed);
    
    m_dataSpeed->setAttributeValue(kTTSym_type, kTTSym_decimal);
    v = TTValue(0., 10.);
    m_dataSpeed->setAttributeValue(kTTSym_rangeBounds, v);
    m_dataSpeed->setAttributeValue(kTTSym_description, TTSymbol("change i-score speed rate execution"));
    
    TTModularRegisterObject(TTAddress("/Transport/Speed"), m_dataSpeed);
    
    
    ////////////
    // Example : Create a distant application
    ////////////
    TTObjectBasePtr anApplication = NULL;
    TTSymbol        MinuitApplicationName;
    
    // create an application called MinuitDevice1
    MinuitApplicationName = TTSymbol("MinuitDevice1");
    args = TTValue(MinuitApplicationName);
    TTObjectBaseInstantiate(kTTSym_Application, TTObjectBaseHandle(&anApplication), args);
    
    // set application type : here 'mirror' because it use Minuit protocol
    anApplication->setAttributeValue(kTTSym_type, TTSymbol("mirror"));
    
    ////////////
    // Example : Register i-score and MinuitDevice1 to the Minuit protocol
    ////////////
    
    // check if the Minuit protocol has been loaded
    if (getProtocol(TTSymbol("Minuit"))) {
        
        // register the local application to the Minuit protocol
        getProtocol(TTSymbol("Minuit"))->sendMessage(TTSymbol("registerApplication"), TTSymbol(applicationName), kTTValNONE);
        
        // get parameter's table
        v = TTSymbol(applicationName);
        err = getProtocol(TTSymbol("Minuit"))->getAttributeValue(TTSymbol("applicationParameters"), v);
        
        if (!err) {
            
            hashParameters = TTHashPtr((TTPtr)v[0]);
        
            // replace the Minuit parameters for the local application
            hashParameters->remove(TTSymbol("port"));
            hashParameters->append(TTSymbol("port"), 8002);
            
            hashParameters->remove(TTSymbol("ip"));
            hashParameters->append(TTSymbol("ip"), TTSymbol("127.0.0.1"));
        
            v = TTSymbol(applicationName);
            v.append((TTPtr)hashParameters);
            getProtocol(TTSymbol("Minuit"))->setAttributeValue(TTSymbol("applicationParameters"), v);
        }
        
        // register this application to the Minuit protocol
        getProtocol(TTSymbol("Minuit"))->sendMessage(TTSymbol("registerApplication"), MinuitApplicationName, kTTValNONE);
        
        // get parameter's table
        v = TTSymbol(MinuitApplicationName);
        err = getProtocol(TTSymbol("Minuit"))->getAttributeValue(TTSymbol("applicationParameters"), v);
        
        if (!err) {
            
            hashParameters = TTHashPtr((TTPtr)v[0]);
        
            // replace the Minuit parameters for the distant application
            hashParameters->remove(TTSymbol("port"));
            hashParameters->append(TTSymbol("port"), 9998);
            
            hashParameters->remove(TTSymbol("ip"));
            hashParameters->append(TTSymbol("ip"), TTSymbol("127.0.0.1"));
        
            v = TTValue(MinuitApplicationName);
            v.append((TTPtr)hashParameters);
            getProtocol(TTSymbol("Minuit"))->setAttributeValue(TTSymbol("applicationParameters"), v);
        }
        
        // run the Minuit protocol
        TTModularApplications->sendMessage(TTSymbol("ProtocolRun"), TTSymbol("Minuit"), kTTValNONE);
        
        ////////////
        // Example : Build the namespace of MinuitDevice1 using discovery feature (if the protocol provides it)
        ////////////
        
        // you can create an OSC receive on the 9998 port to see that a namespace request is sent by i-score (using Pure Data for example)
        anApplication->sendMessage(TTSymbol("DirectoryBuild"));
    }
/*
    ////////////
    // Example : Read the namespace of MinuitDevice1 from a namespace file
    ////////////
    
    // create a TTXmlHandler class to parse a xml namespace file
    TTXmlHandlerPtr myXmlHandler = NULL;
    TTObjectBaseInstantiate(kTTSym_XmlHandler, TTObjectBaseHandle(&myXmlHandler), kTTValNONE);
    
    // prepare the TTXmlHandler to pass the result of the parsing to MinuitDevice1
    v = TTValue(anApplication);
    myXmlHandler->setAttributeValue(TTSymbol("object"), v);
    
    // read a namespace file
    err = myXmlHandler->sendMessage(TTSymbol("Read"), TTSymbol("/Users/WALL-E/Documents/Jamoma/Modules/Modular/implementations/MaxMSP/jcom.modular/remoteApp - namespace.xml"), kTTValNONE);
    
    if (!err) {
        // get the root of the TNodeDirectory of MinuitDevice1
        // note : the TTNodeDirectory is a tree structure used to registered and retrieve TTObjects using TTAddress
        TTNodeDirectoryPtr anApplicationDirectory = getApplicationDirectory(TTSymbol("MinuitDevice1"));
        
        // return all addresses of the TTNodeDirectory
        std::cout << "__ Dump MinuitDevice1 directory __" << std::endl;
        
        // start to dump addresses recursilvely from the root of the directory
        dumpAddressBelow(anApplicationDirectory->getRoot());
        
        std::cout << "__________________________________" << std::endl;
    }
*/    
}

void Engine::initScore()
{
    TTTimeEventPtr  startEvent = NULL;
    TTTimeEventPtr  endEvent = NULL;
    TTValue         args;
    
    // this initializes the Score framework
    TTScoreInit();
    
    // Create a time event for the start
    args = TTUInt32(0);
    TTObjectBaseInstantiate(TTSymbol("TimeEvent"), TTObjectBaseHandle(&startEvent), args);
    
    // Create a time event for the end
    args = TTUInt32(SCENARIO_SIZE);
    TTObjectBaseInstantiate(TTSymbol("TimeEvent"), TTObjectBaseHandle(&endEvent), args);
    
    // Create the main scenario
    args = TTObjectBasePtr(startEvent);
    args.append(TTObjectBasePtr(endEvent));
    TTObjectBaseInstantiate(TTSymbol("Scenario"), TTObjectBaseHandle(&m_mainScenario), args);
    
    m_mainScenario->setAttributeValue(kTTSym_name, TTSymbol("root"));
    
    // Store the main scenario (so ROOT_BOX_ID is 1)
    cacheTimeProcess(m_mainScenario, "root");
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
    
    // Delete the main scenario
    m_mainScenario->sendMessage(TTSymbol("ReleaseEvents"));
    TTObjectBaseRelease(TTObjectBaseHandle(&m_mainScenario));
}

TimeProcessId Engine::cacheTimeProcess(TTTimeProcessPtr timeProcess, const std::string & name)
{
    TimeProcessId id;
    EngineCacheElementPtr e;
    
    e = new EngineCacheElement();
    e->object = TTObjectBasePtr(timeProcess);
    e->name = name;
    
    id = m_nextTimeProcessId;
    m_timeProcessMap[id] = e;
    m_nextTimeProcessId++;
    
    cacheRunningCallback(id);
    
    return id;
}

TTTimeProcessPtr Engine::getTimeProcess(TimeProcessId boxId)
{
    return TTTimeProcessPtr(TTObjectBasePtr(m_timeProcessMap[boxId]->object));
}

void Engine::uncacheTimeProcess(TimeProcessId boxId)
{
    EngineCacheElementPtr e = m_timeProcessMap[boxId];
    
    uncacheRunningCallback(boxId);
    
    delete e;
    m_timeProcessMap.erase(boxId);
}

void Engine::clearTimeProcess()
{
    EngineCacheMapIterator it;
    
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        // don't remove the root time process (the main scenario)
        if (it->first != ROOT_BOX_ID) {
            
            uncacheRunningCallback(it->first);
            delete it->second;
        }
    }
    
    // don't clear the m_timeProcessMap (because it is not empty)
    m_runningCallbackMap.clear();
    
    // set the next id to 2 because the main scenario is registered with the 1 id
    m_nextTimeProcessId = 2;
}

IntervalId Engine::cacheInterval(TTTimeProcessPtr timeProcess)
{
    TimeProcessId id;
    EngineCacheElementPtr e;
    
    e = new EngineCacheElement();
    e->object = TTObjectBasePtr(timeProcess);
    
    id = m_nextIntervalId;
    m_intervalMap[id] = e;
    m_nextIntervalId++;
    
    return id;
}

TTTimeProcessPtr Engine::getInterval(IntervalId relationId)
{
    return TTTimeProcessPtr(TTObjectBasePtr(m_intervalMap[relationId]->object));
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
    TTTimeProcessPtr        timeProcess = getTimeProcess(timeProcessId);
    TTTimeEventPtr          timeEvent;
    ConditionedProcessId    id;
    EngineCacheElementPtr   e;
    TTValue                 v;
    
    // Get start or end time event
    if (controlPointId == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    
    timeEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Create a new engine cache element
    e = new EngineCacheElement();
    e->object = timeProcess;
    e->index = controlPointId;
    
    id = m_nextConditionedProcessId;
    m_conditionedProcessMap[id] = e;
    m_nextConditionedProcessId++;
    
    // We cache an observer on time event ready attribute
    cacheReadyCallback(id, controlPointId);
    
    // We cache a TTData to allow remote triggering
    cacheTriggerDataCallback(id, timeProcessId);
    
    return id;
}

TTTimeProcessPtr Engine::getConditionedProcess(ConditionedProcessId triggerId, TimeEventIndex& controlPointId)
{
    controlPointId = m_conditionedProcessMap[triggerId]->index;
    
    return TTTimeProcessPtr(TTObjectBasePtr(m_conditionedProcessMap[triggerId]->object));
}

void Engine::uncacheConditionedProcess(ConditionedProcessId triggerId)
{
    // Get the engine cache element
    EngineCacheElementPtr e = m_conditionedProcessMap[triggerId];
    
    // Uncache observer on time event ready attribute
    uncacheReadyCallback(triggerId, e->index);
    
    // Uncache TTData used for remote triggering
    uncacheTriggerDataCallback(triggerId);
    
    // Delete the engine cache element
    delete e;
    m_conditionedProcessMap.erase(triggerId);
}

void Engine::clearConditionedProcess()
{
    EngineCacheMapIterator it;
    
    for (it = m_conditionedProcessMap.begin(); it != m_conditionedProcessMap.end(); ++it) {
        
        uncacheReadyCallback(it->first, it->second->index);
        uncacheTriggerDataCallback(it->first);
        
        delete it->second;
    }
    
    m_conditionedProcessMap.clear();
    m_readyCallbackMap.clear();
    m_triggerDataMap.clear();
    
    m_nextConditionedProcessId = 1;
}

void Engine::cacheTimeCondition(ConditionedProcessId triggerId, TTTimeConditionPtr timeCondition)
{
    EngineCacheElementPtr   e;
    TTValue                 args, v;
    
    e = new EngineCacheElement();
    e->object = timeCondition;
    e->index = triggerId;
    
    m_timeConditionMap[triggerId] = e;
}

TTTimeConditionPtr Engine::getTimeCondition(ConditionedProcessId triggerId)
{
    return TTTimeConditionPtr(TTObjectBasePtr(m_timeConditionMap[triggerId]->object));
}

void Engine::uncacheTimeCondition(ConditionedProcessId triggerId)
{
    EngineCacheElementPtr   e = m_runningCallbackMap[triggerId];
    
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

void Engine::cacheRunningCallback(TimeProcessId boxId)
{
    EngineCacheElementPtr   e;
    TTValue                 v;
    TTValuePtr              runningAttributeBaton;
    TTObjectBasePtr         scheduler;
    TTAttributePtr          anAttribute;
    TTErr                   err;
    
    e = new EngineCacheElement();
    e->object = NULL;
    e->index = boxId;
    
    // create a TTCallback to observe each time process scheduler running attribute (using TimeProcessSchedulerRunningAttributeCallback)
    TTObjectBaseInstantiate(TTSymbol("callback"), &e->object, kTTValNONE);
    
    runningAttributeBaton = new TTValue(TTPtr(this)); // runningAttributeBaton will be deleted during the callback destruction
    runningAttributeBaton->append(TTUInt32(boxId));
    
    e->object->setAttributeValue(kTTSym_baton, TTPtr(runningAttributeBaton));
    e->object->setAttributeValue(kTTSym_function, TTPtr(&TimeProcessSchedulerRunningAttributeCallback));
    
    // register for scheduler running attribute observation
    getTimeProcess(boxId)->getAttributeValue(TTSymbol("scheduler"), v);
    scheduler = v[0];
    
    err = scheduler->findAttribute(TTSymbol("running"), &anAttribute);
    
    if (!err)
        anAttribute->registerObserverForNotifications(*e->object);
    
    m_runningCallbackMap[boxId] = e;
}

void Engine::uncacheRunningCallback(TimeProcessId boxId)
{
    EngineCacheElementPtr   e = m_runningCallbackMap[boxId];
    TTValue                 v;
    TTObjectBasePtr         scheduler;
    TTAttributePtr          anAttribute;
    TTErr                   err;
    
    // unregister for scheduler running attribute observation
    getTimeProcess(boxId)->getAttributeValue(TTSymbol("scheduler"), v);
    scheduler = v[0];
    
    err = scheduler->findAttribute(TTSymbol("running"), &anAttribute);
    
    if (!err) {
        
        err = anAttribute->unregisterObserverForNotifications(*e->object);
        
        if (!err)
            TTObjectBaseRelease(&e->object);
    }
    
    delete e;
    m_runningCallbackMap.erase(boxId);
}

void Engine::cacheReadyCallback(ConditionedProcessId triggerId, TimeEventIndex controlPointId)
{
    EngineCacheElementPtr   e;
    TTTimeProcessPtr        timeProcess = getConditionedProcess(triggerId, controlPointId);
    TTTimeEventPtr          timeEvent;
    TTValue                 v, out;
    TTValuePtr              readyAttributeBaton;
    TTAttributePtr          anAttribute;
    TTErr                   err;
    
    // Create a new engine cache element
    e = new EngineCacheElement();
    e->object = NULL;
    e->index = triggerId;
    
    // Get start or end time event
    if (controlPointId == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    
    timeEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Create a TTCallback to observe time event ready attribute (using TimeEventReadyAttributeCallback)
    TTObjectBaseInstantiate(TTSymbol("callback"), &e->object, kTTValNONE);
    
    readyAttributeBaton = new TTValue(TTPtr(this)); // readyAttributeBaton will be deleted during the callback destruction
    readyAttributeBaton->append(TTUInt32(triggerId));
    
    e->object->setAttributeValue(kTTSym_baton, TTPtr(readyAttributeBaton));
    e->object->setAttributeValue(kTTSym_function, TTPtr(&TimeEventReadyAttributeCallback));
    
    // register for ready attribute observation
    err = timeEvent->findAttribute(TTSymbol("ready"), &anAttribute);
    
    if (!err)
        anAttribute->registerObserverForNotifications(*e->object);
    
    m_readyCallbackMap[triggerId] = e;
}

void Engine::uncacheReadyCallback(ConditionedProcessId triggerId, TimeEventIndex controlPointId)
{
    EngineCacheElementPtr   e = m_readyCallbackMap[triggerId];
    TTTimeProcessPtr        timeProcess = getConditionedProcess(triggerId, controlPointId);
    TTTimeEventPtr          timeEvent;
    TTValue                 v;
    TTAttributePtr          anAttribute;
    TTErr                   err;
    
    // Get start or end time event
    if (controlPointId == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    
    timeEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // unregister for scheduler running attribute observation    
    err = timeEvent->findAttribute(TTSymbol("ready"), &anAttribute);
    
    if (!err) {
        
        err = anAttribute->unregisterObserverForNotifications(*e->object);
        
        if (!err)
            TTObjectBaseRelease(&e->object);
    }
    
    delete e;
    m_readyCallbackMap.erase(triggerId);
}

void Engine::cacheTriggerDataCallback(ConditionedProcessId triggerId, TimeProcessId boxId)
{
    EngineCacheElementPtr   e;
    TTValue                 v;
    TTString                address;
    TTSymbol                boxName = TTSymbol();
    
    e = new EngineCacheElement();
    e->object = NULL;
    e->index = boxId;
    
    // Create a TTData
    TTModularCreateData(kTTSym_message, NULL, NULL, &e->object);
    
    e->object->setAttributeValue(kTTSym_type, kTTSym_none);
    e->object->setAttributeValue(kTTSym_description, TTSymbol("trigger an event"));
    
    // register the TTData under /Box.n/start or /Box.n/end address
    address = "/";
    address += m_timeProcessMap[boxId]->name.c_str();

    if (m_conditionedProcessMap[triggerId]->index == BEGIN_CONTROL_POINT_INDEX)
        address += "/start";
    else
        address += "/end";
    
    TTModularRegisterObject(TTAddress(address), e->object);
    
    m_triggerDataMap[triggerId] = e;
}

void Engine::uncacheTriggerDataCallback(ConditionedProcessId triggerId)
{
    ; // TODO
}

// Edition ////////////////////////////////////////////////////////////////////////

TimeProcessId Engine::addBox(TimeValue boxBeginPos, TimeValue boxLength, const std::string & name, TimeProcessId motherId)
{
    TTTimeEventPtr      startEvent, endEvent;
    TTTimeProcessPtr    timeProcess;
    TimeProcessId       boxId;
    TTSymbol            boxName;
    TTValue             v, args;
    
    // Create a time event for the start
    args = TTUInt32(boxBeginPos);
    m_mainScenario->sendMessage(TTSymbol("TimeEventCreate"), args, v);
    startEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Create a time event for the end
    args = TTUInt32(boxBeginPos+boxLength);
    m_mainScenario->sendMessage(TTSymbol("TimeEventCreate"), args, v);
    endEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Create a new automation time process
    args = TTSymbol("Automation");
    args.append(TTObjectBasePtr(startEvent));
    args.append(TTObjectBasePtr(endEvent));
    m_mainScenario->sendMessage(TTSymbol("TimeProcessCreate"), args, v);
    timeProcess = TTTimeProcessPtr(TTObjectBasePtr(v[0]));
    
    boxName = TTSymbol(name);
    timeProcess->setAttributeValue(kTTSym_name, boxName);
    
    // Cache it and get an unique id for this process
    boxId = cacheTimeProcess(timeProcess, name);
    
    iscoreEngineDebug TTLogMessage("TimeProcess %ld created at %ld ms for a duration of %ld ms\n", boxId, boxBeginPos, boxLength);
    
	return boxId;
}

void Engine::removeBox(TimeProcessId boxId)
{
    TTTimeProcessPtr        timeProcess;
    EngineCacheMapIterator  it;
    ConditionedProcessId    id1 = 0;
    ConditionedProcessId    id2 = 0;
    TTValue                 v, events;
    TTErr                   err;
    
    // Retreive the time process using the boxId
    timeProcess = getTimeProcess(boxId);
    
    // TODO : delete TTCallback used to observe each time process scheduler running attribute
    // getTimeProcess(boxId)->removeObserver() ?
    
    // Is it an conditionedProcess ?
    // note : it can be registered 2 times for a start and end conditioned event
    for (it = m_conditionedProcessMap.begin(); it != m_conditionedProcessMap.end(); ++it) {
        
        if (it->second->object == timeProcess) {
         
            if (!id1) id1 = it->first;
            else if (!id2) id2 = it->first;
        }
    }
    
    if (id1) uncacheConditionedProcess(id1);
    if (id2) uncacheConditionedProcess(id2);
    
    // Remove the time process from the cache
    uncacheTimeProcess(boxId);
    
    // Release the time process
    v = TTObjectBasePtr(timeProcess);
    m_mainScenario->sendMessage(TTSymbol("TimeProcessRelease"), v, events);
    
    // Release start event
    err = m_mainScenario->sendMessage(TTSymbol("TimeEventRelease"), events[0], kTTValNONE);
    if (err)
        iscoreEngineDebug TTLogMessage("Box %ld cannot release his start event\n", boxId);
    
    // Release end event
    err = m_mainScenario->sendMessage(TTSymbol("TimeEventRelease"), events[1], kTTValNONE);
    if (err)
        iscoreEngineDebug TTLogMessage("Box %ld cannot release his end event\n", boxId);
}

IntervalId Engine::addTemporalRelation(TimeProcessId boxId1,
                                       TimeEventIndex controlPoint1,
                                       TimeProcessId boxId2,
                                       TimeEventIndex controlPoint2,
                                       TemporalRelationType type,
                                       vector<TimeProcessId>& movedBoxes)
{
    TTTimeProcessPtr        timeProcess = NULL;
    TTTimeProcessPtr        tp1, tp2;
    TTTimeEventPtr          startEvent, endEvent;
    IntervalId              relationId;
    TTValue                 v, args;
    EngineCacheMapIterator  it;
    TTErr                   err;
    
    // Get the events from the given box ids and pass them to the time process
    tp1 = getTimeProcess(boxId1);
    tp2 = getTimeProcess(boxId2);
    
    if (controlPoint1 == BEGIN_CONTROL_POINT_INDEX)
        tp1->getAttributeValue(TTSymbol("startEvent"), v);
    else
        tp1->getAttributeValue(TTSymbol("endEvent"), v);
    
    startEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    if (controlPoint2 == BEGIN_CONTROL_POINT_INDEX)
        tp2->getAttributeValue(TTSymbol("startEvent"), v);
    else
        tp2->getAttributeValue(TTSymbol("endEvent"), v);
    
    endEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // TODO : order the events
    
    // Create a new interval time process
    args = TTSymbol("Interval");
    args.append(TTObjectBasePtr(startEvent));
    args.append(TTObjectBasePtr(endEvent));
    err = m_mainScenario->sendMessage(TTSymbol("TimeProcessCreate"), args, v);
    timeProcess = TTTimeProcessPtr(TTObjectBasePtr(v[0]));

    // an error can append if the start is after the end
    if (!err) {
        
        // Set the time process rigid
        timeProcess->setAttributeValue(TTSymbol("rigid"), kTTBoolYes);
        
        // Cache it and get an unique id for this process
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
    TTTimeProcessPtr    timeProcess;
    TTValue             v, events;
    TTErr               err;
    
    // Retreive the interval using the relationId
    timeProcess = getInterval(relationId);
    
    // Don't release the events (they are destroyed with the box)
    
    // Release the time process
    v = TTObjectBasePtr(timeProcess);
    m_mainScenario->sendMessage(TTSymbol("TimeProcessRelease"), v, events);
    
    // Release start event
    err = m_mainScenario->sendMessage(TTSymbol("TimeEventRelease"), events[0], kTTValNONE);
    if (err)
        iscoreEngineDebug TTLogMessage("Relation %ld cannot release his start event\n", relationId);
    
    // Release end event
    err = m_mainScenario->sendMessage(TTSymbol("TimeEventRelease"), events[1], kTTValNONE);
    if (err)
        iscoreEngineDebug TTLogMessage("Relation %ld cannot release his end event\n", relationId);
    
    // Remove the interval from the cache
    uncacheInterval(relationId);
}

void Engine::changeTemporalRelationBounds(IntervalId relationId, BoundValue minBound, BoundValue maxBound, vector<TimeProcessId>& movedBoxes)
{
    TTTimeProcessPtr        timeProcess = getInterval(relationId);
    EngineCacheMapIterator  it;
    TTValue                 v;

    // filtering NO_BOUND (-1) and negative value because we use unsigned int
    if (minBound == NO_BOUND)
        v = TTValue(TTUInt32(0));
    else if (minBound < 0)
        v = TTValue(TTUInt32(abs(minBound)));
    else
        v = TTValue(TTUInt32(minBound));
    
    if (maxBound == NO_BOUND)
        v.append(TTUInt32(0));
    else if (maxBound < 0)
        v = TTValue(TTUInt32(abs(maxBound)));
    else
        v.append(TTUInt32(maxBound));
    
    // NOTE : sending 0 0 means the relation is not rigid
    // NOTE : it is also possible to use the "rigid" attribute to swicth between those two states
    timeProcess->sendMessage(TTSymbol("Limit"), v, kTTValNONE);
    
    // return the entire timeProcessMap except the first process !!! (this is bad but it is like former engine)
    it = m_timeProcessMap.begin();
    it++;
    for (; it != m_timeProcessMap.end(); ++it)
        movedBoxes.push_back(it->first);
}

bool Engine::isTemporalRelationExisting(TimeProcessId boxId1, TimeEventIndex controlPoint1, TimeProcessId boxId2, TimeEventIndex controlPoint2)
{
    TTValue                 v1, v2, v;
    TTTimeProcessPtr        timeProcess, tp1, tp2;
    IntervalId              found = NO_ID;
    EngineCacheMapIterator  it;
    
    // Get the events from the given box ids and pass them to the time process
    tp1 = getTimeProcess(boxId1);
    tp2 = getTimeProcess(boxId2);
    
    if (controlPoint1 == BEGIN_CONTROL_POINT_INDEX)
        tp1->getAttributeValue(TTSymbol("startEvent"), v1);
    else
        tp1->getAttributeValue(TTSymbol("endEvent"), v1);
    
    if (controlPoint2 == BEGIN_CONTROL_POINT_INDEX)
        tp2->getAttributeValue(TTSymbol("startEvent"), v2);
    else
        tp2->getAttributeValue(TTSymbol("endEvent"), v2);
    
    // Look into the interval map to retreive an interval with the same events
    for (it = m_intervalMap.begin(); it != m_intervalMap.end(); ++it) {
        
        timeProcess = TTTimeProcessPtr(TTObjectBasePtr(it->second->object));
        
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
        
        if (v == v1) {
            
            timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
            
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
    TTValue                 v, vt;
    TTTimeProcessPtr        timeProcess = getInterval(relationId);
    TTTimeEventPtr          event;
    TimeProcessId           found = NO_ID;
    EngineCacheMapIterator  it;
    
    // get the start event of the interval
	timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    event = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        timeProcess = TTTimeProcessPtr(TTObjectBasePtr(it->second->object));
        
        timeProcess->getAttributeValue(TTSymbol("startEvent"), vt);
        
        if (v == vt) {
            found = it->first;
            break;
        }
        
        timeProcess->getAttributeValue(TTSymbol("endEvent"), vt);
        
        if (v == vt) {
            found = it->first;
            break;
        }
    }
    
    return found;
}

TimeEventIndex Engine::getRelationFirstCtrlPointIndex(IntervalId relationId)
{
    TTValue                 v, vt;
    TTTimeProcessPtr        timeProcess = getInterval(relationId);
    TTTimeEventPtr          event;
    TimeEventIndex          ctrlPointId = NO_ID;
    EngineCacheMapIterator  it;
    
    // get the start event of the interval
	timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    event = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        timeProcess = TTTimeProcessPtr(TTObjectBasePtr(it->second->object));
        
        timeProcess->getAttributeValue(TTSymbol("startEvent"), vt);
        
        if (v == vt) {
            ctrlPointId = BEGIN_CONTROL_POINT_INDEX;
            break;
        }
        
        timeProcess->getAttributeValue(TTSymbol("endEvent"), vt);
        
        if (v == vt) {
            ctrlPointId = END_CONTROL_POINT_INDEX;
            break;
        }
    }
    
    return ctrlPointId;
}

TimeProcessId Engine::getRelationSecondBoxId(IntervalId relationId)
{
    TTValue                 v, vt;
    TTTimeProcessPtr        timeProcess = getInterval(relationId);
    TTTimeEventPtr          event;
    TimeProcessId           found = NO_ID;
    EngineCacheMapIterator  it;
    
    // Get the end event of the interval
	timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    event = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        timeProcess = TTTimeProcessPtr(TTObjectBasePtr(it->second->object));
        
        timeProcess->getAttributeValue(TTSymbol("startEvent"), vt);
        
        if (v == vt) {
            found = it->first;
            break;
        }
        
        timeProcess->getAttributeValue(TTSymbol("endEvent"), vt);
        
        if (v == vt) {
            found = it->first;
            break;
        }
    }
    
    return found;
}

TimeEventIndex Engine::getRelationSecondCtrlPointIndex(IntervalId relationId)
{
    TTValue                 v, vt;
    TTTimeProcessPtr        timeProcess = getInterval(relationId);
    TTTimeEventPtr          event;
    TimeEventIndex          ctrlPointId = NO_ID;
    EngineCacheMapIterator  it;
    
    // get the start event of the interval
	timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    event = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        timeProcess = TTTimeProcessPtr(TTObjectBasePtr(it->second->object));
        
        timeProcess->getAttributeValue(TTSymbol("startEvent"), vt);
        
        if (v == vt) {
            ctrlPointId = BEGIN_CONTROL_POINT_INDEX;
            break;
        }
        
        timeProcess->getAttributeValue(TTSymbol("endEvent"), vt);
        
        if (v == vt) {
            ctrlPointId = END_CONTROL_POINT_INDEX;
            break;
        }
    }
    
    return ctrlPointId;
}

BoundValue Engine::getRelationMinBound(IntervalId relationId)
{
    TTTimeProcessPtr    timeProcess = getInterval(relationId);
    TTValue             v;
    
    timeProcess->getAttributeValue(TTSymbol("durationMin"), v);
    
    return v[0];
}

BoundValue Engine::getRelationMaxBound(IntervalId relationId)
{
    TTTimeProcessPtr  timeProcess = getInterval(relationId);
    TTValue         v;

    timeProcess->getAttributeValue(TTSymbol("durationMax"), v);
    
    return v[0];
}

bool Engine::performBoxEditing(TimeProcessId boxId, TimeValue start, TimeValue end, vector<TimeProcessId>& movedBoxes)
{
    TTTimeProcessPtr        timeProcess = getTimeProcess(boxId);
    TTValue                 v;
    EngineCacheMapIterator  it;
    TTErr                   err;
    
    v = TTValue(start);
    v.append(end);
    
    err = timeProcess->sendMessage(TTSymbol("Move"), v, kTTValNONE);
    
    // return the entire timeProcessMap except the first process !!! (this is bad but it is like former engine)
    it = m_timeProcessMap.begin();
    it++;
    for (; it != m_timeProcessMap.end(); ++it)
        movedBoxes.push_back(it->first);
    
    return !err;
}

std::string Engine::getBoxName(TimeProcessId boxId)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    TTSymbol            name;
    
	timeProcess->getAttributeValue(TTSymbol("name"), v);
    
    name = v[0];
    
    return name.c_str();
}

unsigned int Engine::getBoxVerticalPosition(TimeProcessId boxId)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    
	timeProcess->getAttributeValue(TTSymbol("verticalPosition"), v);
    
    return v[0];
}

void Engine::setBoxVerticalPosition(TimeProcessId boxId, unsigned int newPosition)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v = TTUInt32(newPosition);
    
	timeProcess->setAttributeValue(TTSymbol("verticalPosition"), v);
}

unsigned int Engine::getBoxVerticalSize(TimeProcessId boxId)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    
	timeProcess->getAttributeValue(TTSymbol("verticalSize"), v);
    
    return v[0];
}

void Engine::setBoxVerticalSize(TimeProcessId boxId, unsigned int newSize)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v = TTUInt32(newSize);
    
	timeProcess->setAttributeValue(TTSymbol("verticalSize"), v);
}

QColor Engine::getBoxColor(TimeProcessId boxId)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    QColor              color;
    
	timeProcess->getAttributeValue(TTSymbol("color"), v);
    
    color = QColor(v[0], v[1], v[2]);
    
    return color;
}

void Engine::setBoxColor(TimeProcessId boxId, QColor newColor)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    
    v = newColor.red();
    v.append(newColor.green());
    v.append(newColor.blue());
    
	timeProcess->setAttributeValue(TTSymbol("color"), v);
}

TimeValue Engine::getBoxBeginTime(TimeProcessId boxId)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    
	timeProcess->getAttributeValue(TTSymbol("startDate"), v);
    
    return v[0];
}

TimeValue Engine::getBoxEndTime(TimeProcessId boxId)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    
	timeProcess->getAttributeValue(TTSymbol("endDate"), v);
    
    return v[0];
}

TimeValue Engine::getBoxDuration(TimeProcessId boxId)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    
	timeProcess->getAttributeValue(TTSymbol("duration"), v);
    
    return v[0];
}

int Engine::getBoxNbCtrlPoints(TimeProcessId boxId)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    
	timeProcess->getAttributeValue(TTSymbol("intermediateEvents"), v);
    
    return v.size() + 2; // because there is always a start and an end event too
}

TimeEventIndex Engine::getBoxFirstCtrlPointIndex(TimeProcessId boxId)
{
	return BEGIN_CONTROL_POINT_INDEX;
}

TimeEventIndex Engine::getBoxLastCtrlPointIndex(TimeProcessId boxId)
{
	return END_CONTROL_POINT_INDEX;
}

void Engine::setCtrlPointMessagesToSend(TimeProcessId boxId, TimeEventIndex controlPointIndex, std::vector<std::string> messageToSend, bool muteState)
{
    TTValue             v;
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTTimeEventPtr      event;
    TTObjectBasePtr     state;
    TTUInt32            i;

    // Get the start or end event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    
    event = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // get the state of the event
    event->getAttributeValue(TTSymbol("state"), v);
    state = v[0];
    
    // clear the state
    state->sendMessage(TTSymbol("Clear"));
    
    // parse each incoming string into < directory:/address, value >
    for (i = 0; i < messageToSend.size(); i++) {
        
        TTValue v = TTString(messageToSend[i]);
        v.fromString();
        
        TTSymbol aSymbol = v[0];
		TTAddress anAddress = toTTAddress(aSymbol.string().data());
        v[0] = anAddress;
        
        // append a line to the state
        state->sendMessage(TTSymbol("Append"), v, kTTValNONE);
    }
    
    // Flatten the state to increase the speed of the recall
    state->sendMessage(TTSymbol("Flatten"));
    
    // Update all curves (for automation process only)
    TTValue empty; // théo : this is to pass thru a Foundation bug which will disapear in a next version
    timeProcess->sendMessage(TTSymbol("CurveUpdate"), empty, empty);
}

void Engine::getCtrlPointMessagesToSend(TimeProcessId boxId, TimeEventIndex controlPointIndex, std::vector<std::string>& messages)
{
    TTValue             v;
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTTimeEventPtr      event;
    TTBoolean           flattened;
    TTObjectBasePtr     state;
    TTListPtr           lines = NULL;
    TTDictionaryPtr     aLine;
    TTAddress           address;
    std::string         s;
    
    // Get the start or end event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    
    event = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // get the state of the event
    event->getAttributeValue(TTSymbol("state"), v);
    state = v[0];
    
    // check if the state is flattened
    state->getAttributeValue(TTSymbol("flattened"), v);
    flattened = v[0];
    
    if (!flattened)
        state->sendMessage(TTSymbol("Flatten"));
    
    // get the state lines
    state->getAttributeValue(TTSymbol("flattenedLines"), v);
    lines = TTListPtr((TTPtr)v[0]);
    
    if (lines) {
        // edit each line address into a "directory/address value" string
        for (lines->begin(); lines->end(); lines->next()) {
            
            aLine = TTDictionaryPtr((TTPtr)lines->current()[0]);
            
            // get the target address
            aLine->lookup(kTTSym_target, v);
            address = v[0];
            
            // get value
            aLine->getValue(v);
            v.toString();
            
            // edit string
            s = toNetworkTreeAddress(address);
            s += " ";
            s += TTString(v[0]).c_str();
            
            messages.push_back(s);
        }
    }
}

void Engine::setCtrlPointMutingState(TimeProcessId boxId, TimeEventIndex controlPointIndex, bool mute)
{
    TTValue             v;
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTTimeEventPtr      event;
    
    // Get the start or end event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    
    event = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    event->setAttributeValue(kTTSym_mute, TTBoolean(mute));
}

//CURVES ////////////////////////////////////////////////////////////////////////////////////

void Engine::addCurve(TimeProcessId boxId, const std::string & address)
{
    TTTimeProcessPtr  timeProcess = getTimeProcess(boxId);
    
    // add the curve addresses into the automation time process
    timeProcess->sendMessage(TTSymbol("CurveAdd"), toTTAddress(address), kTTValNONE);
}

void Engine::removeCurve(TimeProcessId boxId, const std::string & address)
{
    TTTimeProcessPtr  timeProcess = getTimeProcess(boxId);
    
    // remove the curve addresses of the automation time process
    timeProcess->sendMessage(TTSymbol("CurveRemove"), toTTAddress(address), kTTValNONE);
}

void Engine::clearCurves(TimeProcessId boxId)
{
    TTTimeProcessPtr  timeProcess = getTimeProcess(boxId);
    
    // clear all the curves of the automation time process
    timeProcess->sendMessage(TTSymbol("Clear"));
}

std::vector<std::string> Engine::getCurvesAddress(TimeProcessId boxId)
{
    std::vector<std::string> curveAddresses;
    
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTValue             v;
    
    // get the curve addresses of the automation time process
    timeProcess->getAttributeValue(TTSymbol("curveAddresses"), v);
    
    // copy the addresses into the vector
    for (TTUInt32 i = 0; i < v.size(); i++)
        curveAddresses.push_back(toNetworkTreeAddress(v[i]));
    
	return curveAddresses;
}

void Engine::setCurveSampleRate(TimeProcessId boxId, const std::string & address, unsigned int nbSamplesBySec)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTObjectBasePtr     curve;
    TTValue             v;
    TTErr               err;
    
    // get curve object at address
    err = timeProcess->sendMessage(TTSymbol("CurveGet"), toTTAddress(address), v);
    
    if (!err) {
        
        curve = v[0];
        
        curve->setAttributeValue(TTSymbol("sampleRate"), nbSamplesBySec);
    }
}

unsigned int Engine::getCurveSampleRate(TimeProcessId boxId, const std::string & address)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTObjectBasePtr     curve;
    TTValue             v;
    TTErr               err;
    
    // get curve object at address
    err = timeProcess->sendMessage(TTSymbol("CurveGet"), toTTAddress(address), v);
    
    if (!err) {
        
        curve = v[0];
        
        curve->getAttributeValue(TTSymbol("sampleRate"), v);
        
        return TTUInt32(v[0]);
    }
    
	return 0;
}

void Engine::setCurveRedundancy(TimeProcessId boxId, const std::string & address, bool redundancy)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTObjectBasePtr     curve;
    TTValue             v;
    TTErr               err;
    
    // get curve object at address
    err = timeProcess->sendMessage(TTSymbol("CurveGet"), toTTAddress(address), v);
    
    if (!err) {
        
        curve = v[0];
        
        curve->setAttributeValue(TTSymbol("redundancy"), redundancy);
    }
}

bool Engine::getCurveRedundancy(TimeProcessId boxId, const std::string & address)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTObjectBasePtr     curve;
    TTValue             v;
    TTErr               err;
    
    // get curve object at address
    err = timeProcess->sendMessage(TTSymbol("CurveGet"), toTTAddress(address), v);
    
    if (!err) {
        
        curve = v[0];
        
        curve->getAttributeValue(TTSymbol("redundancy"), v);
        
        return TTBoolean(v[0]);
    }
    
	return false;
}

void Engine::setCurveMuteState(TimeProcessId boxId, const std::string & address, bool muteState)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTObjectBasePtr     curve;
    TTValue             v;
    TTErr               err;
    
    // get curve object at address
    err = timeProcess->sendMessage(TTSymbol("CurveGet"), toTTAddress(address), v);
    
    if (!err) {
        
        curve = v[0];
        
        curve->setAttributeValue(TTSymbol("active"), !muteState);
    }
}

bool Engine::getCurveMuteState(TimeProcessId boxId, const std::string & address)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTObjectBasePtr     curve;
    TTValue             v;
    TTErr               err;
    
    // get curve object at address
    err = timeProcess->sendMessage(TTSymbol("CurveGet"), toTTAddress(address), v);
    
    if (!err) {
        
        curve = v[0];
        
        curve->getAttributeValue(TTSymbol("active"), v);
        
        return !TTBoolean(v[0]);
    }
    
	return false;
}

bool Engine::setCurveSections(TimeProcessId boxId, std::string address, unsigned int argNb, const std::vector<float> & percent, const std::vector<float> & y, const std::vector<short> & sectionType, const std::vector<float> & coeff)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTObjectBasePtr     curve;
    TTValue             v;
    TTUInt32            i, nbPoints = percent.size();
    TTErr               err;
    
    // get curve object at address
    err = timeProcess->sendMessage(TTSymbol("CurveGet"), toTTAddress(address), v);
    
    if (!err) {
        
        curve = v[0];
        
        // edit value as : x1 y1 b1 x2 y2 b2
        v.resize(nbPoints * 3);
        
        for (i = 0; i < v.size(); i = i+3) {
            
            v[i] = TTFloat64(percent[i/3] / 100.);
            v[i+1] = TTFloat64(y[i/3]);
            v[i+2] = TTFloat64(coeff[i/3]) * TTFloat64(coeff[i/3]) * TTFloat64(coeff[i/3]) * TTFloat64(coeff[i/3]);
        }
        
        // set a curve parameters
        err = curve->setAttributeValue(TTSymbol("parameters"), v);
    }
    
    return err == kTTErrNone;
}

bool Engine::getCurveSections(TimeProcessId boxId, std::string address, unsigned int argNb,
                              std::vector<float> & percent,  std::vector<float> & y,  std::vector<short> & sectionType,  std::vector<float> & coeff)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTObjectBasePtr     curve;
    TTValue             v;
    TTUInt32            i;
    TTErr               err;
    
    // get curve object at address
    err = timeProcess->sendMessage(TTSymbol("CurveGet"), toTTAddress(address), v);
    
    if (!err) {
        
        curve = v[0];

        // get a curve parameters
        err = curve->getAttributeValue(TTSymbol("parameters"), v);
        
        if (!err) {

            // edit percent, y, sectionType and coeff from v : x1 y1 b1 x2 y2 b2
            for (i = 0; i < v.size(); i = i+3) {
                
                percent.push_back(TTFloat64(v[i]) * 100.);
                y.push_back(TTFloat64(v[i+1]));
                sectionType.push_back(1);
                coeff.push_back(sqrt(sqrt(TTFloat64(v[i+2]))));
            }
            
            sectionType.push_back(1);
            coeff.push_back(1);
        }
    }
    
    return err == kTTErrNone;
}

bool Engine::getCurveValues(TimeProcessId boxId, const std::string & address, unsigned int argNb, std::vector<float>& result)
{
    TTTimeProcessPtr    timeProcess = getTimeProcess(boxId);
    TTObjectBasePtr     curve;
    TTValue             v, curveValues;
    TTErr               err;
    
    // get curve object at address
    err = timeProcess->sendMessage(TTSymbol("CurveGet"), toTTAddress(address), v);

    if (!err) {
        
        curve = v[0];
        
        // get time process duration
        timeProcess->getAttributeValue(TTSymbol("duration"), v);
    
        // sample the curve
        err = curve->sendMessage(TTSymbol("Sample"), v, curveValues);
    
        // copy the curveValues into the result vector
        for (TTUInt32 i = 0; i < curveValues.size(); i++)
            result.push_back(TTFloat64(curveValues[i]));
    }
    
	return err == kTTErrNone;
}

ConditionedProcessId Engine::addTriggerPoint(TimeProcessId containingBoxId, TimeEventIndex controlPointIndex)
{
    TTTimeProcessPtr        timeProcess = getTimeProcess(containingBoxId);
    TTTimeEventPtr          timeEvent;
    TTTimeConditionPtr      timeCondition;
    ConditionedProcessId    triggerId;
    TTValue                 v, args, out;
    
    // Get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    
    timeEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Create a TTTimeCondition with no case
    m_mainScenario->sendMessage(TTSymbol("TimeConditionCreate"), args, v);
    timeCondition = TTTimeConditionPtr(TTObjectBasePtr(v[0]));
    
    // We cache the time process and the event index instead of the event itself
    triggerId = cacheConditionedProcess(containingBoxId, controlPointIndex);
    
    // We cache the TTTimeCondition
    cacheTimeCondition(triggerId, timeCondition);
    
    // note : see in setTriggerPointMessage to see how the event is linked to the condition
    
	return triggerId;
}

void Engine::removeTriggerPoint(ConditionedProcessId triggerId)
{
    TTValue             v, out;
    
    // Release the time condition
    v = TTObjectBasePtr(getTimeCondition(triggerId));
    m_mainScenario->sendMessage(TTSymbol("TimeConditionRelease"), v, out);
    
    // Uncache
    uncacheConditionedProcess(triggerId);
    
    uncacheTimeCondition(triggerId);
}

void Engine::setTriggerPointMessage(ConditionedProcessId triggerId, std::string triggerMessage)
{
    TimeEventIndex      controlPointIndex;
    TTTimeProcessPtr    timeProcess = getConditionedProcess(triggerId, controlPointIndex);
    TTTimeEventPtr      timeEvent;
    TTValue             v, out;
    
    // Get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    
    timeEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // clear the former case
    getTimeCondition(triggerId)->sendMessage(TTSymbol("CaseRemove"), TTSymbol(triggerMessage), out);
    
    // add a case to the condition
    getTimeCondition(triggerId)->sendMessage(TTSymbol("CaseAdd"), TTSymbol(triggerMessage), out);
    
    // link the event to the case
    v = TTSymbol(triggerMessage);
    v.append(TTObjectBasePtr(timeEvent));
    getTimeCondition(triggerId)->sendMessage(TTSymbol("CaseLinkEvent"), v, out);
}

std::string Engine::getTriggerPointMessage(ConditionedProcessId triggerId)
{
    TimeEventIndex          controlPointIndex;
    TTTimeProcessPtr        timeProcess = getConditionedProcess(triggerId, controlPointIndex);
    TTTimeEventPtr          timeEvent;
    TTSymbol                expression;
    TTValue                 v, out;
    
    // Get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    
    timeEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Get the expression case related to this event
    if (!getTimeCondition(triggerId)->sendMessage(TTSymbol("CaseFind"), v, out)) {
        
        expression = out[0];
        return expression.c_str();
    }
    else {
        
        string empty;
        return empty;
    }
}

TimeProcessId Engine::getTriggerPointRelatedBoxId(ConditionedProcessId triggerId)
{
    TimeEventIndex          controlPointIndex;
    TTTimeProcessPtr        timeProcess = getConditionedProcess(triggerId, controlPointIndex);
    EngineCacheMapIterator  it;
    TimeProcessId           id = NO_ID;
    
    // look for the time process id into the time process map
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        if (it->second->object == timeProcess) {
            
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

void Engine::setViewZoom(QPointF zoom)
{
    TTValue v;
    
    v.append(zoom.x());
    v.append(zoom.y());
    
    m_mainScenario->setAttributeValue(TTSymbol("viewZoom"), v);
}

QPointF Engine::getViewZoom()
{
    QPointF zoom;
    TTValue v;
    
    m_mainScenario->getAttributeValue(TTSymbol("viewZoom"), v);
    
    zoom = QPointF(v[0], v[1]);
    
    return zoom;
}

void Engine::setViewPosition(QPointF position)
{
    TTValue v;
    
    v.append(position.x());
    v.append(position.y());
    
    m_mainScenario->setAttributeValue(TTSymbol("viewPosition"), v);
}

QPointF Engine::getViewPosition()
{
    QPointF position;
    TTValue v;
    
    m_mainScenario->getAttributeValue(TTSymbol("viewPosition"), v);
    
    position = QPointF(v[0], v[1]);
    
    return position;
}

//Execution ///////////////////////////////////////////////////////////
void Engine::setGotoValue(TimeValue gotoValue)
{
    TTValue         v;
    TTObjectBasePtr scheduler;
    
    m_mainScenario->getAttributeValue(TTSymbol("scheduler"), v);
    
    scheduler = v[0];
    
    scheduler->setAttributeValue(kTTSym_offset, TTFloat64(gotoValue));
}

TimeValue Engine::getGotoValue()
{
    TTValue         v;
    TTObjectBasePtr scheduler;
    
    m_mainScenario->getAttributeValue(TTSymbol("scheduler"), v);
    
    scheduler = v[0];
    
    scheduler->getAttributeValue(kTTSym_offset, v);
    
    return TimeValue(TTFloat64(v[0]));
}

bool Engine::play()
{
    // compile the main scenario
    m_mainScenario->sendMessage(TTSymbol("Compile"));
    
    return !m_mainScenario->sendMessage(TTSymbol("Play"));
}

void Engine::pause(bool pauseValue)
{
    if (pauseValue)
        m_mainScenario->sendMessage(TTSymbol("Pause"));
    else
        m_mainScenario->sendMessage(TTSymbol("Resume"));
}

bool Engine::isPaused()
{
    TTValue         v;
    TTObjectBasePtr aScheduler;
    
    // get the scheduler object of the main scenario
    m_mainScenario->getAttributeValue(TTSymbol("scheduler"), v);
    aScheduler = TTObjectBasePtr(v[0]);
    
    aScheduler->getAttributeValue(TTSymbol("paused"), v);
    
    return TTBoolean(v[0]);
}

bool Engine::stop()
{
    return !m_mainScenario->sendMessage(TTSymbol("Stop"));
}

bool Engine::isRunning()
{
    TTValue         v;
    TTObjectBasePtr aScheduler;
    
    // get the scheduler object of the main scenario
    m_mainScenario->getAttributeValue(TTSymbol("scheduler"), v);
    aScheduler = TTObjectBasePtr(v[0]);
    
    aScheduler->getAttributeValue(TTSymbol("running"), v);
    
    return TTBoolean(v[0]);
}

TimeValue Engine::getCurrentExecutionTime()
{
    TTValue         v;
    TTUInt32        time;
    TTObjectBasePtr aScheduler;
    
    // get the scheduler object of the main scenario
    m_mainScenario->getAttributeValue(TTSymbol("scheduler"), v);
    aScheduler = TTObjectBasePtr(v[0]);
    
    aScheduler->getAttributeValue(TTSymbol("realTime"), v);
    time = TTFloat64(v[0]);
    
    return time;
}

void Engine::setExecutionSpeedFactor(float factor)
{
    TTValue         v;
    TTObjectBasePtr aScheduler;

    // get the scheduler object of the main scenario
    m_mainScenario->getAttributeValue(TTSymbol("scheduler"), v);
    aScheduler = TTObjectBasePtr(v[0]);
    
    aScheduler->setAttributeValue(TTSymbol("speed"), TTFloat64(factor));
}

float Engine::getExecutionSpeedFactor()
{
    TTValue         v;
    TTObjectBasePtr aScheduler;
    
    // get the scheduler object of the main scenario
    m_mainScenario->getAttributeValue(TTSymbol("scheduler"), v);
    aScheduler = TTObjectBasePtr(v[0]);
    
    aScheduler->getAttributeValue(TTSymbol("speed"), v);
    
    return TTFloat64(v[0]);
}

float Engine::getProcessProgression(TimeProcessId processId)
{
    TTValue         v;
    TTFloat64       time;
    TTTimeProcessPtr  timeProcess = getTimeProcess(processId);
    TTObjectBasePtr aScheduler;
    
    // get the scheduler object of the time process
    timeProcess->getAttributeValue(TTSymbol("scheduler"), v);
    aScheduler = TTObjectBasePtr(v[0]);
    
    aScheduler->getAttributeValue(TTSymbol("progression"), v);
    time = TTFloat64(v[0]);
    
    return time;
}

void Engine::trigger(ConditionedProcessId triggerId)
{
    TimeEventIndex      controlPointIndex;
    TTTimeProcessPtr    timeProcess = getConditionedProcess(triggerId, controlPointIndex);
    TTTimeEventPtr      timeEvent;
    TTValue             v, out;
    
    // Get start or end time event
    if (controlPointIndex == BEGIN_CONTROL_POINT_INDEX)
        timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    else
        timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
            
    timeEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
            
    timeEvent->sendMessage(kTTSym_Trigger);
}

void Engine::addNetworkDevice(const std::string & deviceName, const std::string & pluginToUse, const std::string & DeviceIp, const std::string & DevicePort)
{
    TTValue         v;
    TTSymbol        applicationName(deviceName);
    TTSymbol        protocolName(pluginToUse);
    TTObjectBasePtr anApplication = NULL;
    TTHash          hashParameters;
    
    // if the application doesn't already exist
    if (!getApplication(applicationName)) {
        
        // create the application
        v = applicationName;
        TTObjectBaseInstantiate(kTTSym_Application, TTObjectBaseHandle(&anApplication), v);
        
        // check if the protocol has been loaded
		if (getProtocol(protocolName)) {
            
            // register the application to the protocol
            v = applicationName;
            getProtocol(protocolName)->sendMessage(TTSymbol("registerApplication"), v, kTTValNONE);
            
            // set plugin parameters (OSC or Minuit Plugin)
            hashParameters.append(TTSymbol("ip"), TTSymbol(DeviceIp));
            
            v = TTSymbol(DevicePort);
            v.fromString();
            hashParameters.append(TTSymbol("port"), v);
            
            v = applicationName;
            v.append(TTPtr(&hashParameters));
            getProtocol(protocolName)->setAttributeValue(TTSymbol("applicationParameters"), v);
            
            // run the protocol for this application
            getProtocol(protocolName)->sendMessage(TTSymbol("Run"), applicationName, kTTValNONE);
        }
    }
}

void Engine::removeNetworkDevice(const std::string & deviceName)
{
    TTValue         v;
    TTSymbol        applicationName(deviceName);
    TTSymbol        protocolName;
    TTObjectBasePtr anApplication = getApplication(applicationName);
    
    // if the application exists
    if (anApplication) {
        
        // get the protocols of the application
        v = getApplicationProtocols(applicationName);
        protocolName = v[0]; // we register application to 1 protocol only
        
        // stop the protocol for this application
        getProtocol(protocolName)->sendMessage(TTSymbol("Stop"), applicationName, kTTValNONE);
        
        // unregister the application to the protocol
        v = TTValue(applicationName);
        getProtocol(protocolName)->sendMessage(TTSymbol("unregisterApplication"), v, kTTValNONE);
        
        // delete the application
        TTObjectBaseRelease(TTObjectBaseHandle(&anApplication));
    }
}

void Engine::sendNetworkMessage(const std::string & stringToSend)
{
#ifdef TODO_ENGINE
	m_networkController->deviceSendSetRequest(stringToSend);
#endif
}

void Engine::getNetworkDevicesName(std::vector<std::string>& devicesName, std::vector<bool>& couldSendNamespaceRequest)
{
    TTValue applicationNames, protocolNames;
    TTSymbol name;
    
    // get all application name
    TTModularApplications->getAttributeValue(TTSymbol("applicationNames"), applicationNames);
    
    for (TTUInt8 i = 0; i < applicationNames.size(); i++) {
        
        // don't return the local application
        name = applicationNames[i];
        
        if (name == getLocalApplicationName)
            continue;
        
        devicesName.push_back(name.c_str());
        
        // get all protocol names used by this application
        protocolNames = getApplicationProtocols(name);
        
        // if there is at least one protocol,
        if (protocolNames.size()) {
            
            // look if it provides namespace exploration
            name = protocolNames[0];
            couldSendNamespaceRequest.push_back(getProtocol(name)->mDiscover);
        }
    }
}

std::vector<std::string> Engine::requestNetworkSnapShot(const std::string & address)
{
    vector<string>      snapshot;
    TTAddress           anAddress = toTTAddress(address);
    TTSymbol            type;
    TTNodeDirectoryPtr  aDirectory;
    TTNodePtr           aNode;
    TTMirrorPtr         aMirror;
    TTList              nodeList;
    TTString            s;
    TTValue             v;
    
    // get the application directory
    aDirectory = getApplicationDirectory(anAddress.getDirectory());
    
    if (aDirectory) {
    
        // get the node
        if (!aDirectory->getTTNode(anAddress, &aNode)) {
            
            // get object attributes
            aMirror = TTMirrorPtr(aNode->getObject());
            if (aMirror) {
                
                type = aMirror->getName();
                
                if (type == TTSymbol("Data")) {
                    
                    // get the value attribute
                    aMirror->getAttributeValue(TTSymbol("value"), v);
                    v.toString();
                    s = TTString(v[0]);
                    
                    // append address value to the snapshot
                    snapshot.push_back(address + " " + s.data());
                }
            }
        }
    }
    
    return snapshot;
}

int
Engine::requestObjectAttributeValue(const std::string & address, const std::string & attribute, vector<string>& value){

    TTAddress           anAddress = toTTAddress(address);
    TTNodeDirectoryPtr  aDirectory;
    TTNodePtr           aNode;
    TTMirrorPtr         aMirror;
    TTString            s;
    TTValue             v;

    aDirectory = getApplicationDirectory(anAddress.getDirectory());
    value.clear();

    if (!aDirectory)
        return 1;

    if (!aDirectory->getTTNode(anAddress, &aNode)) {

        // get object attributes
        aMirror = TTMirrorPtr(aNode->getObject());

        if (aMirror) {
            if(!aMirror->getAttributeValue(TTSymbol(attribute), v)){               
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
Engine::requestObjectType(const std::string & address, std::string & nodeType){
    TTNodeDirectoryPtr  aDirectory;
    TTAddress           anAddress = toTTAddress(address);
    TTSymbol            type;
    TTMirrorPtr         aMirror;
    TTNodePtr           aNode;

    nodeType = "none";
    aDirectory = getApplicationDirectory(anAddress.getDirectory());

    if (!aDirectory)
        return 0;

    if (!aDirectory->getTTNode(anAddress, &aNode)) {

        aMirror = TTMirrorPtr(aNode->getObject());
        if (aMirror) {

            type = aMirror->getName();

            if (type != kTTSymEmpty)
                nodeType = type.c_str();
            
        }
        return 1;
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

    aDirectory = getApplicationDirectory(anAddress.getDirectory());
    children.clear();

    if (!aDirectory)
        return 0;

    if (!aDirectory->getTTNode(anAddress, &aNode)) {

        aNode->getChildren(S_WILDCARD, S_WILDCARD, nodeList);
        nodeList.sort(&TTModularCompareNodePriorityThenNameThenInstance);

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
    aDirectory = getApplicationDirectory(anAddress.getDirectory());
    
    if (!aDirectory)
        return 0;
    
    // explore the directory at this address
    // notice the tree is already built (see in initModular)
    if (!aDirectory->getTTNode(anAddress, &aNode)) {
        
        // get object attributes
        aMirror = TTMirrorPtr(aNode->getObject());
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
        nodeList.sort(&TTModularCompareNodePriorityThenNameThenInstance);

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
            aMirror = TTMirrorPtr(childNode->getObject());
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



// LOAD AND STORE

void Engine::store(std::string fileName)
{
    TTValue v, none;
    
    // Create a TTXmlHandler
    TTObject aXmlHandler(kTTSym_XmlHandler);
    
    // Pass the application manager and the main scenario object
    v = TTObjectBasePtr(TTModularApplications);
    v.append(m_mainScenario);
    aXmlHandler.set(kTTSym_object, v);
    
    // Write
    aXmlHandler.send(kTTSym_Write, TTSymbol(fileName), none);
}

void Engine::load(std::string fileName)
{
    TTValue             v, objects, none;
    TTObjectBasePtr     timeProcess, timeEvent, timeCondition;
    TTSymbol            name;
    TimeProcessId       timeProcessId;
    IntervalId          relationId;
    ConditionedProcessId triggerId;
    
    // Clear all the EngineCacheMaps
    // note : this should be useless because all elements are removed by the maquette
    clearTimeProcess();
    clearInterval();
    clearConditionedProcess();
    clearTimeCondition();
    
    // Create a TTXmlHandler
    TTObject aXmlHandler(kTTSym_XmlHandler);
    
    // Pass the application manager and the main scenario object
    v = TTObjectBasePtr(TTModularApplications);
    v.append(m_mainScenario);
    aXmlHandler.set(kTTSym_object, v);
    
    // Read
    aXmlHandler.send(kTTSym_Read, TTSymbol(fileName), none);
    
    // Rebuild all the EngineCacheMaps from the main scenario content
    
    // get all TTTimeProcesses
    m_mainScenario->getAttributeValue(TTSymbol("timeProcesses"), objects);
    
    // for all time process
    for (TTUInt32 i = 0; i < objects.size(); i++) {
        
        timeProcess = objects[i];
        
        // for each Automation process
        if (timeProcess->getName() == TTSymbol("Automation")) {
            
            timeProcess->getAttributeValue(kTTSym_name, v);
            name = v[0];
            
            // Cache it and get an unique id for this process
            timeProcessId = cacheTimeProcess((TTTimeProcessPtr)timeProcess, name.c_str());
            
            // if the Start event of the Automation process is conditioned
            timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
            timeEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
            
            timeEvent->getAttributeValue(TTSymbol("condition"), v);
            timeCondition = TTTimeConditionPtr(TTObjectBasePtr(v[0]));
            
            if (timeCondition != NULL) {
            
                // We cache the time process and the event index instead of the event itself
                triggerId = cacheConditionedProcess(timeProcessId, BEGIN_CONTROL_POINT_INDEX);
            
                // We cache the TTTimeCondition
                cacheTimeCondition(triggerId, (TTTimeConditionPtr)timeCondition);
            }
            
            // if the End event of the Automation process is conditioned
            timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
            timeEvent = TTTimeEventPtr(TTObjectBasePtr(v[0]));
            
            timeEvent->getAttributeValue(TTSymbol("condition"), v);
            timeCondition = TTTimeConditionPtr(TTObjectBasePtr(v[0]));
            
            if (timeCondition != NULL) {
                
                // We cache the time process and the event index instead of the event itself
                triggerId = cacheConditionedProcess(timeProcessId, END_CONTROL_POINT_INDEX);
            
                // We cache the TTTimeCondition
                cacheTimeCondition(triggerId, (TTTimeConditionPtr)timeCondition);
            }
        }
        
        // for each Interval process
        else if (timeProcess->getName() == TTSymbol("Interval")) {
            
            // Cache it and get an unique id for this process
            relationId = cacheInterval((TTTimeProcessPtr)timeProcess);
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
    
	while(isRunning()){
		if ((getCurrentExecutionTime()/60)%2 == 0) {
			if (mustDisplay) {
				std::system("clear");
				for (unsigned int i = 0; i < boxesId.size(); ++i) {
					unsigned int processPercent;
                    
					processPercent = getProcessProgression(boxesId[i]);
                    
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

void TimeEventReadyAttributeCallback(TTPtr baton, const TTValue& value)
{
    TTValuePtr              b;
    EnginePtr               engine;
    ConditionedProcessId    triggerId;
    TTBoolean               isReady;
	
	// unpack baton (engine, triggerId)
	b = (TTValuePtr)baton;
	engine = EnginePtr((TTPtr)(*b)[0]);
    triggerId = ConditionedProcessId((*b)[1]);
	
	// Unpack data (isReady)
	isReady = value[0];
    
	if (engine->m_TimeEventReadyAttributeCallback != NULL)
		engine->m_TimeEventReadyAttributeCallback(triggerId, isReady);
}

void TimeProcessSchedulerRunningAttributeCallback(TTPtr baton, const TTValue& value)
{
    TTValuePtr      b;
    EnginePtr       engine;
    TimeProcessId   boxId;
    TTBoolean       running;
	
	// unpack baton (engine, boxId)
	b = (TTValuePtr)baton;
	engine = EnginePtr((TTPtr)(*b)[0]);
    boxId = TTUInt32((*b)[1]);
	
	// Unpack data (running)
	running = value[0];
    
    iscoreEngineDebug {
        
        if (running)
            TTLogMessage("TimeProcess %ld starts at %ld ms\n", boxId, engine->getCurrentExecutionTime());
        else
            TTLogMessage("TimeProcess %ld ends at %ld ms\n", boxId, engine->getCurrentExecutionTime());
    }
    
    // don't pass main scenario running state
    if (boxId > 1 && engine->m_TimeProcessSchedulerRunningAttributeCallback != NULL)
        engine->m_TimeProcessSchedulerRunningAttributeCallback(boxId, running);

}

void TransportDataValueCallback(TTPtr baton, const TTValue& value)
{
    TTValuePtr          b;
    EnginePtr           engine;
    TTSymbol            transport;
	
	// unpack baton (engine, transport)
	b = (TTValuePtr)baton;
	engine = EnginePtr((TTPtr)(*b)[0]);
    transport = (*b)[0];
	
    if (transport == TTSymbol("Play"))
        engine->play();
    
    else if (transport == TTSymbol("Stop"))
        engine->stop();
    
    else if (transport == TTSymbol("Pause"))
        engine->pause(!engine->isPaused());
    
    else if (transport == TTSymbol("Rewind")) {
        engine->stop();
        engine->setGotoValue(0);
    }
    else if (transport == TTSymbol("StartPoint")) {
        
        if (value.size() == 1)
            if (value[0].type() == kTypeUInt32)
                engine->setGotoValue(value[0]);
        
    }
    else if (transport == TTSymbol("Speed")) {
        
        if (value.size() == 1)
            if (value[0].type() == kTypeFloat32)
                engine->setExecutionSpeedFactor(value[0]);
    }
    
    engine->m_TransportDataValueCallback(transport, value);
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

#if 0
#pragma mark -
#pragma mark Miscellaneous Methods
#endif

// TODO : this should move into a TTModularAPI file
TTErr TTModularCreateData(TTSymbol service, TTValuePtr baton, TTFunctionWithBatonAndValue valueCallback, TTObjectBasePtr *returnedData)
{
    TTValue			args;
	TTObjectBasePtr	returnValueCallback;
	
	// prepare arguments
	returnValueCallback = NULL;
	TTObjectBaseInstantiate(TTSymbol("callback"), TTObjectBaseHandle(&returnValueCallback), kTTValNONE);
    
	returnValueCallback->setAttributeValue(kTTSym_baton, TTPtr(baton));
	returnValueCallback->setAttributeValue(kTTSym_function, TTPtr(valueCallback));
    
	args.append(returnValueCallback);
	
	args.append(service);
	
    // create data
	*returnedData = NULL;
	TTObjectBaseInstantiate(kTTSym_Data, TTObjectBaseHandle(returnedData), args);
	
	return kTTErrNone;
}

// TODO : this should move into a TTModularAPI file
TTErr TTModularRegisterObject(TTAddress address, TTObjectBasePtr object)
{
    TTNodePtr returnedTTNode;
    TTBoolean nodeCreated;
    
    return getLocalDirectory->TTNodeCreate(address, object, NULL, &returnedTTNode, &nodeCreated);
}

TTBoolean TTModularCompareNodePriorityThenNameThenInstance(TTValue& v1, TTValue& v2)
{
	TTNodePtr	n1, n2;
	TTObjectBasePtr o1, o2;
	TTValue		v;
    TTValue    name1;
    TTValue    name2;
    TTValue    instance1;
    TTValue    instance2;
	TTInt32		p1 = 0;
	TTInt32		p2 = 0;
	
	// get priority of v1
	n1 = TTNodePtr((TTPtr)v1[0]);
	if (n1) {
        
        name1 = n1->getName();
        instance1 = n1->getInstance();
		o1 = n1->getObject();
		
		if (o1)
			if (!o1->getAttributeValue(kTTSym_priority, v))
				p1 = v[0];
	}
	
	// get priority of v2
	n2 = TTNodePtr((TTPtr)v2[0]);
	if (n2) {
        
        name2 = n2->getName();
        instance2 = n2->getInstance();
		o2 = n2->getObject();
		
		if (o2)
			if (!o2->getAttributeValue(kTTSym_priority, v))
				p2 = v[0];
	}
	
	if (p1 == p2) {
        
        if (name1 == name2) {
            
            if (instance1 == instance2)
                return v1 < v2;
            else
                return instance1 < instance2;
        }
        else
            return name1 < name2;
    }
	
	if (p1 == 0) return NO;
	if (p2 == 0) return YES;
	
	return p1 < p2;
}

