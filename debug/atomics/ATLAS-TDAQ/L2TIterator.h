//CPP:ATLAS-TDAQ/L2TIterator.cpp
#if !defined L2TIterator_h
#define L2TIterator_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include <vector>

#include "hybrid/stdevstool.h"
#include "common/BaseSimulator.h"
#include "ATLAS-TDAQ/tdaqPacketTool.h"
#include "ATLAS-TDAQ/L1TConfiguration.h"


class L2TIterator: public BaseSimulator {
	// Declare the state,
	// output variables
	// and parameters

	// Parameters
	double meanSteps;
	int stochasticSteps = 0;

	// State Variables
	int steps;
	std::queue<std::shared_ptr<void>> arrivedSignals;
	bool newEvent = true;
	float forwardingPercentageThreshold;
	//Actual Event
	std::shared_ptr<EventInfo> eventInfo;

	//counters
	int accepted;
	int rejected;
	int arrived;

	// helpers

	enum STATE {SENDING, WAITING};
	STATE mystate;
public:
	L2TIterator(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);

	int getNextStepsAmount();
	bool fragmentsThresholdCalculate();
};
#endif
