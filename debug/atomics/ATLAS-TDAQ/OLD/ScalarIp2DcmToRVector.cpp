#include "ScalarIp2DcmToRVector.h"

void ScalarIp2DcmToRVector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char* fvar = va_arg(parameters,char*);
	puPerDcm = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: puPerDcm: %i \n", t, this->getName(), puPerDcm);

	fvar = va_arg(parameters,char*);
	dcmPerRack = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: dcmPerRack: %i \n", t, this->getName(), dcmPerRack);

	sigma=INF;
}

double ScalarIp2DcmToRVector::ta(double t) {
	//This function returns a double.
	return sigma;
}

void ScalarIp2DcmToRVector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void ScalarIp2DcmToRVector::dext(Event x, double t) {
	// Request arrived
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u \n", t, this->getName(), arrivedPacket->ID);

	// Safety Checks
	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	} if(strcmp(arrivedPacket->FlowID, FLOW_ID_1LTRIGGER_RESPONSE) != 0 ){
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received. Wrong FlowId: only FLOW_ID_1LTRIGGER_RESPONSE are expected. Arrived: %s. \n",t, this->getName(), arrivedPacket->FlowID);
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	// find out DCM to which this packet is directed to (in the IP protocol)
	protocol* ipProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_IP);
	int puId = ipProtocol->headerFields[IP_DST];

	int dcmId = puId / this->puPerDcm;
	int dcmToRIndex = dcmId / this->dcmPerRack;

	// Change the index of the event to go to the corresponing DCM
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = x.valuePtr;
	vec->index = dcmToRIndex;

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Received Packet ID #%u, directed to PU=%i and DCM=%i. Sending to ToR=%i \n", t, this->getName(), arrivedPacket->ID, puId, dcmId, vec->index);

	outputVectorialEvents.push(vec);

	sigma=0;
}

Event ScalarIp2DcmToRVector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void ScalarIp2DcmToRVector::exit() {

}
