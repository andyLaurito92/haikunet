#include "VectorBondedLinkVector.h"

void VectorBondedLinkVector::init(double t,...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char* fvar = va_arg(parameters,char*);
	outputLinks = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: outputLinks: %i \n", t, this->getName(), outputLinks);

	this->logger->initSignals(std::vector<std::string> { "port", "index"});

	sigma=INF;
}

void VectorBondedLinkVector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void VectorBondedLinkVector::dext(Event x, double t) {
	//	 Event arrived from ROS
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(arrivedEvent->valuePtr);

	// Safety Checks
	if(arrivedEvent == NULL || arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (VectorialEvent*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received packet #%i \n", t, this->getName(), arrivedPacket->ID);

	// find out DCM to which this packet is directed to (in the IP protocol)
	auto ipProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_IP);
	auto tcpProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_TCP);

	int destinationId = ipProtocol->headerFields[IP_DST];
	int sourceId = ipProtocol->headerFields[IP_SRC];

	// Get outgoing link using dst and src addresses
	// TODO: improve model using real values for MAC and IP addresses (using just a few hardcoded ones works worst that using constant 1 values for MAC addesses)
	unsigned long destMacAddress = 1; //this->dcmMacAddress[destinationId % this->dcmMacAddress.size() ];
	unsigned long srcMacAddress = 1;  //this->rosMacAddress[sourceId % this->rosMacAddress.size() ];
	int packetId = 6;
	unsigned int hash = calculateHash(destMacAddress,srcMacAddress,destinationId,sourceId, packetId);
	int port = hash % this->outputLinks; // And then hash is reduced modulo slave count.

	// Send it to appropriate ROS link
	int index = (this->outputLinks * sourceId) + port;

	this->logger->logSignal(t,port,"port");
	this->logger->logSignal(t,index,"index");

	// Change the index of the event to go through corresponding port
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedEvent->valuePtr;
	vec->index = index;

	outputVectorialEvents.push(vec);

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Message (SRC=%i (%lu), DST=%i (%lu)) calculated port=%i and sending using index=%i\n", t, this->getName(), sourceId, srcMacAddress, destinationId, destMacAddress, port, vec->index);

	sigma=0;
}

Event VectorBondedLinkVector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

/**
 * Calculates the port based on the following hash algorithm
 * Documentation: https://www.kernel.org/doc/Documentation/networking/bonding.txt
 * hash = source MAC XOR destination MAC XOR packet type ID
		hash = hash XOR source IP XOR destination IP
		hash = hash XOR (hash RSHIFT 16)
		hash = hash XOR (hash RSHIFT 8)
		And then hash is reduced modulo slave count.
 Implementation code can be found here: https://github.com/torvalds/linux/blob/master/drivers/net/bonding/bond_main.c#L3050 (see method bond_xmit_hash)
 */
unsigned int VectorBondedLinkVector::calculateHash(long destMacAddress, long srcMacAddres, long destIp, long srcIp, long packetTypeId){
	unsigned int hash = destMacAddress ^ srcMacAddres ^ packetTypeId;
	hash ^= destIp ^ srcIp;
	hash ^= (hash >> 16);
	hash ^= (hash >> 8);

	return hash;
}
