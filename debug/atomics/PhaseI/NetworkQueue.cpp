#include "NetworkQueue.h"

void NetworkQueue::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	%Type% is the parameter type

	char *fvar;

	fvar = va_arg(parameters, char*);
	this->maxCapacity_bytes = readDefaultParameterValue<long>(fvar);
	if (this->maxCapacity_bytes == -1) this->maxCapacity_bytes = std::numeric_limits<long>::max();
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: MaxCapacity: %ld jobs \n", t, this->getFullName().data(), this->maxCapacity_bytes);

	this->sigma = std::numeric_limits<double>::max();
	this->discardedPackets = 0;

	this->logger->initSignals(std::map<std::string,int>{
        {"discards", SCILAB_LOG_LEVEL_ALWAYS},
        {"qsize_bytes", SCILAB_LOG_LEVEL_ALWAYS },
        {"qsize_pkts", SCILAB_LOG_LEVEL_IMPORTANT }
	});
	this->logger->logSignal(t, 0, "discards");
	this->logger->logSignal(t, 0, "qsize_bytes");
	this->logger->logSignal(t, 0, "qsize_pkts");

	this->serverstate = SERVERBUSY; // start waiting for server request. TODO: evaluate start with SERVERWAITING (need to change server too)

	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: sigma=%f \n", t, this->getFullName().data(), this->sigma);
	return;
}

void NetworkQueue::dint(double t) {
	// Emitted a packet, dequeue and go back waiting (either to new packet or new request)
	this->unqueuePacket(t);
	this->sigma = std::numeric_limits<double>::max();
	this->serverstate = SERVERBUSY;

	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: packet served (and dequeued, queue size: %i packets), to wait again \n", t, this->getFullName().data(), this->cantPacketsInQueue);
}
void NetworkQueue::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: event arrived to port %u \n", t, this->getFullName().data(), x.port);

	if (x.port == 0) { // new job Arrived
		auto arrivedPacket = castEventPointer<NetworkPacket>(x); // get the job from the incoming event

		if (this->queueSize_bytes + arrivedPacket->getLength_bytes() >= (unsigned long)this->maxCapacity_bytes) { // Queue Full: Rejects
			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Incoming packet #%i (%u bytes) REFUSED. NetworkPacket DISCARDED Not enough capacity for packet (bufferSize=%lu, MaxCapacity=%l) \n", t, this->getFullName().data(), arrivedPacket->getId(), arrivedPacket->getLength_bytes(), this->queueSize_bytes, this->maxCapacity_bytes);
 			this->logger->logSignal(t, (double) ++this->discardedPackets, "discards");
		}else{ // Otherwise : Accepts
			this->queuePacket(t, arrivedPacket);

			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Queued packet #%u (%u bytes). queue size: %lu bytes (%u packets)\n", t, this->getFullName().data(), arrivedPacket->getId(), arrivedPacket->getLength_bytes(), this->queueSize_bytes, this->queuedPackets.size());
		}
	}else if (x.port == 1) { // Request from Server Arrived
		debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Dequeue Requested by Server... \n", t, this->getFullName().data());
		if(this->serverstate == SERVERWAITING){
			throw std::runtime_error(this->getFullName() + " - NetworkQueue::dext: Received request from server which already has requested. Only a single request can be issued to the queue.");
		}
		this->serverstate = SERVERWAITING;
	}

	if (!this->queuedPackets.empty() && this->serverstate == SERVERWAITING) { // Queue is not empty and server waiting
		debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s: server requested next packet, serving it ... \n", t, this->getFullName().data());
		this->sigma = 0; // Force dequeueing next available element at the head of the queue;
	} else { // Queue is empty
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: keep waiting (server not requested or empty queue) \n", t, this->getFullName().data());
		this->sigma = this->sigma - this->e; // Continues as before
	}

	return;
}

Event NetworkQueue::lambda(double t) {
	if (this->serverstate == SERVERWAITING && this->queuedPackets.size() > 0) {
		// Must emit a packet
		auto jobToSend = this->queuedPackets.front();

		debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_lamb: sending job #%u (queue size: %i packets)\n", t, this->getFullName().data(), jobToSend->getId(), this->queuedPackets.size());
		return Event(jobToSend, 0);
	} else {
		throw std::runtime_error("NetworkQueue::lambda: inconsistent state. Empty queue or server not waiting");
		return Event();
	}

	return Event();
}

void NetworkQueue::queuePacket(double t, std::shared_ptr<NetworkPacket> packetToQueue) {
	this->queuedPackets.push_back(packetToQueue);
	int packetSize = packetToQueue->getLength_bytes();
	cantPacketsInQueue++;
	queueSize_bytes += packetSize;

	this->logger->logSignal(t, (double) this->queueSize_bytes, "qsize_bytes");
	this->logger->logSignal(t, (double) this->queuedPackets.size(), "qsize_pkts");
}

void NetworkQueue::unqueuePacket(double t) {
	auto packetToRemove = this->queuedPackets.front();

	cantPacketsInQueue--;
	queueSize_bytes -= packetToRemove->getLength_bytes();
	this->queuedPackets.pop_front();

	this->logger->logSignal(t, (double) this->queueSize_bytes, "qsize_bytes");
	this->logger->logSignal(t, (double) this->queuedPackets.size(), "qsize_pkts");
}
