#include "VectorIpSrc2IndexVector.h"

std::shared_ptr<TdaqPacket> VectorIpSrc2IndexVector::getPacketFromIncomingEvent(Event x){
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	return std::static_pointer_cast<TdaqPacket>(arrivedEvent->valuePtr);
}

int VectorIpSrc2IndexVector::getProtocolId(){
	return PROTOCOL_ID_IP;
}

int VectorIpSrc2IndexVector::getProtocolHeaderId(){
	return IP_SRC;
}
