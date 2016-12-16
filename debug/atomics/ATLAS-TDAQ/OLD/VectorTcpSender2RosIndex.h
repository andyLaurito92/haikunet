//CPP:vector/VectorTcpSender2RosIndex.cpp
#if !defined VectorTcpSender2RosIndex_h
#define VectorTcpSender2RosIndex_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"
#include "L1TConfiguration.h"

// TODO: this model is not used any more
class VectorTcpSender2RosIndex: public Simulator {

	// Parameters


	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers
	


public:
	VectorTcpSender2RosIndex(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
