#if !defined PACKETTOOL
#define PACKETTOOL

#include <stdlib.h>
#include <queue>
#include <string>
#include <list>
#include <map>
#include <memory>
#include "sinks/ParameterReader.h"

// FLOW IDs
#define FLOW_ID_HLTSV_REQUEST "FLOW_ID_HLTSV_REQUEST" // response from 1LTrigger (from ROS to PU)
#define FLOW_ID_1LTRIGGER_RESPONSE "FLOW_ID_1LTRIGGER_RESPONSE" // response from 1LTrigger (from ROS to PU)
#define FLOW_ID_1LTRIGGER_REQUEST "FLOW_ID_1LTRIGGER_REQUEST"  // request for 1LTrigger    (from PU to ROS)
#define FLOW_ID_L2_REQUEST "FLOW_ID_L2_REQUEST"  // request for 1LTrigger    (from PU to ROS)
#define FLOW_ID_EVENT_BUILD_REQUEST "FLOW_ID_EVENT_BUILD_REQUEST"  // a packet for event building (from PU to EB)
#define FLOW_ID_BUILT_EVENT "FLOW_ID_BUILT_EVENT"  // a packet sent once the event is built (from EB to storage)
#define FLOW_ID_ACK "FLOW_ID_ACK"  // a packet sent to ACK another packet

// Protocol IDs
#define UNDEFINED_PROTOCOL -1
#define PROTOCOL_ID_1LTRIGGER_REQUEST 2
#define PROTOCOL_ID_1LTRIGGER_RESPONSE 3
#define PROTOCOL_ID_EVENT_BUILD_REQUEST 4
#define PROTOCOL_ID_TCP 5
#define PROTOCOL_ID_IP 6

// HEADER IDs

// METADATA IDS
#define OVERHEAD 1
#define PAYLOADSIZE_BITS 2
#define CREATION_TIME 3
#define ACK 4

// IP Fields
#define IP_SRC 8
#define IP_DST 16

// TCP Fields (HEADER ids)
#define SRC_PORT 1
#define DST_PORT 2
#define SEQ 3
#define ACK 4
#define ORIGINAL_PACKET_BIRTH_TIME 5

// Protocols
#define TCP 1
#define UDP 2


struct protocol {
	int layer;
	int protoID=UNDEFINED_PROTOCOL;
	std::map<double, double> headerFields;
	std::map<double, double> trailerFields;
	std::map<double, double> metadata;
};
typedef std::map<double, double> aMap;
typedef std::map<double, double>::iterator aMapIter;
typedef std::list<std::shared_ptr<protocol>> aProtocolList;
//typedef std::list<protocol>::iterator aProtocolListIter;

//typedef std::list<void*> anExperimentalList;

class Packet {
private:
	int length_bits;
	int fluidPackets;

public:

	Packet() {
		ID = 0;
		length_bits = 0;
		fluidPackets = 0;

		printLog(10, "Packet Created (pointer=%i) \n", this);
	}

	~Packet() {
		printLog(10, "Packet Destroyed (pointer=%i) \n", this);
	}

	aProtocolList ProtocolList;
//	anExperimentalList experimentalList;

	//std::string payload;
	std::shared_ptr<void> payload; // can be set to a variable of any type (needs to be casted afterwards. USE FlowID to set a convention for its payload type.

	int ID;
	char* FlowID;
	int ComplexityFactor;

	double birthTime;
	double lastModifTime;
	int lastQueueSizeAfterLeaving; // After this packet leaves

	int auxiliar_SEQ;

	double currentChannelPropagationTime;
	double currentReceptionTimestamp;

	void calcLength() // in bits
	{
		this->length_bits = 0;
		for (auto protocol : ProtocolList) {
			// OVERHEAD
			auto mit = protocol->metadata.find(OVERHEAD);
			if (mit != protocol->metadata.end()){
				this->length_bits += (int) mit->second;
			}

			// PAYLOAD
			mit = protocol->metadata.find(PAYLOADSIZE_BITS);
			if (mit != protocol->metadata.end()){
				this->length_bits += (int) mit->second;
			}
		}
		//printLog(level, "Packet: Calculated Length %u \n", this->length);
	}


	void setLength(int len) {
		this->length_bits = len;
	}


	int getLength_bits() // in bits
	{
		this->calcLength();
		return this->length_bits;
	}

	int getLength_bytes() // in bytes
	{
		this->calcLength();
		return this->length_bits >> 3;
	}


	void setFluidPackets(int len) {
		this->fluidPackets = len;
	}


	int getFluidPackets() // in bits
	{
		return this->fluidPackets;
	}


	double setPropagationTime(int plength, double lat, double bw) // In ms.
			{
		currentChannelPropagationTime = (plength / bw) * 1000 + lat; // In ms. (length: bits, bw: bps, lat: ms)
		printLog(10, "Packet: Calculated Propagation Time %g \n", currentChannelPropagationTime);

		return currentChannelPropagationTime;
	}

	void printPacketInfo(int level) {
		printLog(level, "-------    Information for Packet #%u(%s): ----------------\n", this->ID, this->FlowID);
		printLog(level, "ID= %u \n", this->ID);
		printLog(level, "FlowID= %s\n", this->FlowID);
		printLog(level, "birthTime= %.16f \n", this->birthTime);
		printLog(level, "getLength()= %u\n", this->getLength_bits());
		printLog(level, "ComplexityFactor= %u \n", this->ComplexityFactor);

		printLog(level, "Protocols (%u): \n", (int) this->ProtocolList.size());
		for (auto protocol : ProtocolList) {
			printLog(level, "\t Protocol ID= %u \n", protocol->protoID);
			printLog(level, "\t Protocol layer= %u \n", protocol->layer);

			aMap myMetadataFields = protocol->metadata;
			printLog(level, "\t Protocol METADATA (%u): \n", myMetadataFields.size());
			for (aMapIter mapIt = myMetadataFields.begin(); mapIt != myMetadataFields.end(); mapIt++)
				printLog(level, "\t\t %g=%g \n", (*mapIt).first, (*mapIt).second);

			aMap myHeaderFields = protocol->headerFields;
			printLog(level, "\t Protocol Headers (%u): \n", myHeaderFields.size());
			for (aMapIter mapIt = myHeaderFields.begin(); mapIt != myHeaderFields.end(); mapIt++)
				printLog(level, "\t\t %g=%g \n", (*mapIt).first, (*mapIt).second);

			printLog(level, "\t-------------------  \n");
		}

		printLog(level, "-------    END packet Information  ----------------\n", this->ID);
	}
};

typedef std::queue<Packet*> aPacketQueue;
typedef std::deque<Packet*> aDoubleEndedPacketQueue;
#endif
