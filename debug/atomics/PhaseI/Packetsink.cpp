#include "Packetsink.h"

void Packetsink::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	this->sigma = std::numeric_limits<double>::max(); // always just wait for packets to arrive

	this->logger->initSignals(std::map<std::string,int>{
		{"count", SCILAB_LOG_LEVEL_PRIORITY}, // arrived packets
		{"residence_time", SCILAB_LOG_LEVEL_ALWAYS}, // time since birthtime (
		{"bits", SCILAB_LOG_LEVEL_ALWAYS} // received bits
	});
}

void Packetsink::dint(double t) {
	this->sigma = std::numeric_limits<double>::max(); // always just wait for packets to arrive
	return;
}

void Packetsink::dext(Event x, double t) {
	// Packet arrived
	auto packet = castEventPointer<NetworkPacket>(x); // get the packet from the incoming DEVSEvent

//	if(isnan(packet->getBirthTime()) ) return; // avoid logging not well defined packets

	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u (size: %u bits; birthtime: %f; residenceTime=%f) \n", t, this->getName(), packet->getId(), packet->getLength_bits(), packet->getBirthTime(), t - packet->getBirthTime());

	this->packetCount++;


	// this is a workaround. If 2 or more packets arrive at the very same time the logger will only log the last one, so the logged throughput will be wrong.
	// TODO: this should be fixed at the logger, although this is a desired behavior for example for the queues.
	double tToLog = t;
	if(this->lastT == t){
		tToLog += 1.0e-10; // this is a magic number.. a very small double, but std::numeric_limits<double>::denorm_min() or min() does not work
		if(tToLog == this->lastT){
			debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: 2 packets arrived at the very same time, but the workaround at the sink did not work. This might cause problems at the logger, that will skip the first packet causing the throughput to be wrong. \n", t, this->getFullName().data(), tToLog);
		}
	}

	this->logger->logSignal(tToLog, this->packetCount,"count");
	this->logger->logSignal(tToLog, t - packet->getBirthTime(),"residence_time");
	this->logger->logSignal(tToLog, packet->getLength_bits(),"bits");

	this->lastT = t;
}

Event Packetsink::lambda(double t) {
	// should never arrive here: always just wait for packets to arrive
	return Event();
}
