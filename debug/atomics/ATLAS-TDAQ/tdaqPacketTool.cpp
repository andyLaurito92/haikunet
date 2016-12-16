#include "tdaqPacketTool.h"

int TdaqPacket::nextPacketId = 1;

std::shared_ptr<FragmentInfo> TdaqPacket::getFragmentInfo() {
	std::shared_ptr<FragmentInfo> info = NULL;

	for (auto protocol : this->ProtocolList ) {
		int protocolId = protocol->protoID;
		if (protocolId == PROTOCOL_ID_1LTRIGGER_RESPONSE ){// protocols that can contain Frangment Info
			info = std::static_pointer_cast<FragmentInfo>( this->payload);
		}
	}

	if (info == NULL || info.get() == NULL) {
		printLog(LOG_LEVEL_ERROR, "GetFragmentInfo: WARNING!!!! attempted to retrieve FragmentInfo from wrong packet. FRAGMENT INFO NOT FOUND! \n Packet Info: \n");
		this->printPacketInfo(LOG_LEVEL_ERROR);
		return NULL;
	}

	return info;
}

std::shared_ptr<FragmentRequestInfo> TdaqPacket::getFragmentRequestInfo() {
			std::shared_ptr<FragmentRequestInfo> info = NULL;

			for (auto protocol : this->ProtocolList) {
				int protocolId = protocol->protoID;
				if (protocolId == PROTOCOL_ID_1LTRIGGER_REQUEST ) {// protocols that can contain Frangment Info
					info =  std::static_pointer_cast<FragmentRequestInfo>(this->payload);
				}
			}

			if (info == NULL || info.get() == NULL) {
				printLog(LOG_LEVEL_ERROR, "GetFragmentRequestInfo: WARNING!!!! attempted to retrieve FragmentRequestInfo from wrong packet. FRAGMENT REQUEST INFO NOT FOUND! \n Packet Info: \n");
				this->printPacketInfo(LOG_LEVEL_ERROR);
				return NULL;
			}

			return info;
}

std::shared_ptr<EventInfo> TdaqPacket::getEventInfo() {
	std::shared_ptr<EventInfo>  info = NULL;
	if (strcmp (this->FlowID,FLOW_ID_EVENT_BUILD_REQUEST) == 0) {
		info =  std::static_pointer_cast<EventInfo>( this->payload);
	}

	if (info == NULL || info.get() == NULL) {
		printLog(LOG_LEVEL_ERROR, "getEventInfo: WARNING!!!! attempted to retrieve EventInfo from wrong packet. EventInfo INFO NOT FOUND! \n Packet Info: \n");
		this->printPacketInfo(LOG_LEVEL_ERROR);
		return NULL;
	}

	return info;
}

std::shared_ptr<protocol> TdaqPacket::getProtocol(int protocolId) {
	for (auto protocol : this->ProtocolList) {
		if (protocolId == protocol->protoID) {
			return protocol;
		}
	}

	printLog(LOG_LEVEL_ERROR, "getProtocol(%u, %u(%s): WARNING!!!! attempted to retrieve protocol not present in the packet. protocol INFO NOT FOUND! \n Packet Info: \n", protocolId, this->ID,this->FlowID);
	throw;
}

/*********** Factory methods ********************/
/*static*/ std::shared_ptr<TdaqPacket> TdaqPacket::createHltsvRequestPacket(double t, std::shared_ptr<HltsvRequest> hltsvRequest){
	std::shared_ptr<TdaqPacket> p = createPacket(t, FLOW_ID_HLTSV_REQUEST);

	p->payload = hltsvRequest;

	return p;
}

/*static*/ std::shared_ptr<TdaqPacket> TdaqPacket::createBuiltEventPacket(double t, std::shared_ptr<EventInfo> eventInfo) {
	std::shared_ptr<TdaqPacket> p = createPacket(t, FLOW_ID_BUILT_EVENT);

	p->payload = eventInfo;

	//TODO: set packet size

	return p;
}

