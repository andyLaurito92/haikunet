//CPP:ATLAS-TDAQ/EBRequester.cpp
#if !defined EBRequester_h
#define EBRequester_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "PacketSender.h"
#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "tdaqPacketTool.h"
#include "hybrid/stdevstool.h"
#include "ATLAS-TDAQ/L1TConfiguration.h"
#include <algorithm>

class EBRequester: public BaseSimulator {
	// parameters
	int stochasticSimulation;

	// state variables
	enum STATE { REQUESTING, WAITING };
	STATE mystate;
	std::shared_ptr<FragmentRequestInfo> pendingInfoToRequest;

	// counters
	int totalSentRequests;
	int totalEventsRequested;

	// helper objects
	

public:
	EBRequester(const char *n) : BaseSimulator(n) { };
	~EBRequester() { };
	virtual void init(double, ...);
	virtual void dint(double);
	virtual void dext(Event, double);
	virtual Event lambda(double);
};
#endif
