//CPP:ATLAS-TDAQ/L2TRequester.cpp
#if !defined PUEventRequester_h
#define PUEventRequester_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "PacketSender.h"

#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "sinks/ParameterReader.h"
#include "hybrid/stdevstool.h"
#include "common/BaseSimulator.h"

#include "tdaqPacketTool.h"
#include "ATLAS-TDAQ/L1TConfiguration.h"
#include <algorithm>
#include <vector>
#include <set>
#include <tuple>
#include <ATLAS-TDAQ/NetworkConfiguration.h>

class L2TRequester: public BaseSimulator {

public:
	// parameters
	int stochasticSimulation;
	int l2Requests;        // How many fragments to request in total
	int requestsPerSource; // how many fragments to request to each ROS

	// state variables
	enum STATE { REQUESTING, WAITING };
	STATE mystate;
    int eventId;
    int puId;
    std::set<int> requestedRobIds;

	std::shared_ptr<FragmentRequestInfo> pendingRequest;

	// counters
	int totalSentRequests;
	int totalEventsRequested;



public:
	L2TRequester(const char *n) : BaseSimulator(n) { };
	~L2TRequester() { };
	virtual void init(double, ...);
	virtual void dint(double);
	virtual void dext(Event, double);
	virtual Event lambda(double);

private:
	template<typename ForwardIterator>
	ForwardIterator random_shuffle_n(ForwardIterator begin, ForwardIterator end, std::size_t num_random);


};
#endif
