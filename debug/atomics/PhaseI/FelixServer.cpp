#include "FelixServer.h"

void FelixServer::init(double t,...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	  %Type% is the parameter type

    // get the flows for this server
	this->allmyFlows = FlowDefinitions::getSourceFlows(this->getFullName());
	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: There are %u flows defined for this server. \n", this->getFullName().data(), this->allmyFlows.size());

	// If there are no flows defined for this server, stay there waiting for ever and never generate anything
	if(this->allmyFlows.size() == 0) {
		debugMsg(LOG_LEVEL_ERROR, "[INIT] %s: There are 0 flows defined for this server. It will never generate anything.  \n", this->getFullName().data());
		this->sigma = std::numeric_limits<double>::infinity();
		return;
	}


	// variables to be logged
	std::map<std::string,int> flowsVariables;
	// global variables (sum of the rest)
	flowsVariables["count"] = SCILAB_LOG_LEVEL_PRIORITY;
	flowsVariables["intergen"] = SCILAB_LOG_LEVEL_PRIORITY;
	flowsVariables["sent_bits"] = SCILAB_LOG_LEVEL_ALWAYS;

	// variables particular per flow
	this->currFlowToGenPacket = 0;
	for(auto& flow : this->allmyFlows){
		auto index = &flow - &this->allmyFlows[0];
		debugMsg(LOG_LEVEL_PRIORITY, "[INIT] %s: Flow %u \n",t, this->getFullName().data(), index);
		debugMsg(LOG_LEVEL_PRIORITY, "[INIT] %s: StartTime: %f secs. \n",t, this->getFullName().data(), flow->getStartTime());
		debugMsg(LOG_LEVEL_PRIORITY, "[INIT] %s: generation period: %s \n", this->getFullName().data(), flow->getPeriod()->getFullName().data());
		debugMsg(LOG_LEVEL_PRIORITY, "[INIT] %s: PacketSize: %s (bits) \n", this->getFullName().data(), flow->getSize()->getFullName().data());

		this->nextsValuesOfGeneration.push_back(flow->getPeriod()->nextValue() + std::max(flow->getStartTime() - t, 0.0));

		flowsVariables["count_"+flow->getFlowId()] = SCILAB_LOG_LEVEL_DEBUG;
		flowsVariables["intergen_"+flow->getFlowId()] = SCILAB_LOG_LEVEL_DEBUG;
		flowsVariables["sent_bits_"+flow->getFlowId()] = SCILAB_LOG_LEVEL_PRIORITY;
	}
	this->logger->initSignals(flowsVariables);

	this->SetStateForNextFlowToGeneratePacket(t);
}

void FelixServer::dint(double t) {
	this->nextsValuesOfGeneration[this->currFlowToGenPacket] = this->allmyFlows[this->currFlowToGenPacket]->getPeriod()->nextValue() + t;

	this->SetStateForNextFlowToGeneratePacket(t);

	return;
}
void FelixServer::dext(Event x, double t) {
	return; // no input ports
}

Event FelixServer::lambda(double t) {
	auto packet = createPacket(t);

	// log
	this->logger->logSignal(t, this->sigma, "intergen_"+this->flow->getFlowId());
	this->logger->logSignal(t, this->sigma, "intergen");
	this->logger->logSignal(t, this->counter, "count_"+this->flow->getFlowId());
	this->logger->logSignal(t, this->counter, "count");
	this->logger->logSignal(t, packet->getLength_bits(), "sent_bits_"+this->flow->getFlowId());
	this->logger->logSignal(t, packet->getLength_bits(), "sent_bits");

	int level = LOG_LEVEL_PRIORITY;
	if( (this->counter % 100000 ) == 0){ // to show some progress every now and then
		level = LOG_LEVEL_ALWAYS; // always
	}
	debugMsg(level, "[%f] %s_lamda: Sending packet #%u (size=%u bits), generated after %f \n", t, this->getFullName().data(), packet->getId(), packet->getLength_bits(),  this->sigma);

	// send the new job
	return Event(packet, 0);
}

void FelixServer::SetStateForNextFlowToGeneratePacket(double t){
	for(unsigned int i = 0; i < this->allmyFlows.size(); i++ ){
		double minNextPacketGeneration = this->nextsValuesOfGeneration[this->currFlowToGenPacket];
		double candidateNextPacketGeneration = this->nextsValuesOfGeneration[i];

		if( minNextPacketGeneration > candidateNextPacketGeneration )	this->currFlowToGenPacket = i;
	}
	this->setNextFlow(this->allmyFlows[this->currFlowToGenPacket], t, this->nextsValuesOfGeneration[this->currFlowToGenPacket] - t);
}

void FelixServer::setNextFlow(std::shared_ptr<Flow> flow, double t, double sigmaValue){
	this->flow = flow;

	// Read parameters from flows assigned to this server (by name)
	this->startTime = this->flow->getStartTime();
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[setNextFlow] %s: StartTime: %f secs. \n",t, this->getFullName().data(), this->startTime);

	this->generationPeriod = this->flow->getPeriod();
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[setNextFlow] %s: generation period: %s \n", this->getFullName().data(), this->generationPeriod->getFullName().data());

	this->packetSize_bits = this->flow->getSize();
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[setNextFlow] %s: PacketSize: %s (bits) \n", this->getFullName().data(), this->packetSize_bits->getFullName().data());


	// Initialize
	this->sigma = sigmaValue;

	debugMsg(LOG_LEVEL_FULL_LOGGING, "[setNextFlow] %s: sigma=%f \n", this->getFullName().data(), this->sigma);
}

std::shared_ptr<NetworkPacket> FelixServer::createPacket(double t){
	// Create new packet
	this->counter++;
	auto packet = std::make_shared<NetworkPacket>(t);

	// create a protocol to include the packet size
	std::shared_ptr<IProtocol> appProtocol = std::make_shared<FelixAppProtocol>(this->getNextPacketSize_bits());
	packet->addProtocol(appProtocol);

	// create a routing protocol with the route determined by the flow
	std::shared_ptr<IProtocol> routeProtocol = std::make_shared<FlowProtocol>(this->flow->getFlowId());
	packet->addProtocol(routeProtocol);

	// create a QoS protocol with the priorities determined by the flow
	std::shared_ptr<IProtocol> qosProtocol = std::make_shared<QualityOfServiceProtocol>(this->flow->getTypeOfService()); // we need a copy of the route, as it will be modified along the way
	packet->addProtocol(qosProtocol);


	return packet;
}

int FelixServer::getNextPacketSize_bits(){
	// stochastic job weight
	int packetSize = this->packetSize_bits->nextValue();

	// TODO: temporarily to allow for exp and norm distributions (cero values are generated)
	if(packetSize <= 0){
		packetSize = 1;
	}

	// validate
	if(packetSize <= 0) {
		debugMsg(LOG_LEVEL_ERROR, "Wrong distribution for packetSize generated a 0 size packet.\n");
		throw std::runtime_error("Wrong distribution for packetSize generated a 0 size packet.");
	}

	return packetSize;
}
