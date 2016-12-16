//CPP:ATLAS-TDAQ/PacketNetworkServer.cpp
//CPP:hybrid/stdevstool.cpp
#if !defined packetNetworkServer_h
#define packetNetworkServer_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "hybrid/stdevstool.h"
#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"
#include "hybrid/stdevstool.h"
#include "common/BaseSimulator.h"


class PacketNetworkServer: public BaseSimulator {
	// Parameters
	double bandwidth_bits_s;
	double bandwidth_bits_s_r;
	double minTransitDelay_ms;
	double minTransitDelay_s;
	int stochasticSimulation;

	enum STATE { REQUESTING, WAITING_NEXT_JOB, PROCESSING};
	STATE mystate;

	long counter;
	double lastEmitionTime;
	std::shared_ptr<TdaqPacket> incomingPacket;

	// Helpers
	

	public:
		PacketNetworkServer(const char *n): BaseSimulator(n) {};
		void init(double, ...);
		void dint(double);
		void dext(Event , double );
		Event lambda(double);
	private:

};
#endif
