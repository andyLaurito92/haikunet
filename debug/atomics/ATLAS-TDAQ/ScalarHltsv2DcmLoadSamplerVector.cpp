#include "ScalarHltsv2DcmLoadSamplerVector.h"

void ScalarHltsv2DcmLoadSamplerVector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	sigma=INF;
}

double ScalarHltsv2DcmLoadSamplerVector::ta(double t) {
	//This function returns a double.
	return sigma;
}

void ScalarHltsv2DcmLoadSamplerVector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void ScalarHltsv2DcmLoadSamplerVector::dext(Event x, double t) {
	// Request arrived
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u \n", t, this->getName(), arrivedPacket->ID);

	// Safety Checks
	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	int puId;

	if(x.port == 0){
		// find out PU Id from the packet
		auto hltsvRequest = std::static_pointer_cast<HltsvRequest>( arrivedPacket->payload);

		puId = hltsvRequest->puId;
		//printLog(LOG_LEVEL_IMPORTANT, "[%f] %s_ext: Received new assignment to PU %i (DCM %i). \n",t, this->getName(), puId, NetworkConfiguration::getDcmIndex(puId));
	}

	if(x.port == 1){
		auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
		auto eventInfo = std::static_pointer_cast<EventInfo> (arrivedPacket->payload);
		puId = eventInfo->puId;

		//printLog(LOG_LEVEL_IMPORTANT, "[%f] %s_ext: Received finished jobs from PU %i (DCM %i). \n",t, this->getName(), puId, NetworkConfiguration::getDcmIndex(puId));
	}

	int index  = NetworkConfiguration::getDcmIndex(puId);

	// Set the value and index
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedPacket;
	vec->index = index;

	outputVectorialEvents.push(std::make_pair(x.port, vec));

	sigma=0;
}

Event ScalarHltsv2DcmLoadSamplerVector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front().second;
	auto sentPort = outputVectorialEvents.front().first;
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,sentPort);
}

void ScalarHltsv2DcmLoadSamplerVector::exit() {

}
