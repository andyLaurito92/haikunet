#include "TcpReceiverBuffer.h"
void TcpReceiverBuffer::init(double t,...) {
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

	this->logger->initSignals(std::vector<std::string> {});

}

void TcpReceiverBuffer::dint(double t) {
	sigma=getNextSigma();
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: next transition in %fs. \n",t, this->getName(), sigma);
}

void TcpReceiverBuffer::dext(Event x, double t) {
	// A packet arrived
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);

	// Safety Checks
	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (std::shared_ptr<TdaqPacket>). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "unable to cast to (std::shared_ptr<TdaqPacket>)";
		return;
	}

	// add packet to send buffer
	this->addPacketToBuffer(arrivedPacket);

	// get sigma
	sigma = this->getNextSigma();
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext:Received Packet (#%i) next transition in %fs. \n",t, this->getName(),arrivedPacket->ID, sigma);
}

Event TcpReceiverBuffer::lambda(double t) {
	auto packetToForward = this->getNextPacketToSend();

	// remove packet from send buffer
	this->removePacketFromBuffer(packetToForward);

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lambda: forwarding packet #%u  \n", t, this->getName(), packetToForward->ID);
	return Event(packetToForward, 0);
}

void TcpReceiverBuffer::addPacketToBuffer(std::shared_ptr<TdaqPacket> packet){
	int rosId = packet->getFragmentInfo()->rosId;

	auto rosPacketBuffer = receivedBufferPerRos.find(rosId);
	if(rosPacketBuffer == receivedBufferPerRos.end()) {
		auto connection = std::make_shared<BufferInfo>(packet);
		receivedBufferPerRos.insert({rosId, connection});
		printLog(LOG_LEVEL_FULL_LOGGING, "[] %s_addPacketToBuffer: added buffer for ROS %i \n", this->getName(), rosId);
	} else {
		rosPacketBuffer->second->addPacket(packet);
	}
}

void TcpReceiverBuffer::removePacketFromBuffer(std::shared_ptr<TdaqPacket> packet){
	int rosId = packet->getFragmentInfo()->rosId;

	auto rosPacketBuffer = receivedBufferPerRos.find(rosId);
	if(rosPacketBuffer == receivedBufferPerRos.end()) {
		printLog(LOG_LEVEL_FULL_LOGGING, "[] %s_removePacketFromBuffer: Attempted to remove packet #%i which is not in buffer \n", this->getName(), packet->ID);
		throw; // should never happen
		return;
	} else {
		rosPacketBuffer->second->removePacket(packet);
	}
}

double TcpReceiverBuffer::getNextSigma(){
	if(getNextPacketToSend() != NULL)
		return 0;
	return INF;
}

std::shared_ptr<TdaqPacket> TcpReceiverBuffer::getNextPacketToSend(){
	// TODO: check performace of doing this each single transition (int and lambda). Might be better to handle each TCP connection in a different model (now this model handles all connections of a DCM)
	for (auto it = receivedBufferPerRos.begin(); it != receivedBufferPerRos.end(); ++it) {
		if(it->second->shouldSendNextPacket()){ // find the first buffer ready to send
			return it->second->getNextPacket();
		}
	}

	return NULL;
}
