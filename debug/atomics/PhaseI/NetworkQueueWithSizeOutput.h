//CPP:hybrid/NetworkQueueWithSizeOutput.cpp
#if !defined NetworkQueueWithSizeOutput_h
#define NetworkQueueWithSizeOutput_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "vector/ScalarSimulator.h"

#include "sinks/ParameterReader.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "hybrid/stdevstool.h"

#include "NetworkData/NetworkPacket.h"
#include "NetworkData/Protocol/IProtocol.h"
#include "common/BaseSimulator.h"

class NetworkQueueWithSizeOutput: public BaseSimulator, ScalarSimulator {
	// parameters
	unsigned long int maxCapacity_bytes;
	double forcedPeriod;

	// State Variables
	std::deque<std::shared_ptr<NetworkPacket>> packetQueue;

	bool pendingRequest;
	bool pendingLengthNotify;
	double sizeout[10];

	// Counters
	int discardedPackets; // Counter for logging
	int cantPacketsInQueue; // Counter for performance
	unsigned long int queueSize_bytes; // Counter for performance


	enum STATE {WAITING,t_NOTIFYING,t_READYTOEMIT};
	STATE mystate;
	enum SERVERSTATE {SERVERWAITING,SERVERBUSY};
	SERVERSTATE serverstate;

	// Helpers


public:
	NetworkQueueWithSizeOutput(const char *n): BaseSimulator(n), ScalarSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
private:
	void queuePacket(std::shared_ptr<NetworkPacket> packetToQueue);
	void unqueuePacket();
};

#endif
