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

Engine::Engine(void(*interactiveEventActiveAttributeCallback)(InteractiveEventId, bool),
               void(*timeProcessSchedulerRunningAttributeCallback)(TimeProcessId, bool),
               void(*transportDataValueCallback)(TTSymbol&, const TTValue&))
{
    m_InteractiveEventActiveAttributeCallback = interactiveEventActiveAttributeCallback;
    m_TimeProcessSchedulerRunningAttributeCallback = timeProcessSchedulerRunningAttributeCallback;
    m_TransportDataValueCallback = transportDataValueCallback;
    
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
    TTString        pluginsDir = "/usr/local/lib/IScore";
    TTHash          hashParameters;
    
    // this initializes the Modular framework and loads protocol plugins (in /usr/local/jamoma/extensions folder)
    TTModularInit();
    
    // create a local application named i-score
    TTModularCreateLocalApplication(applicationName, configFile);
    
    // get i-score application
    iscore = getLocalApplication;
    
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
    
    // create an application called myApplication
    args = TTValue(TTSymbol("myApplication"));
    TTObjectBaseInstantiate(kTTSym_Application, TTObjectBaseHandle(&anApplication), args);
    
    ////////////
    // Example : Register i-score and myApplication to the Minuit protocol
    ////////////
    
    // check if the Minuit protocol has been loaded
    if (getProtocol(TTSymbol("Minuit"))) {
        
        // register the local application to the Minuit protocol
        v = TTValue(TTSymbol(applicationName));
        getProtocol(TTSymbol("Minuit"))->sendMessage(TTSymbol("registerApplication"), v, kTTValNONE);
        
        // set the Minuit parameters for the local application
        hashParameters.clear();
        hashParameters.append(TTSymbol("port"), 8002);
        hashParameters.append(TTSymbol("ip"), TTSymbol("127.0.0.1"));
        
        v = TTValue(TTSymbol(applicationName));
        v.append(TTPtr(&hashParameters));
        getProtocol(TTSymbol("Minuit"))->setAttributeValue(TTSymbol("applicationParameters"), v);
        
        // register this application to the Minuit protocol
        v = TTValue(TTSymbol("myApplication"));
        getProtocol(TTSymbol("Minuit"))->sendMessage(TTSymbol("registerApplication"), v, kTTValNONE);
        
        // set the Minuit parameters for the distant application
        hashParameters.clear();
        hashParameters.append(TTSymbol("port"), 9998);
        hashParameters.append(TTSymbol("ip"), TTSymbol("127.0.0.1"));
        
        v = TTValue(TTSymbol("myApplication"));
        v.append(TTPtr(&hashParameters));
        getProtocol(TTSymbol("Minuit"))->setAttributeValue(TTSymbol("applicationParameters"), v);
        
        // run the Minuit protocol
        TTModularApplications->sendMessage(TTSymbol("ProtocolRun"), TTSymbol("Minuit"), kTTValNONE);
    }
    
    ////////////
    // Example : Read the namespace of myApplication from a namespace file
    ////////////
    
    // create a TTXmlHandler class to parse a xml namespace file
    TTXmlHandlerPtr myXmlHandler = NULL;
    TTObjectBaseInstantiate(kTTSym_XmlHandler, TTObjectBaseHandle(&myXmlHandler), kTTValNONE);
    
    // prepare the TTXmlHandler to pass the result of the parsing to myApplication
    v = TTValue(anApplication);
    myXmlHandler->setAttributeValue(TTSymbol("object"), v);
    
    // read a namespace file
    err = myXmlHandler->sendMessage(TTSymbol("Read"), TTSymbol("/Users/WALL-E/Documents/Jamoma/Modules/Modular/implementations/MaxMSP/jcom.modular/remoteApp - namespace.xml"), kTTValNONE);
    
    if (!err) {
        // get the root of the TNodeDirectory of myApplication
        // note : the TTNodeDirectory is a tree structure used to registered and retrieve TTObjects using TTAddress
        TTNodeDirectoryPtr anApplicationDirectory = getApplicationDirectory(TTSymbol("myApplication"));
        
        // return all addresses of the TTNodeDirectory
        std::cout << "__ Dump myApplication directory __" << std::endl;
        
        // start to dump addresses recursilvely from the root of the directory
        dumpAddressBelow(anApplicationDirectory->getRoot());
        
        std::cout << "__________________________________" << std::endl;
    }
    
    ////////////
    // Example : Build the namespace of myApplication using discovery feature (if the protocol provides it)
    ////////////
    
    // you can create an OSC receive on the 9998 port to see that a namespace request is sent by i-score (using Pure Data for example)
    anApplication->sendMessage(TTSymbol("DirectoryBuild"));
}

