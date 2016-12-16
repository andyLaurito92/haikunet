//CPP:cola_proc/tokenbucket.cpp
#if !defined tokenbucket_h
#define tokenbucket_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "C:/PowerDEVS/bin/gcc/include/math.h"


class tokenbucket: public Simulator { 
// Declare the state, output
// variables and parameters
double Tickets;
double sigma;

double elapsed;

double maxbucketsize;
double y[10];

#define INF 1e20






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
