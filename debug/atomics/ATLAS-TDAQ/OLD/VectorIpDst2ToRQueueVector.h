//CPP:vector/VectorIpDst2ToRQueueVector.cpp
#if !defined VectorIpDst2ToRQueueVector_h
#define VectorIpDst2ToRQueueVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"
#include "NetworkConfiguration.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"


class VectorIpDst2ToRQueueVector: public Simulator {
	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers

public:
	VectorIpDst2ToRQueueVector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
