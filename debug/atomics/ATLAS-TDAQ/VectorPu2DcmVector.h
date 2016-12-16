//CPP:vector/VectorPu2DcmVector.cpp
#if !defined VectorPu2DcmVector_h
#define VectorPu2DcmVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"
#include "NetworkConfiguration.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"


class VectorPu2DcmVector: public Simulator {

	// Parameters


	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers


public:
	VectorPu2DcmVector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
