#include "TcpSender.h"

void TcpSender::init(double t, ...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

	char *fvar;
	fvar = va_arg(parameters, char*);
	minRetransmissionTimeOut = readDefaultParameterValue<double>(fvar);
	if (minRetransmissionTimeOut <= 0)	minRetransmissionTimeOut = INF;
	printLog(LOG_LEVEL_INIT, "[%f] %s: minRetransmissionTimeOut: %f \n", t, this->getName(), minRetransmissionTimeOut);

	fvar = va_arg(parameters, char*);
	slowStartThreshold = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s: slowStartThreshold: %u \n", t, this->getName(), slowStartThreshold);

	fvar = va_arg(parameters, char*);
	calculatedRtt = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s: calculatedRtt: %f \n", t, this->getName(), calculatedRtt);

	fvar = va_arg(parameters, char*);
	rttAlpha = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s: RTT_alfa: %f \n", t, this->getName(), rttAlpha);

	sigma = INF;
	lastSequenceNumber = 0;

	congestionWindow = 1;
	congestionAvoidanceState = SLOW_START;
	calculatedRttDeviation = 0;
	retransmissionTimeOut = minRetransmissionTimeOut;

	totalRetransmissions = 0;
	totalSentPackets = 0;
	//this->logger = createDefaultLogger(SCILAB_LOG_LEVEL_NORMAL, this->getName());
//	this->logger->initSignals(8, "retransmissions", "sent", "cwnd", "rto", "calcRtt", "ignoredAcks", "measuredRtt", "calcRttDev");
	//this->logger->initSignals(5, "rto", "cwnd", "measuredRtt", "calcRtt", "calcRttDev");

	return;
}
double TcpSender::ta(double t) {
	return sigma;
}
void TcpSender::dint(double t) {
	printLog(5, "[%f] %s_int: myState=%u. \n",t, this->getName(), myState);

//	switch (myState) {
//		case FORWARDING_PACKET: { // a packet was forwarded
//			myState = WAITING;
//			break;
//		} case RETRANSMITTING_PACKET: { // a packet was retransmitted
//			myState = WAITING;
//
//			break;
//		} default:
//			break;
//	}

	// recalculate sigma
	this->setNextSigma(t);
	printLog(5, "[%f] %s_int: programmed in %fs \n", t, this->getName(), sigma);
}

void TcpSender::dext(Event x, double t) {
	// A packet arrived
	printLog(5, "[%f] %s_ext: Message arrived to port %u. \n",t, this->getName(), x.port);

	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);

	// Safety Checks
	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(1, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	if (x.port == 0) { // Packet to be forwarded
		// add sequence number to the packet
		arrivedPacket->getProtocol(PROTOCOL_ID_TCP)->headerFields[SEQ] = this->lastSequenceNumber ++;

		// store packet and its timeout
		this->sendBuffer.push_back( std::make_shared<TcpQueuedPacket>(arrivedPacket));

		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: added new packet (#%i), its retransmission time is: %f . Total buffered packets=%i\n", t, this->getName(), arrivedPacket->ID, t + this->retransmissionTimeOut, this->sendBuffer.size());
	}
	else if (x.port == 1) { // Response Packet (ACK)
		auto unAckPacketIt = validateReceivedAckPacket(t, arrivedPacket);
		if(unAckPacketIt == sendBuffer.end()) return;

		if(unAckPacketIt != sendBuffer.begin()){
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Wrong order ACK (duplicate ACK?). Current RTO=%f, CWND=%i \n", t, this->getName());
			// continue as before and ignore packet
			sigma = sigma - e; return; // don;t accept out of order ACKs (comment this line to enable pseudo-SACK)
		}

		(*unAckPacketIt)->ackReceived = true;
		(*unAckPacketIt)->ackPacket = arrivedPacket;

		// update RTO & CA
		setNewRTOAfterAckReceived(t, *unAckPacketIt);
		setNewCAAfterAckReceived(t,  *unAckPacketIt);

		// remove it from unACK list
		int packetId = (*unAckPacketIt)->packet->ID;
		sendBuffer.erase(unAckPacketIt);

		auto tcpProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_TCP);
		int ackSeqNumber = tcpProtocol->headerFields[ACK];
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: packet #%i (ackSeqNumber: %i) removed from unACK packet list. Total buffered packets=%u \n", t, this->getName(), packetId, ackSeqNumber, sendBuffer.size());
		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: ACK for packet #%i (ackSeqNumber: %i) processed. Current RTO=%f, CWND=%i Total buffered packets=%u\n", t, this->getName(), packetId, ackSeqNumber, this->retransmissionTimeOut, this->congestionWindow, sendBuffer.size());
	}

	this->setNextSigma(t);
	printLog(5, "[%f] %s_ext: next transition programmed in the next %fs \n", t, this->getName(), sigma);
}

