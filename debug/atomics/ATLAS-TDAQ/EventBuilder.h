//CPP:ATLAS-TDAQ/EventBuilder.cpp
#if !defined EventBuilder_h
#define EventBuilder_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "tdaqPacketTool.h"
#include "hybrid/stdevstool.h"
#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iterator>

/*!
 * Represents the Event Builder System (or SFI, Sub-Farm Input)
 - Inport0: Accepts packets with fragmentInfo from ROS (to build the event)
 - Outport0: Outputs the built EventInfo

Behaviour:
- Accepts cantRequests requests (Inport)
- After all requests arrived the event is forwarded

 TODO:
 Outport0: Are these bulk requests? To which ROB&ROS (from paper: EB gather the scattered event data from all ROSs)? --> Need more info
 */
class EventBuilder: public BaseSimulator {

	// Parameters
	int cantRequestsPerEvent; // amount of requests to be sent per each incomming event

	// State variables
	FragmentRequestInfo waitingInfo = FragmentRequestInfo(-1, -1, std::vector<int>(), -1, -1); // #robids numbers the model is waiting to be processed
	std::shared_ptr<EventInfo> receivedInfo; // fragments already received

	enum STATE { WAITING, BUILDING_EVENT };
	STATE myState;
	int counter;
	double eventBirthTime;

public:
	EventBuilder(const char *n) : BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
};
#endif
