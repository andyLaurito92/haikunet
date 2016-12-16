#include "ScalarIpSrc2IndexVector.h"

std::shared_ptr<TdaqPacket> ScalarIpSrc2IndexVector::getPacketFromIncomingEvent(Event x){
	return std::static_pointer_cast<TdaqPacket>(x.valuePtr);
}

int ScalarIpSrc2IndexVector::getProtocolId(){
	return PROTOCOL_ID_IP;
}

int ScalarIpSrc2IndexVector::getProtocolHeaderId(){
	return IP_SRC;
}
