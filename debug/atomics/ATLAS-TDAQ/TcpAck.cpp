#include "TcpAck.h"
void TcpAck::init(double t,...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	sigma = INF;
	myState = WAITING;
	totalACKsSent = 0;

	this->logger->initSignals({"totalACKsSent"});

}

void TcpAck::dint(double t) {
	sigma=INF; // a packet was ACKed, wait for next one
	if(packetsToAck.size() != 0){
		sigma = 0; // forwards next packet immediately
		myState = SENDING_ACK;
	}
	myState = WAITING;
}

void TcpAck::dext(Event x, double t) {
	// A packet arrived
	auto packetToAck = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
	packetsToAck.push(packetToAck);

	// Safety Checks
	if(packetToAck == NULL || packetToAck.get() == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "unable to cast to (TdaqPacket*)";
		return;
	}

	// ACK immediately
	sigma=0;
	myState = SENDING_ACK;
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Packet #%u arrived. Sending ACK in %fs. \n",t, this->getName(), packetToAck->ID, sigma);
}

Event TcpAck::lambda(double t) {
	auto packetToAck = packetsToAck.front();
	packetsToAck.pop();

	auto tcpProtocol = packetToAck->getProtocol(PROTOCOL_ID_TCP);
	auto ipProtocol = packetToAck->getProtocol(PROTOCOL_ID_IP);

	int destinationMachineId = ipProtocol->headerFields[IP_SRC]; // TODO: this is particular for ATLAS, use IP SRC
	int sourceMachineId = ipProtocol->headerFields[IP_DST];
	int sourcePortId = tcpProtocol->headerFields[DST_PORT];
	int destinatinPortId = tcpProtocol->headerFields[SRC_PORT];
	int sequenceNumber = tcpProtocol->headerFields[SEQ];

	//TODO: now it is sending ACK with the received SEQ number. TCP sends the ACK for the last ordered SEQ number (out-of-order SEQ number are sent in SACKs).
	auto packetToSend = TdaqPacket::createAckPacket(t, sequenceNumber, packetToAck->birthTime, destinationMachineId, destinatinPortId, sourceMachineId, sourcePortId);

	printLog(LOG_LEVEL_PRIORITY, "[%f]%s_lambda: Sending ACK for packet #%u (with ACK-packet #%u) \n", t, this->getName(), packetToAck->ID, packetToSend->ID);
	packetToSend->printPacketInfo(LOG_LEVEL_FULL_LOGGING);

	return Event(packetToSend, 0);
}