void Engine::initScore()
{
    TTValue args;
    
    // this initializes the Score framework
    TTScoreInit();
    
    // Create the main scenario
    m_mainScenario = NULL;
    TTObjectBaseInstantiate(TTSymbol("Scenario"), TTObjectBaseHandle(&m_mainScenario), args);
    
    // TODO : create a TTCallback to observe each time process scheduler running attribute (using TimeProcessSchedulerRunningAttributeCallback)
    
    // Store the main scenario (so ROOT_BOX_ID is 1)
    cacheTimeProcess(m_mainScenario);
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
    TTObjectBaseRelease(TTObjectBaseHandle(&m_mainScenario));
}

TimeProcessId Engine::cacheTimeProcess(TimeProcessPtr timeProcess)
{
    TimeProcessId id;
    EngineCacheElementPtr e;
    
    e = new EngineCacheElement();
    e->object = TTObjectBasePtr(timeProcess);
    
    id = m_timeProcessMap.size() + 1;
    m_timeProcessMap[id] = e;
    
    return id;
}

IntervalId Engine::cacheInterval(TimeProcessPtr timeProcess)
{
    TimeProcessId id;
    EngineCacheElementPtr e;
    
    e = new EngineCacheElement();
    e->object = TTObjectBasePtr(timeProcess);
    
    id = m_timeProcessMap.size() + 1;
    m_intervalMap[id] = e;
    
    return id;
}

// Edition ////////////////////////////////////////////////////////////////////////

TimeProcessId Engine::addBox(TimeValue boxBeginPos, TimeValue boxLength, TimeProcessId motherId)
{
    TimeProcessPtr  timeProcess;
    TimeProcessId   boxId;
    TTValue         v;
    
    // Create a new automation time process (2 static events are automatically created during creation)
    timeProcess = NULL;
    TTObjectBaseInstantiate(TTSymbol("Automation"), TTObjectBaseHandle(&timeProcess), kTTValNONE);
    
    // Add the time process to the main scenario
    // TODO : get the parent process using motherID and add the time process to this parent process
    v = TTValue(TTObjectBasePtr(timeProcess));
    // CRASH_ENGINE  m_mainScenario->sendMessage(TTSymbol("TimeProcessAdd"), v, kTTValNONE);
    
    // Cache it and get an unique id for this process
    boxId = cacheTimeProcess(timeProcess);
    
    // Set the start and end event dates
    timeProcess->setAttributeValue(TTSymbol("startDate"), boxBeginPos);
    timeProcess->setAttributeValue(TTSymbol("endDate"), boxBeginPos+boxLength);
    
    // TODO : create a TTCallback to observe each time process event active attribute (using InteractiveEventActiveAttributeCallback)
    // getTimeProcess(boxId)->appendObserver() ?
    
    // TODO : create a TTCallback to observe each time process scheduler running attribute (using TimeProcessSchedulerRunningAttributeCallback)
    // getTimeProcess(boxId)->appendObserver() ?
    
	return boxId;
}

void Engine::removeBox(TimeProcessId boxId)
{
    TimeProcessPtr  timeProcess;
    
    // Retreive the time process using the boxId
    timeProcess = getTimeProcess(boxId);
    
    // TODO : delete TTCallback used to observe each time process scheduler running attribute
    // getTimeProcess(boxId)->removeObserver() ?
    
    // Delete the time process (the process is removed from the scenario during the destruction)
    TTObjectBaseRelease(TTObjectBaseHandle(&timeProcess));
}

