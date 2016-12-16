//CPP:vector/ScalarIp2DcmVector.cpp
#if !defined ScalarIp2DcmVector_h
#define ScalarIp2DcmVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"


class ScalarIp2DcmVector: public Simulator {

	// Parameters
	int puPerDcm;

	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers


public:
	ScalarIp2DcmVector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
