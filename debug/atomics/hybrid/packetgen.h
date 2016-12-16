//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/packetgen.cpp
#if !defined packetgen_h
#define packetgen_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/logtoscilabtool.h"
#include "hybrid/packettool.h"
#include "hybrid/stdevstool.h"


class packetgen: public Simulator { 
// Declare the state,
// output variables
// and parameters

aMap myFields; 
aMapIter mapIt;
protocol myprotocol;

std::list<std::shared_ptr<protocol>> myprotoStack;
std::list<std::shared_ptr<protocol>>::iterator myprotoStackit;

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
	packetgen(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
