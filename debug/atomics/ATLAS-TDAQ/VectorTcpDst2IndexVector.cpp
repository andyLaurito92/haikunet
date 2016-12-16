#include "VectorTcpDst2IndexVector.h"

std::shared_ptr<TdaqPacket> VectorTcpDst2IndexVector::getPacketFromIncomingEvent(Event x){
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	auto packet = std::static_pointer_cast<TdaqPacket>(arrivedEvent->valuePtr);

	return packet;
}

int VectorTcpDst2IndexVector::getProtocolId(){
	return PROTOCOL_ID_TCP;
}

int VectorTcpDst2IndexVector::getProtocolHeaderId(){
	return DST_PORT;
}
