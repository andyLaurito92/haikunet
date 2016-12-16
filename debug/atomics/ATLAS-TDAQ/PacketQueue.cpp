#include "PacketQueue.h"

void PacketQueue::init(double t, ...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	%Type% is the parameter type

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s: Init: \n", t, this->getName());

	char *fvar;

	fvar = va_arg(parameters, char*);
	MaxCapacity_bytes = (int) getScilabVar(fvar, true);
	fvar = va_arg(parameters,char*);
	ForcedPeriod = getScilabVar(fvar, true);

	if (MaxCapacity_bytes == -1) MaxCapacity_bytes = (int) INF;
	if (ForcedPeriod == -1) ForcedPeriod = INF;

	printLog(LOG_LEVEL_INIT, "[%f] %s: MaxCapacity: %i Bytes \n", t, this->getName(), MaxCapacity_bytes);
	printLog(LOG_LEVEL_INIT, "[%f] %s: ForcedPeriod: %f \n", t, this->getName(), ForcedPeriod);

	sigma = ForcedPeriod;
	discardedPackets = 0;
	cantPacketsInQueue = 0;
	queueSize_bytes = 0;

	for (int i = 0; i < 10; i++) {
		sizeout[i] = 0;
	}

	this->logger->initSignals({"discards", "eventId", "qsize"});

	mystate = WAITING;
	serverstate = SERVERBUSY;

	return;
}

void PacketQueue::dint(double t) {

	switch (mystate) {
	case WAITING:
		serverstate = SERVERWAITING;
		mystate = t_NOTIFYING;
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: Ready to notify new size \n", t, this->getName());
		sigma = 0;
		return;

	case t_NOTIFYING: // Notified length
		sizeout[1] = 0; // clear discard flag notification

		if (serverstate == SERVERWAITING && queueSize_bytes > 0){ // now emit
			mystate = t_READYTOEMIT;
			sigma = 0;
		} else { // not ready to send, so wait
			mystate = WAITING;
			sigma = ForcedPeriod;
		}

		return;

	case t_READYTOEMIT:
		// Emitted a packet, now notify length again
		if (serverstate == SERVERWAITING && queueSize_bytes > 0) {
			this->unqueuePacket();
			serverstate = SERVERBUSY;
			mystate = t_NOTIFYING;
			sigma = 0;

			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: DEQUEUED SIZE: %u \n", t, this->getName(), queueSize_bytes);
		} else {
			// Nothing else to do, go back waiting
			mystate = WAITING;
			sigma = ForcedPeriod;
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: Go to wait again \n", t, this->getName());
		}

	};
	return;
}
void PacketQueue::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Packet arrived to port %u \n", t, this->getName(), x.port);

	if (x.port == 0) { // Packet Arrived

		auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
		int arrivedSize = arrivedPacket->getLength_bytes();

		if (queueSize_bytes + arrivedSize > MaxCapacity_bytes) { // Queue Full: Rejects
			printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Incoming Packet ID %i Refused. Not enough capacity for packet with size %i bits  (currentSize=%i, MaxCapacity=%i) \n", t, this->getName(), arrivedPacket->ID, arrivedPacket->getLength_bits(), queueSize_bytes, MaxCapacity_bytes);
			printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Discarded Packet is from  ROS=%i \n", t, this->getName(), arrivedPacket->getFragmentInfo()->rosId);
 
			sizeout[1] = arrivedSize; // send a discard on the notifications
			this->logger->logSignal(t, (double) ++discardedPackets, "discards");
//			mystate = WAITING;
//			sigma = sigma - e; // continue as before
//			return;
		}else{ // Otherwise : Accepts
			arrivedPacket->currentReceptionTimestamp = t;
			this->queuePacket(arrivedPacket);

			printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Queued Packet ID #%u (queue size: %u bytes, %i packets)\n", t, this->getName(), arrivedPacket->ID, queueSize_bytes, cantPacketsInQueue);
		}

		mystate = t_NOTIFYING;
		sigma = 0;
	}

	if (x.port == 1) { // Request from Server Arrived
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Dequeue Requested by Server... \n", t, this->getName());

		if (this->queueSize_bytes > 0) { // Queue is not empty
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] Queue_ext: Non Empty Queue... \n", t);
			serverstate = SERVERWAITING;
			mystate = t_READYTOEMIT;
			sigma = 0; // Force dequeueing next available element at the head of the queue;
		} else { // Queue is empty
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Dequeue Failed: Empty Queue... \n", t, this->getName());
			serverstate = SERVERWAITING;
			mystate = t_READYTOEMIT;
			sigma = sigma - e; // Continues waiting for a new packet
		}
	}

	return;
}
Event PacketQueue::lambda(double t) {
	switch (mystate) {
	case WAITING: // Forced Period Elapsed
		// Sends Nothing
		return Event();
		break;
	case t_NOTIFYING: // Must notify new queue length
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: Notifying QUEUE SIZE: %i (bytes). %i packets in queue\n", t, this->getName(), queueSize_bytes, cantPacketsInQueue);
		//this->logger->logSignal(t, (double) queueSize_bytes, "queueLength");
		sizeout[0] = (double) queueSize_bytes;
		return Event(&sizeout, 1);
		break;
	case t_READYTOEMIT:
		if (serverstate == SERVERWAITING && queueSize_bytes > 0) {
			// Must emit a packet
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: Sending packet... NOW QUEUE SIZE: %u bytes \n ", t, this->getName(), queueSize_bytes);

			auto packetToSend = packetQueue.front();
//			packetToSend->lastQueueSizeAfterLeaving = this->getQueueSize_bytes() - 1;
                        if (strcmp (packetToSend->FlowID, FLOW_ID_1LTRIGGER_RESPONSE) == 0){
                            auto requestInfo = std::static_pointer_cast<FragmentInfo>(packetToSend->payload);
                            this->logger->logSignal(t, MaxCapacity_bytes, "qsize");
                            this->logger->logSignal(t,requestInfo -> eventId,"eventId");
                        }


			printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lamb: Dequeueing Packet ID #%u (queue size: %u bytes, %i packets)\n", t, this->getName(), packetToSend->ID, queueSize_bytes, cantPacketsInQueue);
			return Event(packetToSend, 0);
		} else {
			return Event();
		}

		break;
	}
}

void PacketQueue::queuePacket(std::shared_ptr<TdaqPacket> packetToQueue) {
	packetQueue.push_back(packetToQueue);
	int packetSize = packetToQueue->getLength_bytes();
	cantPacketsInQueue++;
	queueSize_bytes += packetSize;

	printLog(LOG_LEVEL_FULL_LOGGING, "[] %s_ext: enqueued Packet #%i (size=%i bits)   NEW QUEUE SIZE: %i bytes and %i packets \n", this->getName(), packetToQueue->ID, packetSize, queueSize_bytes, cantPacketsInQueue);
}

void PacketQueue::unqueuePacket() {
	auto packetToRemove = packetQueue.front();

	cantPacketsInQueue--;
	queueSize_bytes -= packetToRemove->getLength_bytes();
	packetQueue.pop_front();
}

