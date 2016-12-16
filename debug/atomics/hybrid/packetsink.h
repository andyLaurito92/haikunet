//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packetsink.cpp
#if !defined packetsink_h
#define packetsink_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "sinks/logtoscilabtool.h"


class packetsink: public Simulator { 
// Declare the state,
// output variables
// and parameters



aMap myFields; 
aMapIter mapIt;
protocol myprotocol;

aProtocolList myprotoStack;
aProtocolListIter myprotoStackit;
aLoggerToScilab *logger;

Packet *p;

double sigma;
int counter;






public:
	packetsink(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
