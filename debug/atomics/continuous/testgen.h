//CPP:continuous/testgen.cpp
#if !defined testgen_h
#define testgen_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "memory"
#include "continuous/test.h"


class testgen: public Simulator { 
// Declare the state,
// output variables
// and parameters
std::shared_ptr<A> p;
public:
	testgen(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
