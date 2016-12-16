#include "ProtocolHeader2IndexVector.h"

void ProtocolHeader2IndexVector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	sigma=INF;
}

double ProtocolHeader2IndexVector::ta(double t) {
	//This function returns a double.
	return sigma;
}

void ProtocolHeader2IndexVector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void ProtocolHeader2IndexVector::dext(Event x, double t) {
	// Event arrived, get the packet from vectorial or clasic event
	std::shared_ptr<TdaqPacket> arrivedPacket = this->getPacketFromIncomingEvent(x);

	// Safety Checks
	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u \n", t, this->getName(), arrivedPacket->ID);

	// find out DST address to which this packet is directed to (in the IP protocol)
	auto ipProtocol = arrivedPacket->getProtocol(this->getProtocolId());

	// Safety Check
	if(ipProtocol == NULL){
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING] packet (#%i) does not contain the IP protocol (ignore it) \n", t, this->getName(), arrivedPacket->ID);
		sigma = sigma - e; // continue as before
		throw;
		return;
	}

	int sourceId = ipProtocol->headerFields[this->getProtocolHeaderId()];

	// Change the index of the event to go to the corresponding destinationId
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedPacket;
	vec->index = sourceId;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u Prot%i[Head%i]=%i . Sending it with index=%i \n", t, this->getName(), arrivedPacket->ID, this->getProtocolId(), this->getProtocolHeaderId(), sourceId, vec->index);

	outputVectorialEvents.push(vec);

	sigma=0;
}

Event ProtocolHeader2IndexVector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void ProtocolHeader2IndexVector::exit() {

}