IntervalId Engine::addTemporalRelation(TimeProcessId boxId1,
                                       TimeEventIndex controlPoint1,
                                       TimeProcessId boxId2,
                                       TimeEventIndex controlPoint2,
                                       TemporalRelationType type,
                                       vector<TimeProcessId>& movedBoxes)
{
    TimeProcessPtr  timeProcess, tp1, tp2;
    IntervalId      relationId;
    TTValue         v;
    
    // Create a new interval time process
    timeProcess = NULL;
    TTObjectBaseInstantiate(TTSymbol("Interval"), TTObjectBaseHandle(&timeProcess), kTTValNONE);
    
    // Cache it and get an unique id for this process
    relationId = cacheInterval(timeProcess);
    
    // Get the events from the given box ids and pass them to the time process
    tp1 = getTimeProcess(boxId1);
    tp2 = getTimeProcess(boxId2);
    
    if (controlPoint1 == BEGIN_CONTROL_POINT_INDEX)
        tp1->getAttributeValue(TTSymbol("startEvent"), v);
    else
        tp1->getAttributeValue(TTSymbol("endEvent"), v);
    
    timeProcess->setAttributeValue(TTSymbol("startEvent"), v);
    
    if (controlPoint2 == BEGIN_CONTROL_POINT_INDEX)
        tp2->getAttributeValue(TTSymbol("startEvent"), v);
    else
        tp2->getAttributeValue(TTSymbol("endEvent"), v);
    
    timeProcess->setAttributeValue(TTSymbol("endEvent"), v);
    
    // Add the time process to the main scenario
    v = TTValue(TTObjectBasePtr(timeProcess));
    m_mainScenario->sendMessage(TTSymbol("TimeProcessAdd"), v, kTTValNONE);
    
    // TODO : how to fill the movedBoxes ? return the entire timeProcessMap
    
	return relationId;
}

void Engine::removeTemporalRelation(IntervalId relationId)
{
    TimeProcessPtr  timeProcess;
    
    // Retreive the time process using the relationId
    timeProcess = getInterval(relationId);
    
    // Delete the time process (the process is removed from the scenario during the destruction)
    TTObjectBaseRelease(TTObjectBaseHandle(&timeProcess));
}

void Engine::changeTemporalRelationBounds(IntervalId relationId, TimeValue minBound, TimeValue maxBound, vector<TimeProcessId>& movedBoxes)
{
    TimeProcessPtr  timeProcess = getInterval(relationId);
    TimeEventPtr    event;
    TTValue         v;
    
	timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    event = TimeEventPtr(TTObjectBasePtr(v[0]));
    
    event->setAttributeValue(TTSymbol("dateMin"), minBound);
    event->setAttributeValue(TTSymbol("dateMax"), maxBound);
    
    // TODO : how to fill the movedBoxes ? return the entire timeProcessMap
}

bool Engine::isTemporalRelationExisting(TimeProcessId boxId1, TimeEventIndex controlPoint1, TimeProcessId boxId2, TimeEventIndex controlPoint2)
{
    TTValue                 v1, v2, v;
    TimeProcessPtr          timeProcess, tp1, tp2;
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
        
        timeProcess = TimeProcessPtr(TTObjectBasePtr(it->second->object));
        
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
	TimeProcessPtr          timeProcess = getInterval(relationId);
    TimeEventPtr            event;
    TimeProcessId           found = NO_ID;
    EngineCacheMapIterator  it;
    
    // get the start event of the interval
	timeProcess->getAttributeValue(TTSymbol("startEvent"), v);
    event = TimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        timeProcess = TimeProcessPtr(TTObjectBasePtr(it->second->object));
        
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
	return BEGIN_CONTROL_POINT_INDEX;
}

