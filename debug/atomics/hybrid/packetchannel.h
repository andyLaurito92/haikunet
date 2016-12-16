//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp
//CPP:hybrid/packetchannel.cpp
#if !defined packetchannel_h
#define packetchannel_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "sinks/logtoscilabtool.h"


class packetchannel: public Simulator { 
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
char*  overflowAction;
double BWDelayProduct;
double latency;
double bandwidth;
double tAllowNewPacket;
int currentInFlightSize;
bool enableLogSignals;

double arrived;
double departed;
double dropped;

enum STATE {IDLE,SENDING};
STATE state;
STATE nextState;

char* LayerFunction;


public:
	packetchannel(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
