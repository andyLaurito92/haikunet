//CPP:qss/qsstools.cpp
//CPP:qss/liqss3.cpp
#if !defined liqss3_h
#define liqss3_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "math.h"
#include "string.h"
#include "qss/qsstools.h"


class liqss3: public Simulator { 
// Declare here the state, output
// variables and parameters
double X[10],q[10],dq,q_old,dq_old,dx_old;
double a,u[10],sigma;


//parameters
double dQ;
double dQmin,dQrel;
double y[10]; //output
bool band,band2,band3,band4;





































































public:
	liqss3(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
