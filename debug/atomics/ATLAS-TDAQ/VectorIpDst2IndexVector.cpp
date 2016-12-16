#include "VectorIpDst2IndexVector.h"

std::shared_ptr<TdaqPacket> VectorIpDst2IndexVector::getPacketFromIncomingEvent(Event x){
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	return std::static_pointer_cast<TdaqPacket>(arrivedEvent->valuePtr);
}

int VectorIpDst2IndexVector::getProtocolId(){
	return PROTOCOL_ID_IP;
}

int VectorIpDst2IndexVector::getProtocolHeaderId(){
	return IP_DST;
}
