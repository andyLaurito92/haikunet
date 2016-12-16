//CPP:queue/server.cpp
#if !defined server_h
#define server_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"



class server: public Simulator { 
// Declare the state,
// output variables
// and parameters
double Sigma;
int l_server = 1;
int processed;
#define INF 1e20;


public:
	server(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
