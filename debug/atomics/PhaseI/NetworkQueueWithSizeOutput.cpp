#include "NetworkQueueWithSizeOutput.h"

void NetworkQueueWithSizeOutput::init(double t, ...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	%Type% is the parameter type

	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s: Init. DebugLevel:%u   \n", t, this->getName(), this->debugLevel);

	char *fvar;

	fvar = va_arg(parameters, char*);
	unsigned long int capacityParam = readDefaultParameterValue<unsigned long int>(fvar);
	if (capacityParam < 0) {
		this->maxCapacity_bytes = std::numeric_limits<unsigned long int>::max(); // use -1 to set infinite buffer capacity
	} else {
		this->maxCapacity_bytes = capacityParam;
	}

	fvar = va_arg(parameters,char*);
	forcedPeriod = readDefaultParameterValue<double>(fvar);
	if (forcedPeriod == -1) forcedPeriod = std::numeric_limits<int>::max();

	debugMsg(LOG_LEVEL_INIT, "[%f] %s: MaxCapacity: %i Bytes \n", t, this->getName(), maxCapacity_bytes);
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: ForcedPeriod: %f \n", t, this->getName(), forcedPeriod);

	sigma = forcedPeriod;
	discardedPackets = 0;
	cantPacketsInQueue = 0;
	queueSize_bytes = 0;

	for (int i = 0; i < 10; i++) {
		sizeout[i] = 0;
	}

	this->logger->initSignals(std::map<std::string,int>{
		{"discards", LOG_LEVEL_PRIORITY},
		{"eventId", LOG_LEVEL_PRIORITY},
		{"qsize", LOG_LEVEL_PRIORITY}
	});

	mystate = WAITING;
	serverstate = SERVERBUSY;

	return;
}

void NetworkQueueWithSizeOutput::dint(double t) {

	switch (mystate) {
	case WAITING:
		serverstate = SERVERWAITING;
		mystate = t_NOTIFYING;
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: Ready to notify new size \n", t, this->getName());
		sigma = 0;
		return;

	case t_NOTIFYING: // Notified length
		sizeout[1] = 0; // clear discard flag notification

		if (serverstate == SERVERWAITING && queueSize_bytes > 0){ // now emit
			mystate = t_READYTOEMIT;
			sigma = 0;
		} else { // not ready to send, so wait
			mystate = WAITING;
			sigma = forcedPeriod;
		}

		return;

	case t_READYTOEMIT:
		// Emitted a packet, now notify length again
		if (serverstate == SERVERWAITING && queueSize_bytes > 0) {
			this->unqueuePacket();
			serverstate = SERVERBUSY;
			mystate = t_NOTIFYING;
			sigma = 0;

			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: DEQUEUED SIZE: %u \n", t, this->getName(), queueSize_bytes);
		} else {
			// Nothing else to do, go back waiting
			mystate = WAITING;
			sigma = forcedPeriod;
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: Go to wait again \n", t, this->getName());
		}

	};
	return;
}
void NetworkQueueWithSizeOutput::dext(Event x, double t) {
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Packet arrived to port %u \n", t, this->getName(), x.port);

	if (x.port == 0) { // Packet Arrived
		auto arrivedPacket = std::static_pointer_cast<NetworkPacket>(x.valuePtr);
		int arrivedSize = arrivedPacket->getLength_bytes();

		if (queueSize_bytes + arrivedSize > maxCapacity_bytes) { // Queue Full: Rejects
			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Incoming Packet ID %i Refused. Not enough capacity for packet with size %i bits  (currentSize=%i, MaxCapacity=%i) \n", t, this->getName(), arrivedPacket->getId(), arrivedPacket->getLength_bits(), queueSize_bytes, maxCapacity_bytes);

			sizeout[1] = arrivedSize; // send a discard on the notifications
			this->logger->logSignal(t, (double) ++discardedPackets, "discards");
//			mystate = WAITING;
//			sigma = sigma - e; // continue as before
//			return;
		}else{ // Otherwise : Accepts
			this->queuePacket(arrivedPacket);

			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Queued Packet ID #%u (queue size: %u bytes, %i packets)\n", t, this->getName(), arrivedPacket->getId(), queueSize_bytes, cantPacketsInQueue);
		}

		mystate = t_NOTIFYING;
		sigma = 0;
	}

	if (x.port == 1) { // Request from Server Arrived
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Dequeue Requested by Server... \n", t, this->getName());

		if (this->queueSize_bytes > 0) { // Queue is not empty
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] Queue_ext: Non Empty Queue... \n", t);
			serverstate = SERVERWAITING;
			mystate = t_READYTOEMIT;
			sigma = 0; // Force dequeueing next available element at the head of the queue;
		} else { // Queue is empty
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Dequeue Failed: Empty Queue... \n", t, this->getName());
			serverstate = SERVERWAITING;
			mystate = t_READYTOEMIT;
			sigma = sigma - e; // Continues waiting for a new packet
		}
	}

	return;
}
Event NetworkQueueWithSizeOutput::lambda(double t) {
	switch (mystate) {
	case WAITING: // Forced Period Elapsed
		// Sends Nothing
		return Event();
		break;
	case t_NOTIFYING: // Must notify new queue length
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: Notifying QUEUE SIZE: %i (bytes). %i packets in queue\n", t, this->getName(), queueSize_bytes, cantPacketsInQueue);
		//this->logger->logSignal(t, (double) queueSize_bytes, "queueLength");
		sizeout[0] = (double) queueSize_bytes;
		return Event(&sizeout, 1);
		break;
	case t_READYTOEMIT:
		if (serverstate == SERVERWAITING && queueSize_bytes > 0) {
			// Must emit a packet
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: Sending packet... NOW QUEUE SIZE: %u bytes \n ", t, this->getName(), queueSize_bytes);

			auto packetToSend = packetQueue.front();

			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_lamb: Dequeueing Packet ID #%u (queue size: %u bytes, %i packets)\n", t, this->getName(), packetToSend->getId(), queueSize_bytes, cantPacketsInQueue);
			return Event(packetToSend, 0);
		} else {
			return Event();
		}

		break;
	}

	throw std::runtime_error("NetworkQueueWithSizeOutput::lambda bad model state /n ");
}

void NetworkQueueWithSizeOutput::queuePacket(std::shared_ptr<NetworkPacket> packetToQueue) {
	packetQueue.push_back(packetToQueue);
	int packetSize = packetToQueue->getLength_bytes();
	cantPacketsInQueue++;
	queueSize_bytes += packetSize;

	debugMsg(LOG_LEVEL_FULL_LOGGING, "[???] %s_ext: enqueued Packet #%i (size=%i bits)   NEW QUEUE SIZE: %i bytes and %i packets \n", this->getName(), packetToQueue->getId(), packetSize, queueSize_bytes, cantPacketsInQueue);
}

void NetworkQueueWithSizeOutput::unqueuePacket() {
	auto packetToRemove = packetQueue.front();

	cantPacketsInQueue--;
	queueSize_bytes -= packetToRemove->getLength_bytes();
	packetQueue.pop_front();
}