/*static*/ std::shared_ptr<TdaqPacket> TdaqPacket::createFirstLevelTriggerRequestPacket(double t, std::shared_ptr<FragmentRequestInfo> requestInfo, int requestSize_bytes) {
	std::shared_ptr<TdaqPacket> p = createPacket(t, FLOW_ID_1LTRIGGER_REQUEST);

	// add the protocol for L2 request
	auto requestProtocol = std::make_shared<protocol> ();
	requestProtocol->layer = 7;
	requestProtocol->metadata[PAYLOADSIZE_BITS] = requestSize_bytes * 8;
	requestProtocol->metadata[CREATION_TIME] = t;
	requestProtocol->protoID = PROTOCOL_ID_1LTRIGGER_REQUEST;
	p->ProtocolList.push_back(requestProtocol);

	// Add TCP protocol
	auto tcpProtocol  = createTcpProtocol(t, -1, requestInfo->puId);
	p->ProtocolList.push_back(tcpProtocol);

	// Add IP protocol with destination machine
	auto ipProtocol = createIpProtocol(t, requestInfo->rosId, requestInfo->dcmId);
	p->ProtocolList.push_back(ipProtocol);

	// add requested info in the payload
	p->payload = requestInfo;

	return p;
}

/*static*/ std::shared_ptr<TdaqPacket> TdaqPacket::createL2RequestPacket(double t, std::shared_ptr<FragmentRequestInfo> request) {
	std::shared_ptr<TdaqPacket> p = createPacket(t, FLOW_ID_L2_REQUEST);

	// add requested info in the payload
	p->payload = request; // NOTE: to cast it back to vector<int> use: vector<int> robids = ((L2TRequester::RobIds*)p->payload)->robIds;

	return p;
}

/*static*/ std::shared_ptr<TdaqPacket> TdaqPacket::createFirstLevelTriggerResponsePacket(double t, int fragmentSize, std::shared_ptr<FragmentInfo> fragmentInfo, std::shared_ptr<TdaqPacket> requestPacket) {
	auto requestInfo = requestPacket->getFragmentRequestInfo();

	// create new packet
	std::shared_ptr<TdaqPacket> p = createPacket(t, FLOW_ID_1LTRIGGER_RESPONSE);

	// TODO: is it necessary to create a new object. Can we use the same requestedInfo?
	auto info = std::make_shared<FragmentInfo>(fragmentInfo->eventId, fragmentInfo->rosId, fragmentInfo->robId, fragmentSize);
	p->payload = info;

	// Add protocols:
	// ADDs new "1LTriggerResponse" protocol to the packet to track them along a full network path
	// TODO: calculate response size according to amount of requested fragments (now is always 1 fragment).
	auto responseProtocol = std::make_shared<protocol>();
	responseProtocol->protoID = PROTOCOL_ID_1LTRIGGER_RESPONSE;
	responseProtocol->layer = 7;
	responseProtocol->metadata[PAYLOADSIZE_BITS] = fragmentSize * 8; // payload must be in bits, fragment size is in Bytes
	responseProtocol->metadata[CREATION_TIME] = t;
	p->ProtocolList.push_back(responseProtocol);

	// TCP protocol
	auto tcpProtocol  = createTcpProtocol(t, requestInfo->puId, -1);
	p->ProtocolList.push_back(tcpProtocol);

	// Ip protocol with src and dst
	auto ipProtocol  = createIpProtocol(t, requestInfo->dcmId, requestInfo->rosId);
	p->ProtocolList.push_back(ipProtocol);

	// Add a copy of the 'request' protocol (in order to track RTT times. Check  PacketSink model)
	auto requestProtocol = std::make_shared<protocol>(*requestPacket->getProtocol(PROTOCOL_ID_1LTRIGGER_REQUEST));
	requestProtocol->metadata[PAYLOADSIZE_BITS] = 0; // this is protocol is added to response just for simulation purposes, is not there reality so should not extra size (bits)
	p->ProtocolList.push_back(requestProtocol);

	return p;
}

