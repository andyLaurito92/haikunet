//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp
//CPP:hybrid/dummy.cpp
#if !defined dummy_h
#define dummy_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "stdevstool.h"
#include "sinks/logtoscilabtool.h"


class dummy: public Simulator { 
// Declare the state,
// output variables
// and parameters

aMap myFields; 
aMapIter mapIt;
protocol myprotocol;

aProtocolList myprotoStack;
aProtocolListIter myprotoStackit;

STDEVS *mySTDEVS;
Packet *p;
aLoggerToScilab *myLoggerToScilab;

double sigma;
double counter;
char *FlowID;
double Period;
double Psize;
double StartTime;



public:
	dummy(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
