//CPP:vector/ScalarIp2TcpSender.cpp
#if !defined ScalarIp2TcpSender_h
#define ScalarIp2TcpSender_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"
#include "L1TConfiguration.h"


class ScalarIp2TcpSender: public Simulator {

	// Parameters


	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;


	// Helpers
	

public:
	ScalarIp2TcpSender(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