/*static*/ std::shared_ptr<TdaqPacket> TdaqPacket::createEventBuildRequestPacket(double t,int eventId, int puId, std::vector<FragmentInfo> fragments,  double eventBirthTime, double procTime) {
	std::shared_ptr<TdaqPacket> p = createPacket(t, FLOW_ID_EVENT_BUILD_REQUEST);

	// Create Event
	auto evInfo = std::make_shared<EventInfo>(eventId, puId, fragments, eventBirthTime, procTime);

//	// creates a copy of the fragments for the new packets to build the event.
//	// TODO: why? can't just do 'evInfo->fragmentsInfo = fragments' ( vector assignment is copy)? Is't it copied in the constructor anyway?
//	// TODO: verify the cost of copying so much info.
//	for (auto fragment : fragments) {
//		FragmentInfo fragmentCopy { fragment.eventId, fragment.rosId, fragment.robId };
//		evInfo->fragmentsInfo.push_back(fragmentCopy);
//	}

	p->payload = evInfo;

	// ADDs new "buildEventRequest" protocol to the packet to track them along a full network path
	auto buildEventReqProtocol = std::make_shared<protocol>();
	buildEventReqProtocol->protoID = PROTOCOL_ID_EVENT_BUILD_REQUEST;
	buildEventReqProtocol->metadata[PAYLOADSIZE_BITS] = 0; // This is not a network packet, but an internal message not sent through the network
	buildEventReqProtocol->metadata[CREATION_TIME] = t; // time when the request was built

	p->ProtocolList.push_back(buildEventReqProtocol);

	return p;
}

/*static*/ std::shared_ptr<TdaqPacket> TdaqPacket::createAckPacket(double t, int sequenceNumber, double packetToAckBirthTime, int destinationMachineId, int destinationApplicationId, int sourceMachineId, int sourceApplicationId) {
	std::shared_ptr<TdaqPacket> p = createPacket(t, FLOW_ID_ACK);

	// Add 'ACK' to TCP protocol
	auto tcpProtocol = createTcpProtocol(t,destinationApplicationId, sourceApplicationId);
	tcpProtocol->headerFields[ACK] =  sequenceNumber;
	tcpProtocol->headerFields[ORIGINAL_PACKET_BIRTH_TIME] =  packetToAckBirthTime;
	p->ProtocolList.push_back(tcpProtocol);

	// Add IP protocol with destination machine
	auto ipProtocol = createIpProtocol(t, destinationMachineId, sourceMachineId);
	p->ProtocolList.push_back(ipProtocol);

	return p;
}

/*static*/ std::shared_ptr<TdaqPacket> TdaqPacket::createPacket(double t, char* flowID) {
	auto p = std::make_shared<TdaqPacket>();
	p->ID = TdaqPacket::getNextPacketID();
	p->FlowID = flowID;
	p->birthTime = t;

	return p;
}

/*static*/ std::shared_ptr<protocol> TdaqPacket::createTcpProtocol(double t, int destinationApplicationId, int sourceApplicationId){
	auto tcpProtocol = std::make_shared<protocol>();
	tcpProtocol->protoID = PROTOCOL_ID_TCP;
	tcpProtocol->metadata[PAYLOADSIZE_BITS] = 0; // in bits
	tcpProtocol->metadata[CREATION_TIME] = t; // time when the packet was built
	tcpProtocol->layer = 4;
	tcpProtocol->metadata[OVERHEAD] = 44 * 8; // Ethernet (24 Bytes) +  TCP Header (without any options) (20 Bytes)
	tcpProtocol->headerFields[SRC_PORT] = sourceApplicationId;
	tcpProtocol->headerFields[DST_PORT] = destinationApplicationId;

	return tcpProtocol;
}

/*static*/ std::shared_ptr<protocol> TdaqPacket::createIpProtocol(double t, int destinationMachineId, int sourceMachineId){
	auto ipProtocol = std::make_shared<protocol>();
	ipProtocol->protoID = PROTOCOL_ID_IP;
	ipProtocol->metadata[PAYLOADSIZE_BITS] = 0; // in bits
	ipProtocol->metadata[CREATION_TIME] = t; // time when the packet was built
	ipProtocol->layer = 4;
	ipProtocol->metadata[OVERHEAD] = 20 * 8; //  IPv4 Header (without any options) (20 bytes)
	ipProtocol->headerFields[IP_DST] = destinationMachineId; // destination=ROS
	ipProtocol->headerFields[IP_SRC] = sourceMachineId; //origin puId

	return ipProtocol;
}