TimeProcessId Engine::getRelationSecondBoxId(IntervalId relationId)
{
    TTValue                 v, vt;
	TimeProcessPtr          timeProcess = getInterval(relationId);
    TimeEventPtr            event;
    TimeProcessId           found = NO_ID;
    EngineCacheMapIterator  it;
    
    // Get the end event of the interval
	timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    event = TimeEventPtr(TTObjectBasePtr(v[0]));
    
    // Look into the automation map to retreive an automation with the same event
    for (it = m_timeProcessMap.begin(); it != m_timeProcessMap.end(); ++it) {
        
        timeProcess = TimeProcessPtr(TTObjectBasePtr(it->second->object));
        
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
	return END_CONTROL_POINT_INDEX;
}

TimeValue Engine::getRelationMinBound(IntervalId relationId)
{
	TimeProcessPtr  timeProcess = getInterval(relationId);
    TimeEventPtr    event;
    TTValue         v;
    
	timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    event = TimeEventPtr(TTObjectBasePtr(v[0]));
    
    event->getAttributeValue(TTSymbol("dateMin"), v);
    
    return v[0];
}

TimeValue Engine::getRelationMaxBound(IntervalId relationId)
{
    TimeProcessPtr  timeProcess = getInterval(relationId);
    TimeEventPtr    event;
    TTValue         v;
    
	timeProcess->getAttributeValue(TTSymbol("endEvent"), v);
    event = TimeEventPtr(TTObjectBasePtr(v[0]));
    
    event->getAttributeValue(TTSymbol("dateMax"), v);
    
    return v[0];
}

bool Engine::performBoxEditing(TimeProcessId boxId, TimeValue start, TimeValue end, vector<TimeProcessId>& movedBoxes, TimeValue maxModification)
{
    TimeProcessPtr  timeProcess = getTimeProcess(boxId);
    
	timeProcess->setAttributeValue(TTSymbol("startDate"), start);
    timeProcess->setAttributeValue(TTSymbol("endDate"), end);
    
    // TODO : how to fill the movedBoxes ? return the entire timeProcessMap
}

TimeValue Engine::getBoxBeginTime(TimeProcessId boxId)
{
	TimeProcessPtr  timeProcess = getTimeProcess(boxId);
    TTValue         v;
    
	timeProcess->getAttributeValue(TTSymbol("startDate"), v);
    
    return v[0];
}

TimeValue Engine::getBoxEndTime(TimeProcessId boxId)
{
    TimeProcessPtr  timeProcess = getTimeProcess(boxId);
    TTValue         v;
    
	timeProcess->getAttributeValue(TTSymbol("endDate"), v);
    
    return v[0];
}

TimeValue Engine::getBoxDuration(TimeProcessId boxId)
{
    TimeProcessPtr  timeProcess = getTimeProcess(boxId);
    TTValue         v;
    
	timeProcess->getAttributeValue(TTSymbol("duration"), v);
    
    return v[0];
}

int Engine::getBoxNbCtrlPoints(TimeProcessId boxId)
{
	TimeProcessPtr  timeProcess = getTimeProcess(boxId);
    TTValue         v;
    
	timeProcess->getAttributeValue(TTSymbol("intermediateEvents"), v);
    
    return v.size() + 2; // becasue there is always a start and an end event too
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
    TimeProcessPtr  timeProcess = getTimeProcess(boxId);
 #ifdef TODO_ENGINE   
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
		ControlPoint* currentControlPoint = m_editor->getBoxById(boxId)->getControlPoint(controlPointIndex);
		currentSendOSCProcess->addMessages(messageToSend, currentControlPoint->getProcessStepId(), muteState);
	}
#endif
}

void Engine::getCtrlPointMessagesToSend(TimeProcessId boxId, TimeEventIndex controlPointIndex, std::vector<std::string>& messages)
{
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
		ControlPoint* currentControlPoint = m_editor->getBoxById(boxId)->getControlPoint(controlPointIndex);
        
		currentSendOSCProcess->getMessages(messages, currentControlPoint->getProcessStepId());
	}
#endif
}

void Engine::setCtrlPointMutingState(TimeProcessId boxId, TimeEventIndex controlPointIndex, bool mute)
{
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
		ControlPoint* currentControlPoint = m_editor->getBoxById(boxId)->getControlPoint(controlPointIndex);
		currentSendOSCProcess->setMessageMuteState(currentControlPoint->getProcessStepId(), mute);
	}
