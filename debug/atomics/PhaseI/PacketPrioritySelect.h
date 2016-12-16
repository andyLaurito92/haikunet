//CPP:hybrid/PacketPrioritySelect.cpp
//CPP:PhaseI/NetworkData/Protocol/QualityOfServiceProtocol.cpp
#if !defined PacketPrioritySelect_h
#define PacketPrioritySelect_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/ParameterReader.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "vector/VectorialEvent.h"

#include "NetworkData/Protocol/QualityOfServiceProtocol.h"
#include "NetworkData/NetworkPacket.h"
#include "common/BaseSimulator.h"

/**
 * Receives packets, and based on their flowId it selects which output port to send it through
 */
class PacketPrioritySelect: public BaseSimulator {
	// parameters

	// State Variables
	std::shared_ptr<NetworkPacket> arrivedPacket;
	int priority; // priority of arrived packet

	// Counters


public:
	PacketPrioritySelect(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
private:

};

#endif
