#include "EBRequester.h"

void EBRequester::init(double t, ...) {
	BaseSimulator::init(t);
	va_list parameters;
	va_start(parameters,t);
	
	stochasticSimulation = readDefaultParameterValue<int>(SCILAB_VARNAME_STOCHASTIC_SIMULATION);;
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: stochasticSimulation: %s \n", this->getName(), stochasticSimulation?"YES":"NO");

	char* fvar;

//	fvar = va_arg(parameters,char*);
//	requestSize = readDefaultParameterValue<int>(fvar);
//	printLog(LOG_LEVEL_INIT, "[%f] %s_init: requestSize =  %i \n", t, this->getName(),requestSize);

	this->logger->initSignals({"sent", "fragmentsRequest", "eventId"});

	mystate = WAITING;
	totalSentRequests = 0;
	totalEventsRequested = 0;
	sigma = INF; // Waits for first signal to arrive

	va_end(parameters);
	return;
}

void EBRequester::dint(double t) {
	totalEventsRequested++;

	// wait until next request arrives
	mystate = WAITING;
	sigma = INF;

	printLog(10, "[%f] %s_int: sigma: %f ; pendingRequests:%i \n", t, this->getName(), sigma, pendingInfoToRequest->robIds.size());

	return;
}
void EBRequester::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition

	// Signal to request for new event arrived
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: A signal to start requesting arrived  \n", t,this->getName());

	// Safely validations
	if(arrivedPacket == NULL || arrivedPacket->getProtocol(PROTOCOL_ID_EVENT_BUILD_REQUEST) == NULL){
		printLog(LOG_LEVEL_ERROR,"[%f] %s_ext: WARNING!!! Received packet has wrong format. IGNORING...  \n",t, this->getName());
		arrivedPacket->printPacketInfo(10);
		sigma = sigma - e; // continue as before
		return;
	}

	if (mystate == REQUESTING) {
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: WARNING!!! A signal to start requesting arrived while the model was still sending previous packets. CANCELING previous job and restarting with the new one  \n", t,this->getName());
		totalEventsRequested++;
	}

	auto evInfo = std::static_pointer_cast<EventInfo>(arrivedPacket->payload);
	std::vector<FragmentInfo> fragmentsInfo = evInfo->fragmentsInfo;

	std::vector<int> robIdsToRequest;
	for (int robId : L1TConfiguration::getRobIds()) {
		 // request fragments not requested before
		 int rosId = L1TConfiguration::getRobIdToSource().find(robId)->second ;

	     if(fragmentsInfo.end() == std::find(fragmentsInfo.begin(), fragmentsInfo.end(), (FragmentInfo) {evInfo->eventId, rosId, robId})) {
	    	 robIdsToRequest.push_back(robId);
	    	 //printLog(99, "[DEBUG] %s_ext: adding rosId=%i for ROB=%i  \n", this->getName(), rosId, robId);
	     }
	}

	pendingInfoToRequest = std::make_shared<FragmentRequestInfo>(evInfo->eventId, -1 /*RosId*/, robIdsToRequest, evInfo->puId, -1/*DcmId*/);

	// start requesting immediately
	mystate = REQUESTING;
	sigma = 0;

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: will request a total of %i fragments  \n", t,this->getName(), pendingInfoToRequest->robIds.size());

}
Event EBRequester::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)
	std::shared_ptr<TdaqPacket> packetToSend = TdaqPacket::createL2RequestPacket(t, pendingInfoToRequest);
	if(packetToSend == NULL) return Event();

	totalSentRequests++;

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lamb: Sending request for %i fragments (eventId=%i) with the following packet : \n", t, this->getName(), pendingInfoToRequest->robIds.size(),pendingInfoToRequest->eventId );
	packetToSend->printPacketInfo(LOG_LEVEL_FULL_LOGGING);

	this->logger->logSignal(t,(double)totalSentRequests,"sent");

	return Event(packetToSend, 0);
}
