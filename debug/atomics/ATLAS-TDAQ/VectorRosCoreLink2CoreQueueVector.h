//CPP:vector/VectorRosCoreLink2CoreQueueVector.cpp
#if !defined VectorRosCoreLink2CoreQueueVector_h
#define VectorRosCoreLink2CoreQueueVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"
#include "NetworkConfiguration.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"


class VectorRosCoreLink2CoreQueueVector: public Simulator {

	// Parameters
	int nCoreSwitches;
	int nDcmToR;

	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers
	


public:
	VectorRosCoreLink2CoreQueueVector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
