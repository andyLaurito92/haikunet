//CPP:ATLAS-TDAQ/HLTSupervisor.cpp
//CPP:hybrid/stdevstool.cpp
//CPP:sinks/logtoscilabtool.cpp
//CPP:ATLAS-TDAQ/tdaqPacketTool.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/HltsvDcmLoad.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/RandomHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/FifoHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/RandomFifoHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/LeastBusyDcmHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/StochasticPriorityHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/RandomRoundRobinHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/DcmMeasuredStatistics.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/DcmRunStatistics.cpp


#if !defined HLTSupervisor_h
#define HLTSupervisor_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "tdaqPacketTool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "hybrid/packettool.h"
#include "ATLAS-TDAQ/globals.h"
#include "hybrid/stdevstool.h"


#include "ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.h"
#include "ATLAS-TDAQ/HltsvStrategies/RandomHltsvStrategy.h"
#include "ATLAS-TDAQ/HltsvStrategies/FifoHltsvStrategy.h"
#include "ATLAS-TDAQ/HltsvStrategies/RandomFifoHltsvStrategy.h"
#include "ATLAS-TDAQ/HltsvStrategies/LeastBusyDcmHltsvStrategy.h"
#include "ATLAS-TDAQ/HltsvStrategies/StochasticPriorityHltsvStrategy.h"

#include "ATLAS-TDAQ/globals.h"
#include "NetworkConfiguration.h"
#include <algorithm>
#include <vector>
#include <queue>
#include <memory>

class HLTSupervisor: public BaseSimulator {

	// Global accessible mapping of eventIds to PUs. Used to do the "routing" to adequate DCMs and PUs.
	//static std::multimap<int, int> SourceToRobIds;

	// Parameters
	double rate;
	int totalPuCount;
	int stochasticSimulation;
	enum ASSIGMET_ALGORITHM { FIFO, RANDOM, RANDOM_FIFO, LESS_LOADED_DCM, MIN_LATENCY_DCM, NUM_TYPES };
	ASSIGMET_ALGORITHM assignmentAlgorithm;
	double dcmSubsetRatio;
	int metric;
	int priorityQueueSelectionType;

	// state variables
	std::shared_ptr<IHltsvStrategy> strategy;

	double previousSigma;
	int nextEventId;
	int queuedEventsToSend; // number of events that should have been sent but there were no available idle PUs

	enum STATE { WAITING_RATE, SENDING_TO_IDLE_PU };
	STATE mystate;

	// HELPERS


public:
	HLTSupervisor(const char *n) : BaseSimulator(n) {	}	;
	void init(double, ...);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit(double t);
private:
	std::shared_ptr<IHltsvStrategy>  createStrategy(ASSIGMET_ALGORITHM assignmentAlgorithm);
	double getNextSigma();
};
#endif
