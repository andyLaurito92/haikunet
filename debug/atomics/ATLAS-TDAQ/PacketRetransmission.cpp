#include "PacketRetransmission.h"

void PacketRetransmission::init(double t, ...) {
//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//  %Name% is the parameter name
//	%Type% is the parameter type

	char *fvar;

	fvar = va_arg(parameters,char*);
	retransmissionTimeOut = readDefaultParameterValue<double>(fvar);
	if (retransmissionTimeOut <= 0)
		retransmissionTimeOut = INF;
	printLog(LOG_LEVEL_INIT, "[%f] %s: retransmissionTimeOut: %f \n", t, this->getName(), retransmissionTimeOut);

	sigma = INF;
	retransmissions = 0;

	this->logger = createDefaultLogger(SCILAB_LOG_LEVEL_NORMAL, this->getName());
	this->logger->initSignals(1, "retransmissions");

	return;
}

double PacketRetransmission::ta(double t) {
	return sigma;
}

void PacketRetransmission::dint(double t) {
	// A packet was retransmitted, update its RTO and recalculate sigma
	TdaqPacket* retransmittedPacket = this->getNextPacketToRetransmit();

	// remove packet
	std::vector<UnACKPacket>::iterator unAckPacketIt = std::find_if(unACKPackets.begin(), unACKPackets.end(), [&] (const UnACKPacket& p) {
			    return (p.packet->ID == retransmittedPacket->ID);
			});
	unACKPackets.erase(unAckPacketIt);

	// add it with new time at the end
	this->unACKPackets.push_back( {retransmittedPacket, t + this->retransmissionTimeOut, retransmittedPacket->getFragmentRequestInfo()->robIds.size()} );

	// recalculate sigma
	sigma = this->getNextRetransmissionSigma(t);
	printLog(10, "[%f] %s_int: programmed in %fs \n", t, this->getName(), sigma);
}

void PacketRetransmission::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition

	TdaqPacket* arrivedPacket = (TdaqPacket*) x.value;

	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(10, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		return;
	}

	if (x.port == 0) { // Packet being sent
		// store packet and its timeout
		this->unACKPackets.push_back( {arrivedPacket, t + this->retransmissionTimeOut, arrivedPacket->getFragmentRequestInfo()->robIds.size()} );
		// might need to sort if implementation changes:
		// sort(unACKPackets.begin(), unACKPackets.end(), [](unACKPacket p1, unACKPacket p2) {return p1.retransmissionTime < p2.retransmissionTime;});

		printLog(10, "[%f] %s_ext: added new packet (#%i), its retransmission time is: %f \n", t, this->getName(), arrivedPacket->ID, t + this->retransmissionTimeOut);
	}

	if (x.port == 1) { // Response Packet (ACK)
		protocol* tcpProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_TCP);

		// saftyCheck
		if(tcpProtocol == NULL){
			printLog(10, "[%f] %s_ext: [WARNING] packet (#%i) does not contain the TCP protocol (ignore it) \n", t, this->getName(), arrivedPacket->ID);
			sigma = this->getNextRetransmissionSigma(t);
			return;
		}

		int ackId = tcpProtocol->headerFields[ACK];

		// Check if we where waiting for this packet
		std::vector<UnACKPacket>::iterator unAckPacketIt = std::find_if(unACKPackets.begin(), unACKPackets.end(), [&] (const UnACKPacket& p) {
		    return (p.packet->ID == ackId);
		});

		if(unAckPacketIt == unACKPackets.end()){
			printLog(10, "[%f] %s_ext: packet (#%i) was not waiting for ACK (ignore it) \n", t, this->getName(), ackId);
			arrivedPacket->printPacketInfo(10);
		}
		else { // remove it from list
			if(unAckPacketIt->pendingFragments==1)
			{
				unACKPackets.erase(unAckPacketIt);
				printLog(10, "[%f] %s_ext: packet (#%i) removed from unACK packet list \n", t, this->getName(), ackId);
			}
			else
			{
				unAckPacketIt->pendingFragments--; // TODO: this is a weak check as we can receive several responses for the same ROB. A complete remodel with ACKs from the DCM side is pending
				printLog(10, "[%f] %s_ext: receibed fragment requested by packet (#%i). Still %i fragments pending \n", t, this->getName(), ackId, unAckPacketIt->pendingFragments);
			}

		}
	}

	sigma = this->getNextRetransmissionSigma(t);
	printLog(10, "[%f] %s_ext: next retransmission programmed in %fs \n", t, this->getName(), sigma);
}
Event PacketRetransmission::lambda(double t) {
	// need to retransmit
    Packet* packetToRetransmit = this->getNextPacketToRetransmit();
    retransmissions++;
    this->logger->logSignal(t, retransmissions, "retransmissions");

    printLog(10, "[%f] %s_lambda: retransmitting packet (#%i) \n", t, this->getName(), packetToRetransmit->ID);
    return Event(packetToRetransmit, 0);

	return Event();
}
void PacketRetransmission::exit() {
    //Code executed at the end of the simulation.
	this->logger->flush();
}

TdaqPacket* PacketRetransmission::getNextPacketToRetransmit(){
	return unACKPackets.front().packet;
}

double PacketRetransmission::getNextRetransmissionSigma(double t){
	if(unACKPackets.size() == 0)
		return INF;

	return unACKPackets.front().retransmissionTime - t;
}

