#include "ScalarIp2TcpSender.h"

void ScalarIp2TcpSender::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	sigma=INF;
}

double ScalarIp2TcpSender::ta(double t) {
	//This function returns a double.
	return sigma;
}

void ScalarIp2TcpSender::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void ScalarIp2TcpSender::dext(Event x, double t) {
	// TODO: join this class with ScalarIp2TcpSender and accept both scalar and vectorial events

	// Request arrived
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u \n", t, this->getName(), arrivedPacket->ID);

	// Safety Checks
	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	} if(strcmp (arrivedPacket->FlowID,FLOW_ID_ACK) != 0){
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received. Wrong FlowId: only  FLOW_ID_ACK are expected. Arrived: %s. \n",t, this->getName(), arrivedPacket->FlowID);
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	// find out ROS Id from the packet (in the IP protocol)
	auto ipProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_IP);

	// Safety Check
	if(ipProtocol == NULL){
		printLog(1, "[%f] %s_ext: [WARNING] packet (#%i) does not contain the TCP protocol (ignore it) \n", t, this->getName(), arrivedPacket->ID);
		sigma = sigma - e; // continue as before
		throw;
		return;
	}

	int rosId = ipProtocol->headerFields[IP_DST];
	int dcmMachineId = ipProtocol->headerFields[IP_SRC];

	// Set the value and index
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = x.valuePtr;
	vec->index = L1TConfiguration::getTcpSenderIndex(dcmMachineId, rosId);

	outputVectorialEvents.push(vec);

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u from ROS=%i directed to DCM=%i . Sending it to TcpSender=%i \n", t, this->getName(), arrivedPacket->ID, rosId, dcmMachineId, vec->index);

	sigma=0;
}

Event ScalarIp2TcpSender::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void ScalarIp2TcpSender::exit() {

}
