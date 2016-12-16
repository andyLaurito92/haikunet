//CPP:ATLAS-TDAQ/DcmLoadSampler.cpp
//CPP:sinks/logtoscilabtool.cpp
#if !defined DcmLoadSampler_h
#define DcmLoadSampler_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "tdaqPacketTool.h"

#include <algorithm>
#include <limits>
#include <memory>

class DcmLoadSampler: public BaseSimulator {
	// parameters
	double samplePeriod;

	// State Variables
	double lastT;

	// Counters
	uint busyPus;
	uint processedEvents;
        uint assignedEvents;

	double weightedSumCounter;
	uint maxCounter;
	uint minCounter;
	uint eventCounter;

	// Helpers


public:
	DcmLoadSampler(const char *n) : BaseSimulator(n) {	};
	void init(double, ...);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
private:
	void resentCounters();
};
#endif