#endif
}

//CURVES ////////////////////////////////////////////////////////////////////////////////////

void Engine::addCurve(TimeProcessId boxId, const std::string & address)
{
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
        
		currentSendOSCProcess->addCurves(address);
        
	}
#endif
}

void Engine::removeCurve(TimeProcessId boxId, const std::string & address)
{
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
        
		currentSendOSCProcess->removeCurves(address);
        
	}
#endif
}

void Engine::clearCurves(TimeProcessId boxId)
{
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
        
		currentSendOSCProcess->removeAllCurves();
	}
#endif
}

std::vector<std::string> Engine::getCurvesAddress(TimeProcessId boxId)
{
    std::vector<std::string> addressToReturn;
    
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
        
		addressToReturn = currentSendOSCProcess->getCurvesAdress();
	} else {
		//TODO : exception
	}
#endif
    
	return addressToReturn;
}

void Engine::setCurveSampleRate(TimeProcessId boxId, const std::string & address, unsigned int nbSamplesBySec)
{
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
        
		currentSendOSCProcess->setNbSamplesBySec(address, nbSamplesBySec);
	}
#endif
}

unsigned int Engine::getCurveSampleRate(TimeProcessId boxId, const std::string & address)
{
    unsigned int sampleRate = 0;
    
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
        
		sampleRate = currentSendOSCProcess->getNbSamplesBySec(address);
	} else {
		//TODO : exception
	}
#endif
    
	return sampleRate;
}

void Engine::setCurveRedundancy(TimeProcessId boxId, const std::string & address, bool redondancy)
{
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
		currentSendOSCProcess->setAvoidRedondanceInformation(address, !redondancy);
	}
#endif
}

bool Engine::getCurveRedundancy(TimeProcessId boxId, const std::string & address)
{
	bool curveRedundancyInformation = false;
    
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
		curveRedundancyInformation = !currentSendOSCProcess->getAvoidRedondanceInformation(address);
	} else {
		//TODO : exception
	}
#endif
    
	return curveRedundancyInformation;
}

void Engine::setCurveMuteState(TimeProcessId boxId, const std::string & address, bool muteState)
{
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
		currentSendOSCProcess->setCurveMuteStateInformation(address, muteState);
	}
#endif
}

bool Engine::getCurveMuteState(TimeProcessId boxId, const std::string & address)
{
	bool curveMuteState = false;
    
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);

	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
		curveMuteState = currentSendOSCProcess->getCurveMuteStateInformation(address);
	} else {
		//TODO : exception
	}
#endif
    
	return curveMuteState;
}

void Engine::getCurveArgTypes(std::string stringToParse, std::vector<std::string>& result)
{
#ifdef TODO_ENGINE    
	result.clear();
    
	StringParser addressAndArgs(stringToParse);
    
	result.push_back(addressAndArgs.getAddress());
    
	for (unsigned int i = 0; i < addressAndArgs.getNbArg(); ++i) {
		StringParser::argType currentType = addressAndArgs.getType(i);
        
		if (currentType == StringParser::TYPE_FLOAT) {
			result.push_back("FLOAT");
		} else if (currentType == StringParser::TYPE_INT) {
			result.push_back("INT");
		} else if (currentType == StringParser::TYPE_STRING) {
			result.push_back("STRING");
		} else {
			result.push_back("SYMBOL");
		}
	}
#endif
}

bool Engine::setCurveSections(TimeProcessId boxId, std::string address, unsigned int argNb, const std::vector<float> & percent, const std::vector<float> & y, const std::vector<short> & sectionType, const std::vector<float> & coeff)
{
#ifdef TODO_ENGINE
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
        
		return currentSendOSCProcess->setCurvesSections(address, argNb, percent, y, sectionType, coeff);
	}
    
#endif
    
	return false;
}

bool Engine::getCurveSections(TimeProcessId boxId, std::string address, unsigned int argNb,
                              std::vector<float> & percent,  std::vector<float> & y,  std::vector<short> & sectionType,  std::vector<float> & coeff)
{
#ifdef TODO_ENGINE    
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
        
		return currentSendOSCProcess->getCurveSections(address, argNb, percent, y, sectionType, coeff);
	}
    
