#include "L2TProcessing.h"

void L2TProcessing::init(double t, ...) {
	BaseSimulator::init(t);
//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

	char* fvar;

	fvar = va_arg(parameters,char*);
	cantRequestsPerEvent = (int) getScilabVar(fvar, true);
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: cantRequestsPerEvent =  %i \n", this->getName(), cantRequestsPerEvent);

	fvar = va_arg(parameters,char*);
	this->processingPeriod = (double) getScilabVar(fvar, true);
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: processingPeriod =  %f \n", this->getName(), this->processingPeriod);

	fvar = va_arg(parameters,char*);
	stochasticProcessingTimes = (double) getScilabVar(fvar, true);
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: l2StochasticProcessingTime: %s \n", this->getName(), stochasticProcessingTimes?"YES":"NO");

	fvar = va_arg(parameters,char*);
	this->useIncrementalProcessingTime = (bool) getScilabVar(fvar, true);
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: useIncrementalProcessingTime =  %s \n", this->getName(), this->useIncrementalProcessingTime?"YES":"NO");

//	this->logger = new aLoggerToScilab(SCILAB_LOG_LEVEL_NORMAL, this->myself, this->getName(), false);
//	this->logger->initSignals(3, "arrRequests", "arrResponses", "sntEvnts");

	this->logger->initSignals({"procTime", "step", "rawProcTime"});

	sentEvents = 0;
	currentStep = 1;
	puId = -1;
	eventId = -1;

	sigma = INF;  // wait for first request to arrive
	myState = WAITING_NEXT_JOB;
	if(cantRequestsPerEvent==0)
		myState = WAITING_RESPONSE; // if cantRequestsPerEvent = 0 it sends only 1 build event request

}

void L2TProcessing::dint(double t) {
	switch (myState) {
		case PROCESSING:
			myState = WAITING_NEXT_JOB;
			sigma = INF; // wait for next request to arrive
			printLog(10, "[%f] %s_int: Waiting for next job (sigma = %f). \n", t, this->getName(), sigma);
			break;
		case WAITING_RESPONSE:
			sigma = INF;
		    printLog(10, "[%f]%s_int: WAITING for next packet to arrive (%f). \n", t, this->getName(), sigma);
			break;
		default:
			printLog(10, "[%f]%s_int: Error: Do not know what to do... \n", t, this->getName());
			break;
		}

	return;
}

