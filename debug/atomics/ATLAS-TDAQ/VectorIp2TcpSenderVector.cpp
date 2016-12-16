#include "VectorIp2TcpSenderVector.h"

void VectorIp2TcpSenderVector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	sigma=INF;
}

double VectorIp2TcpSenderVector::ta(double t) {
	//This function returns a double.
	return sigma;
}

void VectorIp2TcpSenderVector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void VectorIp2TcpSenderVector::dext(Event x, double t) {
	// TODO: join this class with ScalarIp2TcpSender and accept both scalar and vectorial events

	// Event arrived from the ROS
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(arrivedEvent->valuePtr);
	int rosId = arrivedEvent->index;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u from ROS=%i\n", t, this->getName(), arrivedPacket->ID, rosId);

	// Safety Checks
	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	} if(strcmp (arrivedPacket->FlowID, FLOW_ID_1LTRIGGER_RESPONSE) != 0 ){
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received. Wrong FlowId: only FLOW_ID_1LTRIGGER_RESPONSE or FLOW_ID_ACK are expected. Arrived: %s. \n",t, this->getName(), arrivedPacket->FlowID);
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	// find out DCM to which this packet is directed to (in the IP protocol)
	auto ipProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_IP);
	int dcmMachineId = ipProtocol->headerFields[IP_DST];
	//int rosId = ipProtocol->headerFields[SRC];

	// Change the index of the event to go to the corresponding TcpSender
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedEvent->valuePtr;
	vec->index = L1TConfiguration::getTcpSenderIndex(dcmMachineId, rosId);

	outputVectorialEvents.push(vec);

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u  \n", t, this->getName(), arrivedPacket->ID);

	sigma=0;
}

Event VectorIp2TcpSenderVector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void VectorIp2TcpSenderVector::exit() {

}