#endif
    
	return false;
}

bool Engine::getCurveValues(TimeProcessId boxId, const std::string & address, unsigned int argNb, std::vector<float>& result)
{
#ifdef TODO_ENGINE
	result.clear();
	ECOProcess* currentProcess = m_executionMachine->getProcess(boxId);
    
	if ((currentProcess->getType() == PROCESS_TYPE_NETWORK_MESSAGE_TO_SEND)) {
		SendNetworkMessageProcess* currentSendOSCProcess = (SendNetworkMessageProcess*) currentProcess;
        
		return currentSendOSCProcess->getCurves(address, argNb, getBoxEndTime(boxId) - getBoxBeginTime(boxId), BEGIN_CONTROL_POINT_INDEX, END_CONTROL_POINT_INDEX, result);
	}
#endif
    
    return false;
}

InteractiveEventId Engine::addTriggerPoint(TimeProcessId containingBoxId)
{
#ifdef TODO_ENGINE
	return m_editor->addTriggerPoint(containingBoxId);
#endif
    return NO_ID;
}

void Engine::removeTriggerPoint(InteractiveEventId triggerId)
{
#ifdef TODO_ENGINE
	m_editor->removeTriggerPoint(triggerId);
#endif
}

bool Engine::assignCtrlPointToTriggerPoint(InteractiveEventId triggerId, TimeProcessId boxId, TimeEventIndex controlPointIndex)
{
#ifdef TODO_ENGINE
	return m_editor->setTriggerPointRelatedControlPoint(triggerId, boxId, controlPointIndex);
#endif
    
    return false;
}

void Engine::setTriggerPointMessage(InteractiveEventId triggerId, std::string triggerMessage)
{
#ifdef TODO_ENGINE
	m_editor->setTriggerPointMessage(triggerId, triggerMessage);
#endif
}

std::string Engine::getTriggerPointMessage(InteractiveEventId triggerId)
{
#ifdef TODO_ENGINE
	return m_editor->getTriggerPointMessage(triggerId);
#endif
    
    std::string empty;
    return empty;
}

TimeProcessId Engine::getTriggerPointRelatedBoxId(InteractiveEventId triggerId)
{
#ifdef TODO_ENGINE
	return m_editor->getTriggerPointRelatedBoxId(triggerId);
#endif
    
    return NO_ID;
}

TimeEventIndex Engine::getTriggerPointRelatedCtrlPointIndex(InteractiveEventId triggerId)
{
#ifdef TODO_ENGINE
	return m_editor->getTriggerPointRelatedControlPointIndex(triggerId);
#endif
    
    return 0;
}

void Engine::getBoxesId(vector<TimeProcessId>& boxesID)
{
#ifdef TODO_ENGINE    
	m_editor->getAllBoxesId(boxesID);
#endif
}

void Engine::getRelationsId(vector<IntervalId>& relationsID)
{
#ifdef TODO_ENGINE
	m_editor->getAllAntPostRelationsId(relationsID);
#endif
}

void Engine::getTriggersPointId(vector<InteractiveEventId>& triggersID)
{
#ifdef TODO_ENGINE
	m_editor->getAllTriggersId(triggersID);
#endif
}

//Execution ///////////////////////////////////////////////////////////
void Engine::setGotoValue(TimeValue gotoValue)
{
#ifdef TODO_ENGINE    
	m_executionMachine->setGotoInformation(gotoValue);
#endif
}

TimeValue Engine::getGotoValue()
{
#ifdef TODO_ENGINE    
	return m_executionMachine->getGotoInformation();
#endif
}

bool Engine::play()
{
	if (!isRunning()) {
		compile();
		return run();
        
	} else {
		return false;
	}
}

void Engine::pause(bool pauseValue)
{
#ifdef TODO_ENGINE
	m_executionMachine->pause(pauseValue);
#endif
}

bool Engine::isPaused()
{
#ifdef TODO_ENGINE    
	return m_executionMachine->isPaused();
#endif
    
    return NO;
}

