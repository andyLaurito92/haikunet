//CPP:vector/ScalarIp2DcmToRVector.cpp
#if !defined ScalarIp2DcmToRVector_h
#define ScalarIp2DcmToRVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"


class ScalarIp2DcmToRVector: public Simulator {

	// Parameters
	int puPerDcm;
	int dcmPerRack;

	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers
		

public:
	ScalarIp2DcmToRVector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
