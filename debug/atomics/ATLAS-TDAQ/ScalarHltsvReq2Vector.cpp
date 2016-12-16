#include "ScalarHltsvReq2Vector.h"

void ScalarHltsvReq2Vector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	sigma=INF;
}

double ScalarHltsvReq2Vector::ta(double t) {
	//This function returns a double.
	return sigma;
}

void ScalarHltsvReq2Vector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void ScalarHltsvReq2Vector::dext(Event x, double t) {
	// Request arrived
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u \n", t, this->getName(), arrivedPacket->ID);

	// Safety Checks
	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	} if(strcmp (arrivedPacket->FlowID,FLOW_ID_HLTSV_REQUEST) != 0){
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received. Wrong FlowId: only FLOW_ID_HLTSV_REQUEST are expected. Arrived: %s. \n",t, this->getName(), arrivedPacket->FlowID);
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	// find out PU Id from the packet
	auto hltsvRequest = std::static_pointer_cast<HltsvRequest>( arrivedPacket->payload);

	// Safety Check
	if(hltsvRequest == NULL){
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING] packet (#%i) does not contain the hltsvRequest (ignore it) \n", t, this->getName(), arrivedPacket->ID);
		sigma = sigma - e; // continue as before
		throw "Bad content on packet";
		return;
	}

	// Set the value and index
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedPacket;
	vec->index = hltsvRequest->puId;

	outputVectorialEvents.push(vec);

	sigma=0;
}

Event ScalarHltsvReq2Vector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void ScalarHltsvReq2Vector::exit() {

}
