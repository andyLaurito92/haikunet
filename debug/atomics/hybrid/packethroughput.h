//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packethroughput.cpp
#if !defined packethroughput_h
#define packethroughput_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/logtoscilabtool.h"


class packethroughput: public Simulator { 
// Declare the state,
// output variables
// and parameters

#define inf 1E10

double out_rate[10];
double lastcount ;
double runningtime ;
double rate ;
double slope_rate;

double sigma;





public:
	packethroughput(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