void L2TProcessing::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: packet arrived. Port:%i \n",t, this->getName(), x.port);
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket> (x.valuePtr);

	if (x.port == 0) { // packet arrived with request to the 1st level trigger, to know which packets to wait for.
		// get ROB ids from packet payload (safe cast due to FLOW_ID)
		auto requestInfo = std::static_pointer_cast<FragmentRequestInfo>(arrivedPacket->payload);

		printLog(LOG_LEVEL_PRIORITY,"[%f] %s_ext: Received request for a total of %i fragments  \n",t, this->getName(),  requestInfo->robIds.size());

		if (arrivedPacket == NULL || strcmp (arrivedPacket->FlowID,FLOW_ID_L2_REQUEST) != 0) { // wrong packet format, ignore it
			printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: Error wrong packet received: unable to find robIds in arrived packets. \n",t, this->getName());
			this->sigma = this->sigma - this->e; // continue as before
			return;
		}

		if(this->waitingInfo.robIds.size() == 0 && requestInfo->eventId != this->eventId){ // its the first request, we start 'building' the event
			this->eventBirthTime = t;
			this->puId = requestInfo->puId;
			this->eventId = requestInfo->eventId;
			this->currentStep = 1;
			printLog(LOG_LEVEL_PRIORITY, "[%.16f] %s_ext: FIRST REQUEST sent (L2 starts requesting first fragments), eventbirthTime=%.16f EventId=%i\n",t, this->getName(), t, this->eventId);
		} else if (this->waitingInfo.robIds.size() == 0 && requestInfo->eventId == this->eventId) { // its the first request from the same event, we start a new step
			this->currentStep++;
			printLog(LOG_LEVEL_PRIORITY, "[%.16f] %s_ext: New step started: %i th step",t, this->getName(), this->currentStep);
		}
		this->myState = WAITING_RESPONSE;

		// make a copy so we can change it
		this->waitingInfo.eventId = requestInfo->eventId;
		this->waitingInfo.robIds = requestInfo->robIds;

		//this->logger->logSignal(t, (double) this->waitingInfo.robIds.size(), "arrRequests");
	}

	if (x.port == 1) { // packet arrived from the 1st level trigger with the requested information
		auto info = arrivedPacket->getFragmentInfo();

		if (info == NULL ) { // wrong packet format, ignore it
			printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: Error wrong packet received: unable to find #ROB or #ROS in arrived packets. \n",t, this->getName());
			this->sigma = this->sigma - this->e; // continue as before
			return;
		}

		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: RESPONSE arrived (#%u). ROS=%i  ROB=%i  \n",t, this->getName(), arrivedPacket->ID, info->rosId, info->robId);

		// check if model is waiting for this eventId
		if(info->eventId != this->waitingInfo.eventId){
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: ERROR!!! a packet with the wrong event Id arrived (expecting eventId=%i, arrived=%i). Packet Info: \n",t, this->getName(),waitingInfo.eventId, info->eventId);
			arrivedPacket->printPacketInfo(LOG_LEVEL_FULL_LOGGING);
			this->sigma = this->sigma - this->e; // continue as before
			return;
		}

		// check if model is waiting for this ROB
		auto itResponse = std::find(this->waitingInfo.robIds.begin(), this->waitingInfo.robIds.end(), info->robId);

		if(this->waitingInfo.robIds.end() == itResponse)
		{
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: WARNING!!! a packet that the model was not expected arrived. Packet Info: \n",t, this->getName());
			arrivedPacket->printPacketInfo(LOG_LEVEL_FULL_LOGGING);
			this->sigma = this->sigma - this->e; // continue as before
			return;
		}

		// remove packet from waiting list and put it in received
		this->waitingInfo.robIds.erase(itResponse);
		this->receivedInfo.push_back(*info);

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: New packet for PROCESSING arrived. waitingResponses=%i receivedInfo=%i  \n",t, this->getName(), this->waitingInfo.robIds.size(), this->receivedInfo.size());

		//this->logger->logSignal(t, (double) this->receivedInfo.size(), "arrResponses");
	}

	// set sigma and state
	if(this->waitingInfo.robIds.size() == 0) // all requests arrived
	{
		// we start processing the event
		this->myState = PROCESSING;
		this->sigma = this->getNextProccessingTime(t);

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: procTime=%f currentStep=%i \n",t, this->getName(), this->sigma, this->currentStep);
		this->logger->logSignal(t, this->sigma, "procTime");
		this->logger->logSignal(t, this->currentStep, "step");
	}else
	{
		printLog(LOG_LEVEL_FULL_LOGGING, "[%.16f] %s_ext: Keep waiting for more responses to arrive \n",t, this->getName());
		this->myState = WAITING_RESPONSE;
		this->sigma = INF;
	}
}

Event L2TProcessing::lambda(double t) {
	std::shared_ptr<TdaqPacket> packetToSend;

	switch (myState) {
	case PROCESSING: // processing finished
		printLog(LOG_LEVEL_PRIORITY, "[%f]%s_lamb: finished processing. Forwarding event to EB . \n", t, this->getName());

		// create event packet and send it
		packetToSend =  createEventPacket(t, this -> sigma);
		receivedInfo.clear();
		waitingInfo.robIds.clear();

		sentEvents++;
//		this->logger->logSignal(t, (double) sentEvents, "sntEvnts");

		return Event(packetToSend, 0);
		break;

	default:
		printLog(LOG_LEVEL_ERROR, "[%f] %s_lamb: Error: Do not know what to do... (myState = %f) \n", t, this->getName(), myState);
		break;
	}
}

std::shared_ptr<TdaqPacket> L2TProcessing::createEventPacket(double t, double procTime){
	auto p = TdaqPacket::createEventBuildRequestPacket(t, this->eventId, this->puId, receivedInfo,  eventBirthTime, procTime);

	printLog(10, "[%.16f]%s: Created EVENT packet to be sent to L2TIterator (eventBirthTime=%.16f, eventId=%i): \n", t, this->getName(), eventBirthTime, this->eventId);
	p->printPacketInfo(10);

	return p;
}

double L2TProcessing::getNextProccessingTime(double t){
	double time = this->processingPeriod;
	if(this->stochasticProcessingTimes){
        //time = STDEVS::StDevsDefaultInstance->poisson(this->processingPeriod); // WARNING: poisson returns ints so it is not suitable here. It models discrete values.
		time = STDEVS::getConfigurationInstance()->exponential(this->processingPeriod);
		//time = STDEVS::StDevsDefaultInstance->uniform() * this->processingPeriod * 2;
	}

	this->logger->logSignal(t, time, "rawProcTime");

	if(this->useIncrementalProcessingTime){
		time = time * this->currentStep;
	}

	return time;
}
