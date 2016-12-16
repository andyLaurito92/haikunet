//CPP:hybrid/NetworkQueue.cpp
#if !defined NetworkQueue_h
#define NetworkQueue_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/ParameterReader.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "hybrid/stdevstool.h"

#include "NetworkData/NetworkPacket.h"
#include "NetworkData/Protocol/IProtocol.h"
#include "common/BaseSimulator.h"

#include <deque>


class NetworkQueue: public BaseSimulator {
	// parameters
	long maxCapacity_bytes;

	// State Variables
	std::deque<std::shared_ptr<NetworkPacket>> queuedPackets;

	// Counters
	int discardedPackets = 0; // Counter for logging
	int cantPacketsInQueue = 0; // Counter for performance
	unsigned long int queueSize_bytes = 0; // Counter for performance

	enum SERVERSTATE {SERVERWAITING,SERVERBUSY};
	SERVERSTATE serverstate;

public:
	NetworkQueue(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
private:
	void queuePacket(double t, std::shared_ptr<NetworkPacket> packetToQueue);
	void unqueuePacket(double t);
};

#endif