bool Engine::stop()
{
#ifdef TODO_ENGINE    
	return m_executionMachine->stop();
#endif
    
    return NO;
}

bool Engine::isRunning()
{
#ifdef TODO_ENGINE
	return m_executionMachine->isRunning();
#endif
}

void Engine::compile()
{
#ifdef TODO_ENGINE    
	m_executionMachine->reset();
    
	std::map<unsigned int, StoryLine> hierarchyStoryLine;
    
	computeHierarchyStoryLine(ROOT_BOX_ID, m_editor->getCSP(), hierarchyStoryLine);
    
    //	StoryLine storyLine(m_editor->getCSP());
    
	if (hierarchyStoryLine[ROOT_BOX_ID].m_constrainedBoxes.size() >= 1) {
		m_executionMachine->compileECO(hierarchyStoryLine, m_executionMachine->getGotoInformation());
	}
#endif
}

bool Engine::run()
{
#ifdef TODO_ENGINE    
	bool runIsOk = m_executionMachine->run();
    
	return runIsOk;
#endif
    
    return NO;
}

TimeValue Engine::getCurrentExecutionTime()
{
#ifdef TODO_ENGINE    
	return m_executionMachine->getTimeInMs();
#endif
    
    return 0;
}

void Engine::setExecutionSpeedFactor(float factor)
{
#ifdef TODO_ENGINE
	m_executionMachine->setSpeedFactor(factor);
#endif
}

float Engine::getExecutionSpeedFactor()
{
#ifdef TODO_ENGINE    
	return m_executionMachine->getSpeedFactor();
#endif
    
    return 1.;
}

float Engine::getProcessProgression(TimeProcessId processId)
{
 #ifdef TODO_ENGINE   
	if (m_executionMachine->isRunning()) {
		return m_executionMachine->getProcessProgressionPercent(processId);
	} else {
		return 0;
	}
#endif
    
    return 0.;
}

bool Engine::receiveNetworkMessage(std::string netMessage)
{
#ifdef TODO_ENGINE    
	if (m_executionMachine->isRunning()) {
		return m_executionMachine->receiveNetworkMessage(netMessage);
	} else {
		return false;
	}
#endif
}

void Engine::simulateNetworkMessageReception(const std::string & netMessage)
{
#ifdef TODO_ENGINE    
	std::string value("");
	receiveNetworkMessageCallBack(this, netMessage, "", value);
    //	if (m_executionMachine->isRunning()) {
    //		m_executionMachine->receiveNetworkMessage(netMessage);
    //	}
#endif
}

void Engine::addNetworkDevice(const std::string & deviceName, const std::string & pluginToUse, const std::string & DeviceIp, const std::string & DevicePort)
{
#ifdef TODO_ENGINE    
	//m_networkController->addDevice(deviceName, pluginToUse, DeviceIp, DevicePort);
	std::map<std::string, std::string>* commParameters = new std::map<std::string, std::string>();
    
	(*commParameters)["ip"] = DeviceIp;
	(*commParameters)["port"] = DevicePort;
    
	m_networkController->deviceAdd(deviceName, pluginToUse, commParameters);
    
	delete commParameters;
#endif
}

void Engine::removeNetworkDevice(const std::string & deviceName)
{
#ifdef TODO_ENGINE    
	m_networkController->deviceRemove(deviceName);
#endif
}

void Engine::sendNetworkMessage(const std::string & stringToSend)
{
#ifdef TODO_ENGINE
	m_networkController->deviceSendSetRequest(stringToSend);
#endif
}

void Engine::getNetworkDevicesName(std::vector<std::string>& devicesName, std::vector<bool>& couldSendNamespaceRequest)
{
#ifdef TODO_ENGINE    
	std::map<std::string, Device*> mapDevices = *(m_networkController->deviceGetCurrent());
    
	map<string, Device*>::iterator it = mapDevices.begin();
    
	while (it != mapDevices.end()) {
		if (m_networkController->deviceIsVisible(it->first)) {
			devicesName.push_back(it->first);
			couldSendNamespaceRequest.push_back(m_networkController->deviceUnderstandDiscoverRequest(it->first));
		}
        
		++it;
	}
#endif
}

