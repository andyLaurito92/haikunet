//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packetmux.cpp
#if !defined packetmux_h
#define packetmux_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "sinks/logtoscilabtool.h"


class packetmux: public Simulator { 
// Declare the state,
// output variables
// and parameters

double sigma; 

#define INF 1e20

Packet *p;

aLoggerToScilab *myLoggerToScilab;

#define INF 1e20













public:
	packetmux(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
