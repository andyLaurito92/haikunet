//CPP:PhaseI/FelixServer.cpp
//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/stdevstool.cpp
//CPP:PhaseI/NetworkData/NetworkPacket.cpp
//CPP:PhaseI/NetworkData/Protocol/FelixAppProtocol.cpp
//CPP:PhaseI/NetworkData/Protocol/RoutingProtocol.cpp
//CPP:PhaseI/NetworkData/Protocol/FlowProtocol.cpp
//CPP:PhaseI/NetworkData/Route.cpp
//CPP:PhaseI/NetworkData/GlobalFlowBasedRoutingTable.cpp
//CPP:PhaseI/FlowDefinitions.cpp


#ifndef FelixServer_h
#define FelixServer_h


#include "simulator.h"
#include "event.h"
#include "stdarg.h"


#include "sinks/ParameterReader.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "hybrid/stdevstool.h"

#include "NetworkData/NetworkPacket.h"
#include "NetworkData/Protocol/FelixAppProtocol.h"
#include "NetworkData/Protocol/FlowProtocol.h"
#include "NetworkData/Protocol/QualityOfServiceProtocol.h"
#include "NetworkData/Route.h"
#include "FlowDefinitions.h"


#include "common/BaseSimulator.h"


class FelixServer: public BaseSimulator {

	// Parameters
	double startTime = -1;
	std::shared_ptr<Flow> flow;

	std::shared_ptr<IDistributionParameter> generationPeriod;
	std::shared_ptr<IDistributionParameter> packetSize_bits;

	// State variables
	int counter = 0;

public:
	FelixServer(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
private:
	std::vector<std::shared_ptr<Flow>> allmyFlows;
	std::vector<double> nextsValuesOfGeneration;
	int currFlowToGenPacket;
	int getNextPacketSize_bits();
	void SetStateForNextFlowToGeneratePacket(double);
	void setNextFlow(std::shared_ptr<Flow> flow, double t, double sigmaValue);

	std::shared_ptr<NetworkPacket> createPacket(double t);
};
#endif
