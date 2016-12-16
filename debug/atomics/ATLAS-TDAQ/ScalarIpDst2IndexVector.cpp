#include "ScalarIpDst2IndexVector.h"

std::shared_ptr<TdaqPacket> ScalarIpDst2IndexVector::getPacketFromIncomingEvent(Event x){
	return std::static_pointer_cast<TdaqPacket>(x.valuePtr);
}

int ScalarIpDst2IndexVector::getProtocolId(){
	return PROTOCOL_ID_IP;
}

int ScalarIpDst2IndexVector::getProtocolHeaderId(){
	return IP_DST;
}
