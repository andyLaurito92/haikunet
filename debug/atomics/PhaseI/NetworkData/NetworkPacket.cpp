#include "NetworkPacket.h"

int NetworkPacket::nextPacketId = 1;

//void NetworkPacket::calcLength() {
//	this->length_bits = 0;
//	for (auto protocolIt : this->protocols) {
//		this->length_bits += protocolIt.second->getSize_bits();
//	}
//}

void NetworkPacket::printInfo(int level) {
	printLog(level, "-------    Information for Packet #%u: ----------------\n", this->id);
	printLog(level, "ID= %u \n", this->id);
	printLog(level, "birthTime= %.16f \n", this->birthTime);
	printLog(level, "getLength()= %u bits (%u Bytes)\n", this->getLength_bits(), this->getLength_bytes());

	printLog(level, "Protocols (%u): \n", (int) this->protocols.size());
	for (auto protocolIt : this->protocols) {
		protocolIt.second->printInfo(level);

		printLog(level, "\t-------------------  \n");
	}

	printLog(level, "-------    END packet Information  ----------------\n", this->id);
}

