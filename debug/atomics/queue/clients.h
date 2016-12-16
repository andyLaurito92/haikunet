//CPP:queue/clients.cpp
#if !defined clients_h
#define clients_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"



class clients: public Simulator { 
// Declare the state,
// output variables
// and parameters
double time;
double Sigma;


public:
	clients(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
