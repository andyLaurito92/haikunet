//CPP:vector/VectorIp2TcpSenderVector.cpp
#if !defined VectorIp2TcpSenderVector_h
#define VectorIp2TcpSenderVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"
#include "L1TConfiguration.h"


class VectorIp2TcpSenderVector: public Simulator {

	// Parameters

	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers



public:
	VectorIp2TcpSenderVector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
