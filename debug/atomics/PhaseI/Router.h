//CPP:PhaseI/Router.cpp
//CPP:PhaseI/NetworkData/NetworkPacket.cpp
//CPP:PhaseI/NetworkData/Protocol/RoutingProtocol.cpp
//CPP:PhaseI/NetworkData/GlobalFlowBasedRoutingTable.cpp
#if !defined Router_h
#define Router_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/ParameterReader.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "hybrid/stdevstool.h"

#include "PhaseI/NetworkData/NetworkPacket.h"
#include "PhaseI/NetworkData/Protocol/FlowProtocol.h"
#include "PhaseI/NetworkData/GlobalFlowBasedRoutingTable.h"
#include "common/BaseSimulator.h"

#include <deque>

/**
 * This model routes incoming packets to outports.
 * 	The decision is based on the global table GlobalFlowBasedRoutingTable(flowId, routerName). If there are more than one possible outport, the model uses a routingPolicy (currently hardcoded to Round-Robin) to decide.
 */
class Router: public BaseSimulator {
	struct outgoingPacket {
		int outgoingPort;
		std::shared_ptr<NetworkPacket> packet;
	};

	// Parameters
	std::queue<outgoingPacket> outputPackets; // use a queue to allow for multiple packets to arrive at the same time

	// state variables
	std::map<std::string, int> lastOutputPortIndexPerFlow; // Map<flowId, lasOutPort> --> this is needed to implement the Round robin policy. TODO: take it outside of this class in the future

	public:
		Router(const char *n): BaseSimulator(n) {};
		void init(double, ...);
		void dint(double);
		void dext(Event , double );
		Event lambda(double);
	private:
		int applyLoadBalancingPolicy(std::string flowId, std::vector<int> possibleOutPorts);
};
#endif
