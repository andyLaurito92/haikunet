//CPP:vector/VectorDcm2DcmToRVector.cpp
#if !defined VectorDcm2DcmToRVector_h
#define VectorDcm2DcmToRVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"


class VectorDcm2DcmToRVector: public Simulator {

	// Parameters
	int dcmPerRack;

	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers
	


public:
	VectorDcm2DcmToRVector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
