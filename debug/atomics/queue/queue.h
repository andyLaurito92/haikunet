//CPP:queue/queue.cpp
#if !defined queue_h
#define queue_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"



class queue: public Simulator { 
// Declare the state,
// output variables
// and parameters
double Sigma;
int l_queue;
bool pending;
#define INF 1e20;


public:
	queue(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
