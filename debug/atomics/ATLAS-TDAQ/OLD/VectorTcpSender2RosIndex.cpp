#include "VectorTcpSender2RosIndex.h"
// TODO: this model is not used any more

void VectorTcpSender2RosIndex::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type


	sigma=INF;
}

double VectorTcpSender2RosIndex::ta(double t) {
	//This function returns a double.
	return sigma;
}

void VectorTcpSender2RosIndex::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void VectorTcpSender2RosIndex::dext(Event x, double t) {
	// Event arrived from the ROS
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(arrivedEvent->valuePtr);

	int tcpSenderIndex = arrivedEvent->index;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u from tcpSender=%i\n", t, this->getName(), arrivedPacket->ID, tcpSenderIndex);

	// find out ROS index
	auto ipProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_IP);
	int dcmMachineId = ipProtocol->headerFields[IP_DST]; // get PU to which the message is directed

	int rosId = L1TConfiguration::getRosIndex(dcmMachineId, tcpSenderIndex); // get ROS from where the message was originated

	// Change the index of the event to go to the corresponding ToR
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedEvent->valuePtr;
	vec->index = rosId;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received event from tcpSender=%i (ROS=%i) and directed to DCM=%i. Sending it to ROS=%i \n", t, this->getName(), tcpSenderIndex, rosId, dcmMachineId, vec->index);

	outputVectorialEvents.push(vec);

	sigma=0;
}

Event VectorTcpSender2RosIndex::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void VectorTcpSender2RosIndex::exit() {

}
