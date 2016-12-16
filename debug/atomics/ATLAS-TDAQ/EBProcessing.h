//CPP:ATLAS-TDAQ/EBProcessing.cpp
#if !defined EBProcessing_h
#define EBProcessing_h

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
#include <memory>


class EBProcessing: public BaseSimulator {
	//parameters
	double processingPeriod; // meant to be used on the distribution for the processing time
	int stochasticSimulation;

	// state variables
	std::shared_ptr<TdaqPacket> packetBeingProcessed;
	
	// HELPERS
	

public:
	EBProcessing(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
