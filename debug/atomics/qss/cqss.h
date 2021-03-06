//CPP:qss/qsstools.cpp

//CPP:qss/cqss.cpp
#if !defined cqss_h
#define cqss_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "math.h"
#include "string.h"
#include "qss/qsstools.h"


class cqss: public Simulator { 
// Declare here the state, output
// variables and parameters
double X[10],q[10];
double sigma;

double qs,qi;

//parameters
double dQ;
double dQmin,dQrel;


double eps,ep;
int band;

double y[10]; //output




















public:
	cqss(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
