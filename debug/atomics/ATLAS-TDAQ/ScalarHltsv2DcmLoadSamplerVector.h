//CPP:vector/ScalarHltsv2DcmLoadSamplerVector.cpp
#if !defined ScalarHltsv2DcmLoadSamplerVector_h
#define ScalarHltsv2DcmLoadSamplerVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"
#include "NetworkConfiguration.h"


class ScalarHltsv2DcmLoadSamplerVector: public Simulator {
	double sigma;
	std::queue<std::pair<int, std::shared_ptr<VectorialEvent>>> outputVectorialEvents;

	// Helpers


public:
	ScalarHltsv2DcmLoadSamplerVector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
