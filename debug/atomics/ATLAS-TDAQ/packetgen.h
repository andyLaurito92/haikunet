//CPP:ATLAS-TDAQ/packetgen.cpp
//CPP:sinks/logtoscilabtool.cpp
//CPP:ATLAS-TDAQ/tdaqPacketTool.cpp
#if !defined packetgen_h
#define packetgen_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "tdaqPacketTool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "hybrid/packettool.h"
#include "hybrid/stdevstool.h"
#include "VectorialEvent.h"

#define HLTSV_REQUEST_PACKET 0
#define FRAGMENT_PACKET 1
class packetgen: public Simulator { 
// Declare the state,
// output variables
// and parameters

aMap myFields; 
aMapIter mapIt;
protocol myprotocol;

aProtocolList myprotoStack;



std::shared_ptr<IPowerDEVSLogger> logger;

double sigma;
int counter;
char *FlowID;
double Period;
double Psize;
double StartTime;
double packetType;

int stochasticSimulation;

public:
	packetgen(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
