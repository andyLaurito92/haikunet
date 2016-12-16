//CPP:vector/VectorRos2ToRIndex.cpp
#if !defined VectorRos2ToRIndex_h
#define VectorRos2ToRIndex_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"
#include "L1TConfiguration.h"


class VectorRos2ToRIndex: public Simulator {

	// Parameters


	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers
	


public:
	VectorRos2ToRIndex(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
