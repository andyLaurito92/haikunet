//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/packetsink.cpp
//CPP:ATLAS-TDAQ/OpenTsdb/OpenTsdbLogger.cpp
//CPP:ATLAS-TDAQ/OpenTsdb/TelnetOpenTsdbClient.cpp
//CPP:ATLAS-TDAQ/tdaqPacketTool.cpp
//CPP:ATLAS-TDAQ/globals.cpp
#if !defined packetsink_h
#define packetsink_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "ATLAS-TDAQ/tdaqPacketTool.h"
#include "hybrid/packettool.h"
#include "NetworkConfiguration.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"

class packetsink: public BaseSimulator {

// Parameters


// State variables
int counter;

// Helpers


public:
	packetsink(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
