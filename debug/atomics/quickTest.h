//CPP:quickTest.cpp
#if !defined quickTest_h
#define quickTest_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"



class quickTest: public Simulator { 
// Declare the state,
// output variables
// and parameters

public:
	quickTest(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
