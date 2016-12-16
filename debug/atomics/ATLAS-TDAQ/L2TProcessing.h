//CPP:ATLAS-TDAQ/L2TProcessing.cpp
#if !defined L2TProcessing_h
#define L2TProcessing_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "tdaqPacketTool.h"
#include "hybrid/stdevstool.h"
#include "hybrid/packettool.h"
#include "tdaqPacketTool.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iterator>


/*!
- Inport0: Accepts packets with request to the 1st level trigger, to know which packets to wait for.
- Inport1: Accepts packets from the 1st level trigger with the requested information
- Outport0: Sends event information of accepted events to the Event Builder.

Behaviour:
1)When the first response arrives (Inport0), the PU processes for a certain time (given by a ditribution with mean processingPeriod).
 2) For each response that arrives a certain time is added to the processing time (given by a ditribution with mean processingPeriod).
 3) Once ALL responses arrived and the processingTime has elapsed, the an event sent (Outport0)
 4) If 300ms elapsed and PU is still processing, the processing is interrupted and the event is sent (Outport0)
 */
class L2TProcessing: public BaseSimulator {
// Declare the state,
// output variables
// and parameters
	//parameters
	double processingPeriod; // meant to be used on the distribution for the processing time
	int cantRequestsPerEvent;
	bool useIncrementalProcessingTime;
	int stochasticProcessingTimes;

	// state variables
	FragmentRequestInfo waitingInfo = FragmentRequestInfo(-1, -1, std::vector<int>(),-1, 1); // robIds numbers the model is waiting to be processed
	std::vector<FragmentInfo> receivedInfo ;   // event info already received
	int puId = -1;
	int eventId = -1;
	int currentStep = 1; // used to increment processing time according to the step (if useIncrementalProcessingTime=1)

	enum STATE { WAITING_NEXT_JOB=0, PROCESSING=1, WAITING_RESPONSE=2};
	STATE myState;
	int sentEvents;
	double eventBirthTime; // time when the event started to be built

	// HELPERS


public:
	L2TProcessing(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);

private:
	std::shared_ptr<TdaqPacket> createEventPacket(double t, double procTime);
	double getNextProccessingTime(double t);
};
#endif
