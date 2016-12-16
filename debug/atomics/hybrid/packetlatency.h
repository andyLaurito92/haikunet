//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packetlatency.cpp
#if !defined packetlatency_h
#define packetlatency_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "sinks/logtoscilabtool.h"


class packetlatency: public Simulator { 
// Declare the state,
// output variables
// and parameters

#define inf 1E10

Packet *p;
Packet *pout;
aPacketQueue myPacketChannel;
aLoggerToScilab *myLoggerToScilab;

double sigma;

double nextSigma;
double propTime;
double latency;

double arrived;
double departed;
double dropped;

enum STATE {IDLE,SENDING};
STATE state;
STATE nextState;

char* LayerFunction;




public:
	packetlatency(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
