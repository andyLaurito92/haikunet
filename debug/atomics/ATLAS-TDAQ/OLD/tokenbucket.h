//CPP:cola_proc/tokenbucket.cpp
#if !defined tokenbucket_h
#define tokenbucket_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "math.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"

// #include "C:/PowerDEVS/bin/gcc/include/math.h"

/*!
  - Accepts requests on inport 0 and forwards them on outport 0 if there are available tickets
  - Accepts new tickets on inport 1
  - Ouputs requests on outport 0

  - Delays requests until there are available credits

NOTE: the actual value of the incoming signals is not taken into account
 */
class tokenbucket: public Simulator { 
// Declare the state, output
// variables and parameters

int waitingPackets; // amount of packets waiting to be sent (when there are not enough tickets)
int Tickets; // available tickets
double sigma;

double elapsed;

int maxbucketsize;
double y[10];

std::shared_ptr<IPowerDEVSLogger> logger;


public:
	tokenbucket(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
