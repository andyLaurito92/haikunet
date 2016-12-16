//CPP:vector/ScalarHltsvReq2Vector.cpp
#if !defined ScalarHltsvReq2Vector_h
#define ScalarHltsvReq2Vector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"


class ScalarHltsvReq2Vector: public Simulator {
	double sigma;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers


public:
	ScalarHltsvReq2Vector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
