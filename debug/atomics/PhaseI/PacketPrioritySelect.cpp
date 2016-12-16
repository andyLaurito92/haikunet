#include "PacketPrioritySelect.h"

void PacketPrioritySelect::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	%Type% is the parameter type

	char *fvar;

	/* TODO: read parameters (fieldName; fieldValue -> Priority)
	 fvar = va_arg(parameters, char*);
	this->maxCapacity_bytes = readDefaultParameterValue<long>(fvar);
	if (this->maxCapacity_bytes == -1) this->maxCapacity_bytes = std::numeric_limits<long>::max();
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: MaxCapacity: %ld jobs \n", t, this->getFullName().data(), this->maxCapacity_bytes);
    */
	this->sigma = std::numeric_limits<double>::max(); // wait next packet to arrive
	this->arrivedPacket.reset();

	this->logger->initSignals(std::map<std::string,int>{
        {"priority", SCILAB_LOG_LEVEL_ALWAYS }
	});

	return;
}

void PacketPrioritySelect::dint(double t) {
	this->arrivedPacket.reset(); // release pointer (to be able to check in dext)
	this->priority = -1; // just to keep consistency
	this->sigma = std::numeric_limits<double>::max(); // wait next packet to arrive
}

void PacketPrioritySelect::dext(Event x, double t) {
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: event arrived to port %u \n", t, this->getFullName().data(), x.port);

	// if 2 packets arrive in the same t, the first one might be lost (this model preempts packets). It should never happen though
	if(this->arrivedPacket){
		debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: a packet arrived before the previous one was sent\n", t, this->getFullName().data());
		throw std::runtime_error("PacketPrioritySelect::dext: a packet arrived before the previous one was sent\n");
	}

	this->arrivedPacket = castEventPointer<NetworkPacket>(x); // get the packet

	auto protocol = arrivedPacket->getProtocol(PROTOCOL_ID_QUALITY_OF_SERVICE); // TODO: what if the procotol is not there?
	std::shared_ptr<QualityOfServiceProtocol> prot = std::static_pointer_cast<QualityOfServiceProtocol>(protocol); // TODO: what if it is the wrong type?
	int typeOfService = prot->getTypeOfService();

	// TODO: make the typeOfService --> priorityMapping
	this->priority = typeOfService;

	debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Arrived packet #%u with ToS=%u. Assigned priority=%u)\n", t, this->getFullName().data(), arrivedPacket->getId(), typeOfService, this->priority);

	this->sigma = 0; // forward packet immediately

	return;
}

Event PacketPrioritySelect::lambda(double t) {
	this->logger->logSignal(t, this->priority, "priority");

	// send packet with index==Priority
	auto vec = std::make_shared<VectorialEvent>(this->arrivedPacket, this->priority);

	return Event(vec, 0);
}