std::vector<std::string> Engine::requestNetworkSnapShot(const std::string & address)
{
#ifdef TODO_ENGINE
	return m_networkController->deviceSnapshot(address);
#endif
    
    std::vector<std::string> empty;
    return empty;
}

int Engine::requestNetworkNamespace(const std::string & address, vector<string>& nodes, vector<string>& leaves, vector<string>& attributs, vector<string>& attributsValue)
{
#ifdef TODO_ENGINE
	return m_networkController->deviceSendDiscoverRequest(address, &nodes, &leaves, &attributs, &attributsValue);
#endif
    
    return 0;
}



// LOAD AND STORE

void Engine::store(std::string fileName)
{
#ifdef TODO_ENGINE    
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL;
    
	doc = xmlNewDoc(BAD_CAST "1.0");
	root_node = xmlNewNode(NULL, BAD_CAST "ENGINES");
	xmlSetProp(root_node, BAD_CAST "version", BAD_CAST ENGINE_VERSION);
    
	std::ostringstream oss;
	oss << m_editor->scenarioSize();
	xmlSetProp(root_node, BAD_CAST "scenarioSize", BAD_CAST oss.str().data());
    
	xmlDocSetRootElement(doc, root_node);
    
	m_editor->store(root_node);
	m_executionMachine->store(root_node);
    
	xmlSaveFormatFileEnc(fileName.data(), doc, "UTF-8", 1);
    
	xmlFreeDoc(doc);
#endif
}

void Engine::load(std::string fileName)
{
#ifdef TODO_ENGINE    
	xmlNodePtr racine = NULL;
	xmlNodePtr n = NULL;
	xmlDocPtr doc = xmlReadFile(fileName.data(), "UTF-8", 0);
    
	racine = xmlDocGetRootElement(doc);
    
	xmlChar* version = xmlGetProp(racine, BAD_CAST "version");
	xmlChar* xmlScenarioSize = xmlGetProp(racine, BAD_CAST "scenarioSize");
    
	(void) version;
    
	int scenarioSize = XMLConversion::xmlCharToInt(xmlScenarioSize);
    
	reset(scenarioSize);
    
	for (n = racine->children; n != NULL; n = n->next) {
		if (n->type == XML_ELEMENT_NODE) {
			if (xmlStrEqual(n->name, BAD_CAST "EDITOR")) {
				std::cout << "LOAD: EDITOR" << std::endl;
				m_editor->load(n);
			} else if (xmlStrEqual(n->name, BAD_CAST "ECOMACHINE")) {
				std::cout << "LOAD: ECO-MACHINE" << std::endl;
				m_executionMachine->load(n, m_networkController);
			}
		}
	}
    
	xmlFreeDoc(doc);
#endif
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

void InteractiveEventActiveAttributeCallback(TTPtr baton, const TTValue& value)
{
    TTValuePtr          b;
    EnginePtr           engine;
    InteractiveEventId  triggerId;
    TTBoolean           active;
	
	// unpack baton (engine, triggerId)
	b = (TTValuePtr)baton;
	engine = EnginePtr((TTPtr)(*b)[0]);
    triggerId = InteractiveEventId((*b)[0]);
	
	// Unpack data (active)
	active = value[0];
    
	if (engine->m_InteractiveEventActiveAttributeCallback != NULL)
		engine->m_InteractiveEventActiveAttributeCallback(triggerId, active);
}

void TimeProcessSchedulerRunningAttributeCallback(TTPtr baton, const TTValue& value)
{
    TTValuePtr          b;
    EnginePtr           engine;
    TimeProcessId       boxId;
    TTBoolean           running;
	
	// unpack baton (engine, boxId, controlPointId)
	b = (TTValuePtr)baton;
	engine = EnginePtr((TTPtr)(*b)[0]);
    boxId = TimeProcessId((*b)[0]);
	
	// Unpack data (running)
	running = value[0];
    
	if (engine->m_TimeProcessSchedulerRunningAttributeCallback != NULL)
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