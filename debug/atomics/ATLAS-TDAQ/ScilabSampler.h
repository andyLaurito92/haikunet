//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSampler.cpp
//CPP:ATLAS-TDAQ/OpenTsdb/OpenTsdbLogger.cpp
//CPP:ATLAS-TDAQ/OpenTsdb/TelnetOpenTsdbClient.cpp
#if !defined ScilabSampler_h
#define ScilabSampler_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <algorithm>
#include <limits>
#include <memory>

#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "common/BaseSimulator.h"
#include "tdaqPacketTool.h"


class ScilabSampler: public BaseSimulator {
	// parameters
	double samplePeriod;

	// State Variables
	double lastT;

	uint lastQueueSize;
	double weightedSumCounter;
	uint maxCounter;
	uint minCounter;
	uint eventCounter;
	uint discardsCounter;
	uint sentBytesCounter;

public:
	ScilabSampler(const char *n) : BaseSimulator(n) {	};
	void init(double, ...);

	void dint(double);
	void dext(Event, double);
	Event lambda(double);

private:
	void resetCounters();
};
#endif
