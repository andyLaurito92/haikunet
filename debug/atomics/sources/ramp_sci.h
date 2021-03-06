//CPP:source/ramp_sci.cpp
#if !defined ramp_sci_h
#define ramp_sci_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "string.h"


class ramp_sci: public Simulator { 
// Declare here the state
// variables and parameters
double sigma;
int j; //states
double y[10]; //output
double t0,tr,u;
double T[4],v[3],mv[4],inf;
double dq;
char* Method;



public:
	ramp_sci(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