Event TcpSender::lambda(double t) {
	printLog(5, "[%f] %s_lambda: myState=%u. \n",t, this->getName(), myState);
	switch (myState) {
		case FORWARDING_PACKET: { // need to forward next packet
			auto packetToForward = this->getNextPacketPendingForward();
			if(packetToForward == NULL){
				printLog(1, "[%f] %s_lambda: Model set to forward a packet which is not available. \n",t, this->getName());
				throw "invalid model state";
			}

			if(packetToForward->forwarded){ // if is was forwarded before, now we are retransmitting
				packetToForward->retransmissions++;
			}

			// Update packet status
			packetToForward->forwarded = true;
			packetToForward->onTheFly = true;
			packetToForward->packet->birthTime = t;

			totalSentPackets++;
			//this->logger->logSignal(t, totalSentPackets, "sent");

			printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lambda: forwarding packet (#%i retransmission %i). Current RTO=%f, CWND=%i \n", t, this->getName(), packetToForward->packet->ID, packetToForward->retransmissions,  this->retransmissionTimeOut, this->congestionWindow);
			this->lastTransmissionTime = t;
			return Event(packetToForward->packet, 0);
			break;
		} case RETRANSMITTING_PACKET: { // Timeout occurred, need to retransmit next packet
			auto packetToRetransmit = this->getNextPacketToRetransmit();

			if(packetToRetransmit == NULL){
				printLog(1, "[%f] %s_lambda: Model set to retransmit a packet which is not available. \n",t, this->getName());
				throw "invalid model state";
			}

			printLog(LOG_LEVEL_INIT, "[%f] %s_lambda: A Retransmission Detected for packet #%i (packetToRetransmit). Current RTO=%f, CWND=%i . \n",t, this->getName(), packetToRetransmit->packet->ID, this->retransmissionTimeOut, this->congestionWindow);

			// Clear the status of all packets in the CWND
			std::for_each(sendBuffer.begin(), sendBuffer.end(), [&] (std::shared_ptr<TcpQueuedPacket> p) {
				p->onTheFly = false;
			});
			if(packetToRetransmit->onTheFly == true){
				printLog(LOG_LEVEL_ERROR, "[DEBUG] %s_for_each: (WARNING) for each is not working property to set forwarded = false \n",this->getName());
				throw;
			}

			// update CWND & CA
			setNewRTOAfterARetransmission();
			setNewCAAfterARetransmission();

			// Update packet status
			packetToRetransmit->forwarded = true;
			packetToRetransmit->retransmissions++;
			packetToRetransmit->onTheFly = true;
			packetToRetransmit->packet->birthTime = t;

			totalRetransmissions++;
			totalSentPackets++;
			//this->logger->logSignal(t, totalRetransmissions, "retransmissions");
			//this->logger->logSignal(t, totalSentPackets, "sent");

			printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lambda: retransmitting packet (#%i).Current RTO=%f, CWND=%i \n", t, this->getName(), packetToRetransmit->packet->ID,this->retransmissionTimeOut, this->congestionWindow);
			this->lastTransmissionTime = t;
			return Event(packetToRetransmit->packet, 0);
			break;
		} default:
			printLog(1, "[%f] %s_lambda: ERROR \n", t, this->getName());
			throw;
			break;
	}

	return Event();
}

void TcpSender::exit() {
	//this->logger->flush();
}

double TcpSender::setNextSigma(double t) {
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_setNextSigma: getAvailableCongestionWindow=%u; unACKed packets =%u; Total buffered packets=%u;  \n",t, this->getName(), getAvailableCongestionWindow(), getUnAckPackets(), sendBuffer.size());

	auto nextPacketToForward = getNextPacketPendingForward();
	if(getAvailableCongestionWindow() > 0 && nextPacketToForward != NULL) { // if there is enough CWND and packets
		myState = FORWARDING_PACKET;
		sigma = 0; // Immediately forward a packet

		// TODO: this a workaround to reproduce 0.6s latencies observed in tcpdumps of TR7.test1
		// when there is a first retransmission of a segment and ACK is received for that segment but the next packet to send is the last one (in the tcpdumps it is a small 390 bytes packet), this small last packet it not sent immediately. It is send only after a timeout of 0.4s
		bool isLastRetransmitt = this->sendBuffer.size() == 1 && nextPacketToForward->forwarded == true && this->congestionWindow == 2 /* && myState == RETRANSMITTING_PACKET */;
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_setNextSigma: isLastRetransmitt=%s  "
				"getAvailableCongestionWindow=%u; unACKed packets =%u; Total buffered packets=%u;"
				"nextPacketToForward->forwarded=%s ; myState=%u ; RTO=%f"
				"\n",t, this->getName(), isLastRetransmitt?"YES":"NO", getAvailableCongestionWindow(), getUnAckPackets(), sendBuffer.size(), nextPacketToForward->forwarded?"YES":"NO", myState, retransmissionTimeOut  );
		if(isLastRetransmitt){
			myState = RETRANSMITTING_PACKET;
			sigma = getNextRetransmissionTime() - t; // next expiration time is to retransmit
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_setNextSigma: 0.6s latency hack used. Not forwarding next packet until timeout occurs.  \n",t, this->getName(), getAvailableCongestionWindow(), getUnAckPackets(), sendBuffer.size());
		}
	}
	else { //
		myState = RETRANSMITTING_PACKET;
		sigma = getNextRetransmissionTime() - t; // next expiration time is to retransmit
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_setNextSigma: myState=%u, sigma=%f , available CWND=%u, CWND size = %u \n",t, this->getName(), myState, sigma, getAvailableCongestionWindow(), congestionWindow);

	//this->logger->logSignal(t, congestionWindow, "cwnd");
	//this->logger->logSignal(t, retransmissionTimeOut, "rto");

	return sigma;
}

std::shared_ptr<TcpSender::TcpQueuedPacket> TcpSender::getNextPacketToRetransmit() {
	return sendBuffer.front();
}

std::shared_ptr<TcpSender::TcpQueuedPacket> TcpSender::getNextPacketPendingForward() {
	auto notForwardedPacketIt = std::find_if(sendBuffer.begin(), sendBuffer.end(), [&] (const std::shared_ptr<TcpQueuedPacket> p) {
		return (p->onTheFly == false);
	});

	if(notForwardedPacketIt == sendBuffer.end()){
		printLog(5, "[DEBUG] %s_getNextPacketPendingForward: NOthing to forward \n", this->getName());
		return NULL;
	}

	return *notForwardedPacketIt;
}

double TcpSender::getNextRetransmissionTime() {
	if(sendBuffer.size() == 0)
		return INF;

	return this->lastTransmissionTime + this->retransmissionTimeOut;
}

int TcpSender::getAvailableCongestionWindow(){
	return congestionWindow - getUnAckPackets();
}

int TcpSender::getUnAckPackets(){
	return std::count_if(sendBuffer.begin(), sendBuffer.end(), [&] (const std::shared_ptr<TcpQueuedPacket> p) {
			return (p->ackReceived == false && p->forwarded == true  && p->onTheFly == true);
	});
}

int TcpSender::setNewRTOAfterAckReceived(double t, std::shared_ptr<TcpQueuedPacket> receivedQueuedPacket){
	auto ackPacket = receivedQueuedPacket->ackPacket;

	if(!receivedQueuedPacket->retransmissions){ // Only recalculate RTO if it was not from a retransmissions packet: Karn's Algorithm
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_setNewRTOAfterAckReceived: (DEBUG) RTO calculation accepted %f \n", t, this->getName(), retransmissionTimeOut);

		auto tcpProtocol = ackPacket->getProtocol(PROTOCOL_ID_TCP);
		double measuredRtt = t - tcpProtocol->headerFields[ORIGINAL_PACKET_BIRTH_TIME];

		//this->logger->logSignal(t, measuredRtt, "measuredRtt");

		calculatedRtt = (1-rttAlpha) * calculatedRtt + rttAlpha * measuredRtt ;
		calculatedRttDeviation = (1-rttAlpha * 2) * calculatedRttDeviation + (rttAlpha * 2) * fabs(calculatedRtt - measuredRtt);

		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_setNewRTOAfterAckReceived: measuredRtt=%f, calculatedRtt=%f, calculatedRttDeviation=%f (rttAlpha=%f) \n", t, this->getName(), measuredRtt, calculatedRtt, calculatedRttDeviation, rttAlpha);
//		this->logger->logSignal(t, calculatedRtt, "calcRtt");
//		this->logger->logSignal(t, calculatedRttDeviation, "calcRttDev");

		retransmissionTimeOut = calculatedRtt + 4 * calculatedRttDeviation;
		retransmissionTimeOut =  std::max(minRetransmissionTimeOut, retransmissionTimeOut);
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_setNewRTOAfterAckReceived: retransmissionTimeOut set to %f \n", t, this->getName(), retransmissionTimeOut);
	}
	else{
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_setNewRTOAfterAckReceived: (DEBUG) RTO calculation ignored %f \n", t, this->getName(), retransmissionTimeOut);
	}

	return retransmissionTimeOut;
}

int TcpSender::setNewCAAfterAckReceived(double t, std::shared_ptr<TcpQueuedPacket> receivedQueuedPacket){
	auto ackPacket = receivedQueuedPacket->ackPacket;

	switch (congestionAvoidanceState) {
				case SLOW_START:
					congestionWindow++;
					printLog(5, "[%f] %s_setNewCAAfterAckReceived: SLOW_START, congestion window increased. Available CWND=%u, CWND size = %u \n",t, this->getName(), getAvailableCongestionWindow(), congestionWindow);

					if(slowStartThreshold <= congestionWindow){ // reach the Slow Start limit
						// swap to CA
						congestionAvoidanceState = CONGESTION_AVOIDANCE;
						pendingCongestionAvoidanceAcksToIncreaseWindow = congestionWindow;

						printLog(5, "[%f] %s_setNewCAAfterAckReceived: switch from SLOW_START to CONGESTION_AVOIDANCE  \n",t, this->getName());
					}

					break;
				case CONGESTION_AVOIDANCE:
					if(pendingCongestionAvoidanceAcksToIncreaseWindow == 0){
						congestionWindow++;
						pendingCongestionAvoidanceAcksToIncreaseWindow = congestionWindow;

						printLog(5, "[%f] %s_setNewCAAfterAckReceived: CONGESTION_AVOIDANCE window increased. Available CWND=%u, CWND size = %u \n",t, this->getName(), getAvailableCongestionWindow(), congestionWindow);
					}
					else {
						pendingCongestionAvoidanceAcksToIncreaseWindow--;
						printLog(5, "[%f] %s_setNewCAAfterAckReceived: CONGESTION_AVOIDANCE ack received. %u ACKs pending to increase window \n",t, this->getName(), pendingCongestionAvoidanceAcksToIncreaseWindow);
					}

					break;
				default:
					break;
			}

	return congestionWindow;
}

int TcpSender::setNewRTOAfterARetransmission(){
	retransmissionTimeOut = retransmissionTimeOut * 2; // TODO: look for a proper calculation according to the TCP version
	printLog(5, "%s_setNewRTOAfterARetransmission: retransmissionTimeOut set to %f \n", this->getName(), retransmissionTimeOut);

	return retransmissionTimeOut;
}

int TcpSender::setNewCAAfterARetransmission(){
	slowStartThreshold = congestionWindow / 2;
	congestionWindow = 1; // TODO: triple ACKs are not modeled
	congestionAvoidanceState = SLOW_START; // switch to SLOW_START


	printLog(5, "%s_setNewCWNDAfterARetransmission: congestionWindow set to %u \n", this->getName(), congestionWindow);

	return congestionWindow;
}

std::deque<std::shared_ptr<TcpSender::TcpQueuedPacket>>::iterator TcpSender::validateReceivedAckPacket(double t, std::shared_ptr<TdaqPacket> arrivedPacket) {
	char* flowID = arrivedPacket->FlowID;
	auto tcpProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_TCP);

	// Safety Checks
	if (strcmp (flowID,FLOW_ID_ACK) != 0 ) {
		printLog(1, "[%f] %s_ext: [WARNING] packet (#%i) does not contain the FlowID=ACK (ignore it) \n", t, this->getName(), arrivedPacket->ID);
		sigma = sigma - e; // continue as before
		throw;
		return sendBuffer.end();
	}
	if(tcpProtocol == NULL){
		printLog(1, "[%f] %s_ext: [WARNING] packet (#%i) does not contain the TCP protocol (ignore it) \n", t, this->getName(), arrivedPacket->ID);
		sigma = sigma - e; // continue as before
		throw;
		return sendBuffer.end();
	}

	// Get the ACK ID from the TCP protocol
	int ackSeqNumber = tcpProtocol->headerFields[ACK];

	// Check if we where waiting for this packet
	auto unAckPacketIt = std::find_if(sendBuffer.begin(), sendBuffer.end(), [&] (const std::shared_ptr<TcpQueuedPacket> p) {
		return (p->packet->getProtocol(PROTOCOL_ID_TCP)->headerFields[SEQ] == ackSeqNumber &&
				p->forwarded == true && p->ackReceived == false );
	});

	if(unAckPacketIt == sendBuffer.end()){
		printLog(5, "[%f] %s_ext: seqNumber:%i was not waiting for ACK (ignore it) \n", t, this->getName(), ackSeqNumber);
		arrivedPacket->printPacketInfo(10);
		ignoredAcks++;
		//this->logger->logSignal(t, ignoredAcks, "ignoredAcks");
		sigma = sigma - e; // continue as before
		return sendBuffer.end();
	}

	return unAckPacketIt;
}



