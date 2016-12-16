//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/packetdemux.cpp
#if !defined packetdemux_h
#define packetdemux_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"
#include "hybrid/packettool.h"

class packetdemux: public Simulator { 

	//parameters
	std::string Labels[10];
	int n;

	double sigma;
	
	std::queue<std::shared_ptr<TdaqPacket>> packetBuffer;

	// Helpers
	std::shared_ptr<IPowerDEVSLogger> logger;

	public:
		packetdemux(const char *n): Simulator(n) {};
		void init(double, ...);
		double ta(double t);
		void dint(double);
		void dext(Event , double );
		Event lambda(double);
		void exit();
};
#endif
