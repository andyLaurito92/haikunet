#include "EventBuilder.h"

void EventBuilder::init(double t, ...) {
	BaseSimulator::init(t);
//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: Initializing \n", this->getName());
	

	char* fvar;

	fvar = va_arg(parameters,char*);
	cantRequestsPerEvent = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: cantRequestsPerEvent =  %i \n",this->getName(), cantRequestsPerEvent);

	this->logger->initSignals({"arrivedEvents", "arrivedRequests", "arrivedResponses", "sntEvts"});

	sigma = 0; // starts sending an empty packet to request for next job
	myState = WAITING;
	counter = 0;
}

void EventBuilder::dint(double t) {
	if (myState == BUILDING_EVENT) {
		sigma = 0; // program to request for more info immediately
		myState = WAITING;
	} else {
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: WAITING for more packets to arrive \n", t, this->getName());
		sigma = INF; // waits for a packet to arrives
	}
}

void EventBuilder::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: packet arrived. Port:%i \n", t, this->getName(), x.port);

	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);

	if (x.port == 0) { // packet arrived with event information gathered by the L2
		// Safely validations
		if(arrivedPacket == NULL || arrivedPacket->getProtocol(PROTOCOL_ID_EVENT_BUILD_REQUEST) == NULL){
			printLog(LOG_LEVEL_ERROR,"[%f] %s_ext: WARNING!!! Received packet has wrong format. IGNORING...  \n",t, this->getName());
			arrivedPacket->printPacketInfo(LOG_LEVEL_ERROR);
			sigma = sigma - e; // continue as before
			return;
		}

		receivedInfo = std::static_pointer_cast<EventInfo>(arrivedPacket->payload);
		eventBirthTime = t;

		sigma = sigma - e; // continue as before

		if (receivedInfo == NULL)
			printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: WARNING, no eventInfo in received packet!! \n");

		this->logger->logSignal(t, (double) counter, "arrivedEvents");

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: EventInfo ARRIVED from L2. EventInfo BirthTime: %f , fragments:%u \n", t, this->getName(), receivedInfo->birthTime, receivedInfo->fragmentsInfo.size());
	}

	if (x.port == 1) { // packet arrived with request to the DCM. This needed to know which packets to wait for.
		// Safely validations
		if(arrivedPacket == NULL || strcmp (arrivedPacket->FlowID,FLOW_ID_L2_REQUEST) != 0){
			printLog(LOG_LEVEL_ERROR,"[%f] %s_ext: WARNING!!! Received packet has wrong format. IGNORING...  \n",t, this->getName());
			arrivedPacket->printPacketInfo(LOG_LEVEL_FULL_LOGGING);
			sigma = sigma - e; // continue as before
			return;
		}

		// get responses to wait for (ROB ids) from packet payload (safe cast due to FLOW_ID)
		auto frReqInfo = std::static_pointer_cast<FragmentRequestInfo>(arrivedPacket->payload);

		// add messages to waiting list
		// make a copy so we can change it
		waitingInfo.eventId = frReqInfo->eventId;
		waitingInfo.robIds = frReqInfo->robIds;

		sigma = sigma - e; // continue as before

		this->logger->logSignal(t, (double) waitingInfo.robIds.size(), "arrivedRequests");

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: REQUEST arrived with %i fragments. \n", t, this->getName(), waitingInfo.robIds.size());
	}

	if (x.port == 2) { // packet arrived from the 1st level trigger with the requested information
		auto info = arrivedPacket->getFragmentInfo();
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: RESPONSE arrived. (pID #%i )ROS=%i  ROB=%i  \n", t, this->getName(), arrivedPacket->ID, info->rosId, info->robId);

		// check if model is waiting for this eventId
		if(info->eventId != waitingInfo.eventId){
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: ERROR!!! a packet with the wrong event Id arrived (expecting eventId=%i, arrived=%i). Packet Info: \n",t, this->getName(),waitingInfo.eventId, info->eventId);
			arrivedPacket->printPacketInfo(LOG_LEVEL_FULL_LOGGING);
			sigma = sigma - e; // continue as before
			return;
		}

		// check if model is waiting for this info
		auto position = std::find(waitingInfo.robIds.begin(), waitingInfo.robIds.end(), (*info).robId);
		if (waitingInfo.robIds.end() == position) {
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: WARNING!!! a packet that the model was not expected arrived. Packet Info: \n", t, this->getName());
			arrivedPacket->printPacketInfo(LOG_LEVEL_FULL_LOGGING);;
			sigma = sigma - e; // continue as before
			return;
		}

		// remove packet from waiting list and put it in received
		waitingInfo.robIds.erase(position);
		receivedInfo->fragmentsInfo.push_back(*info.get());
		myState = BUILDING_EVENT;

		sigma = INF;
		if (receivedInfo->fragmentsInfo.size() >= cantRequestsPerEvent){
			sigma = 0; // When all information is received, program output
		}

		this->logger->logSignal(t, (double) receivedInfo->fragmentsInfo.size(), "arrivedResponses");
		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: RESPONSE arrived (pID #%i ) from ROS=%i  ROB=%i. waitingResponses=%i receivedInfo=%i  \n", t, this->getName(), arrivedPacket->ID, info->rosId, info->robId, waitingInfo.robIds.size(), receivedInfo->fragmentsInfo.size());
	}
}

Event EventBuilder::lambda(double t) {
	// create event packet and send it
	if (myState == BUILDING_EVENT) {
		auto packetToSend = TdaqPacket::createBuiltEventPacket(eventBirthTime,  receivedInfo);

		counter++;
		receivedInfo = NULL;
		waitingInfo.robIds.clear();

		this->logger->logSignal(t, (double) counter, "sntEvts");
		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lambda: FORWARDING built event  \n", t, this->getName());
		return Event(packetToSend, 0);
	} else {
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lambda: requesting next job  \n", t, this->getName());

		return Event(&counter, 1); // Request more info
	}
}

