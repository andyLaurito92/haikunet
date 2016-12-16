//CPP:hybrid/PacketQueue.cpp
//CPP:sinks/logtoscilabtool.cpp
//CPP:ATLAS-TDAQ/OpenTsdb/OpenTsdbLogger.cpp
#if !defined PacketQueue_h
#define PacketQueue_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"

#include "tdaqPacketTool.h"

class PacketQueue: public BaseSimulator {
	// parameters
	int MaxCapacity_bytes;
	double ForcedPeriod;

	// State Variables
	std::deque<std::shared_ptr<TdaqPacket>> packetQueue;

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
	PacketQueue(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
private:
	void queuePacket(std::shared_ptr<TdaqPacket> packetToQueue);
	void unqueuePacket();
};

#endif
