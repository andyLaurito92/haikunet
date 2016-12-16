#if !defined NETWORK_PACKET
#define NETWORK_PACKET

#include <stdlib.h>
#include <string>
#include <list>
#include <map>

#include "Protocol/IProtocol.h"
#include "sinks/ParameterReader.h"

class NetworkPacket {
private:
	// to create unique packet IDs
	static int nextPacketId;
	static int getNextPacketID() { return nextPacketId++; }

	int id;
	double birthTime;
	int length_bits; // internally calculated based on the protocols metadata OVERHEAD and PAYLOADSIZE_BITS
	std::map<int, std::shared_ptr<IProtocol>> protocols;

//	void calcLength(); // not needed

public:
//	std::list<std::shared_ptr<IProtocol>> protocols;

	std::shared_ptr<void> payload; // can be set to a variable of any type (needs to be casted afterwards.

	NetworkPacket(double t) {
		this->id = NetworkPacket::getNextPacketID();
		this->birthTime = t;
		this->length_bits = 0;
	}

//	NetworkPacket(double t, const std::list<std::shared_ptr<IProtocol>>& protocols) : NetworkPacket(t) {
//		this->setProtocols(protocols);
//	}

	~NetworkPacket() {
		//printLog(10, "Packet Destroyed (pointer=%i) \n", this);
	}

	int getId(){ return this->id; }
	double getBirthTime(){ return this->birthTime;	}

	std::shared_ptr<IProtocol> getProtocol(int protocolId) {
		auto it = this->protocols.find(protocolId);
		if(it != this->protocols.end()){
			return it->second;
		} else {
			printLog(LOG_LEVEL_ERROR, ("NetworkPacket::getProtocol: failed to find protocolID=" + std::to_string(protocolId)).data() );
			return std::shared_ptr<IProtocol>();
		}
	}

	void removeProtocol(int protocolId) {
		auto protoIt = this->protocols.find(protocolId);
		if(protoIt != this->protocols.end()){
			this->length_bits -= protoIt->second->getSize_bits(); // substract old size
			this->protocols.erase(protocolId);
		}
	}

	void addProtocol(const std::shared_ptr<IProtocol> protocol) {
		this->length_bits += protocol->getSize_bits(); // add new size
		auto protoIt = this->protocols.find(protocol->getId());
		if(protoIt == this->protocols.end()){
			this->protocols[protocol->getId()] = protocol;
		} else {
			this->length_bits -= protoIt->second->getSize_bits(); // substract old size
			protoIt->second = protocol;
		}
	}

	int getLength_bits() { return this->length_bits; } // in bits
	int getLength_bytes() {
		int bytes = this->length_bits >> 3;

		return (this->length_bits % 8 == 0)? bytes: bytes+1 ;
	} // in bytes

	void printInfo(int level);
};


#endif
