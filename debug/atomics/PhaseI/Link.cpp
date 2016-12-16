#include "Link.h"

void Link::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char* fvar;
	fvar = va_arg(parameters,char*);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: INIT %s \n", t, this->getFullName().data(), fvar);

	this->capacity_bit_s_r = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: capacity_bit_s: %f bits/s. \n", this->getFullName().data(), this->capacity_bit_s_r);
	this->capacity_bit_s_r = 1 / this->capacity_bit_s_r; // revert it to avoid continuous divisions later
	
	fvar = va_arg(parameters,char*);
	this->delay_s = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: delay_s: %f s (%f ms). \n", this->getFullName().data(), this->delay_s, this->delay_s*1000);

	this->mystate = FIRST_REQUEST;
	this->sigma = 0; // Immediately asks for a new packet

	this->logger->initSignals(std::map<std::string,int>{
		{"sent_bits", SCILAB_LOG_LEVEL_ALWAYS},
		{"sent_packets", SCILAB_LOG_LEVEL_PRIORITY},
		{"processingTime", SCILAB_LOG_LEVEL_DEBUG},
	});

}

void Link::dint(double t) {
	switch (this->mystate) {
		case PROCESSING:
		case FIRST_REQUEST:
			this->mystate = WAITING_NEXT_JOB;
			this->sigma = std::numeric_limits<double>::max(); // wait for ever
			debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_int: WAITING for next packet to arrive. \n", t, this->getFullName().data());
			break;
		default:
			debugMsg(LOG_LEVEL_ERROR, "[%f] %s_int: Error: Do not know what to do... \n", t, this->getFullName().data());
			throw std::runtime_error("Link: Invalid model state. see log");
			break;
		}

	return;
}

void Link::dext(Event x, double t) {
	// Safetly check: new packet shoul not arrive until present one is delivered. This is a channel and does not buffer packets.
	if(this->mystate != WAITING_NEXT_JOB){
		debugMsg(LOG_LEVEL_ERROR, "[%f] Link_dext: Error: Packet arrived at channel when another packet (packet ID #%u) was in the channel.  \n", t, this->getFullName().data(), this->incomingPacket->getId());
		throw std::runtime_error( std::string("Packet arrived at channel '") + std::string(this->getFullName()) + std::string("' when another packet was present in the channel"));
	}

	// Request arrived
	this->incomingPacket = castEventPointer<NetworkPacket>(x); // get the packet from the incoming DEVSEvent

	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u (size: %u bits) \n", t, this->getFullName().data(), incomingPacket->getId(), incomingPacket->getLength_bits());

	// Calculate sigma according packet size
	this->sigma = this->capacity_bit_s_r * incomingPacket->getLength_bits() + this->delay_s;

	debugMsg(LOG_LEVEL_PRIORITY, "[%.10f] %s_ext: Calculated delay for packet #%u: %.10f s. PacketSize: %u bits  \n", t, this->getFullName().data(), incomingPacket->getId(), sigma, incomingPacket->getLength_bits());

	mystate = PROCESSING; // NOTE: This Server Preempts the previous task if a new packet arrives (unexpectedly) before the completion of the Processing period
}

Event Link::lambda(double t) {
	switch (mystate) {
	case PROCESSING: {// send the packet
		debugMsg(LOG_LEVEL_PRIORITY, "[%.10f] %s_lamb: emitting Packet ID #%u \n", t, this->getFullName().data(), this->incomingPacket->getId());

		this->logger->logSignal(t, this->incomingPacket->getLength_bits(), "sent_bits");
		this->logger->logSignal(t, 1, "sent_packets");
		this->logger->logSignal(t, this->sigma,"processingTime");

		return Event(this->incomingPacket, 0);
		break;
	} case FIRST_REQUEST: {
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%.10f] %s_lamb: asking for first packet... \n", t, this->getFullName().data());
		return Event(std::make_shared<NetworkPacket>(t), 0); // any sent value is OK to signal the request, we use sigma just because it is there.
		break;
	} default:{
		debugMsg(LOG_LEVEL_ERROR, "[%.10f] %s_lamb: Error: Do not know what to do... \n", t, this->getFullName().data());
		throw std::runtime_error("Link.lambda: Wrong model state. Check log.");
		break;
	}
	}
}

