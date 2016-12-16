//CPP:continuous/testsink.cpp
#if !defined testsink_h
#define testsink_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "memory"
#include "continuous/test.h"
#include "vector"


class testsink: public Simulator { 

typedef std::shared_ptr<A> myPtr;
std::shared_ptr<A> o;


std::vector<myPtr> vector;

public:
	testsink(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
