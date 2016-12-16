//CPP:ATLAS-TDAQ/packetDiscard.cpp
//CPP:hybrid/stdevstool.cpp
#if !defined packetDiscard_h
#define packetDiscard_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "hybrid/stdevstool.h"
#include "common/BaseSimulator.h"


class packetDiscard: public BaseSimulator {
// Declare the state,
// output variables
// and parameters

	// Parameters
	double discardRate;
	double interleaveDelay;

	// State Variables
	std::queue<std::shared_ptr<void>> arrivedSignals;

	//counters
	int accepted;
	int rejected;
	int arrived;

	enum STATE {SENDING, WAITING};
	STATE mystate;

public:
	packetDiscard(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
