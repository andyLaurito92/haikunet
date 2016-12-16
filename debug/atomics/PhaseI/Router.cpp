#include "Router.h"

void Router::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

//	char* fvar;
//	fvar = va_arg(parameters,char*);
//	this->capacity_bit_s_r = readDefaultParameterValue<double>(fvar);
//	debugMsg(LOG_LEVEL_INIT, "[INIT] %s: capacity_bit_s: %f bits/s. \n", this->getFullName(), this->capacity_bit_s_r);
//	this->capacity_bit_s_r = 1 / this->capacity_bit_s_r; // revert it to avoid continuous divisions later
//

	this->sigma = std::numeric_limits<double>::infinity(); // Immediately asks for a new packet


//	this->logger->initSignals({
//		{"sent_bits", SCILAB_LOG_LEVEL_ALWAYS},
//		{"sent_packets", SCILAB_LOG_LEVEL_PRIORITY},
//		{"processingTime", SCILAB_LOG_LEVEL_DEBUG},
//	});
}

void Router::dint(double t) {
	this->sigma = std::numeric_limits<double>::infinity();
	if(this->outputPackets.size() != 0){
		this->sigma = 0;
	}
}

void Router::dext(Event x, double t) {

	// Packet arrived
	auto incomingPacket = castEventPointer<NetworkPacket>(x); // get the packet from the incoming DEVSEvent

	// Get the route (from the protocol)
	auto protocol = incomingPacket->getProtocol(PROTOCOL_ID_FLOW);
	if(!protocol){
		debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u without routing protocol. Discarding (this is normal ONLY at the beginning of simulation as a fake packet is sent by links) \n ", t, this->getFullName().data(), incomingPacket->getId());
		return;
	}
	std::string flowId = std::static_pointer_cast<FlowProtocol>(protocol)->getFlowId();

	// get outport from the routing table
	auto possibleOutPorts = GlobalFlowBasedRoutingTable::getOutPorts(flowId, this->getFullName());

	// validate that an output port exists
	if(possibleOutPorts.size() == 0){
		debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: received packet #%u with flow '%s' which has not configured output port in this device (see GlobalFlowBasedRoutingTable). \n ", t, this->getFullName().data(), incomingPacket->getId(), flowId.data());
		throw std::runtime_error("there is no configured output for this device. See log for details.");
	}

	// Decide which out port to use for forwarding based on the route
	int outPort = this->applyLoadBalancingPolicy(flowId, possibleOutPorts);

	// set to forward packet immediately
	this->outputPackets.push({ outPort, incomingPacket});
	this->sigma = 0;

	debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_ext: received packet #%u from inPort %u. Forwarding through port %u. \n ", t, this->getFullName().data(), incomingPacket->getId(), x.port, outPort);
}

Event Router::lambda(double t) {
	auto outEvent = this->outputPackets.front();
	this->outputPackets.pop();

	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding packet #1 through port %u \n", t, this->getFullName().data(), outEvent.packet->getId(), outEvent.outgoingPort);
	return Event(outEvent.packet,outEvent.outgoingPort);
}

// TODO: implement load balancing policies outside of this class => Currently Round Robin
int Router::applyLoadBalancingPolicy(std::string flowId, std::vector<int> possibleOutPorts){
	uint nextIndex = 0;
	auto it = this->lastOutputPortIndexPerFlow.find(flowId);
	if(it != this->lastOutputPortIndexPerFlow.end()){
		nextIndex = it->second;
		nextIndex++;

		// restart the round
		if(nextIndex >= possibleOutPorts.size()){
			nextIndex = 0;
		}
	}

	// set last used value
	this->lastOutputPortIndexPerFlow[flowId] = nextIndex;

	return possibleOutPorts[nextIndex];
}

