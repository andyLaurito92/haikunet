//CPP:ATLAS-TDAQ//RunScilabJob.cpp
#if !defined ATLAS_TDAQ_RunScilabJob_h
#define ATLAS_TDAQ_RunScilabJob_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "string.h"

#include "ATLAS-TDAQ/globals.h"

/**
 * Updated the default PD runscilabjob in order to take into account the logging backend parameter (do nothing if != to scilab)
 */
class RunScilabJob: public Simulator {
// Declare the state,
// output variables
// and parameters
double sigma;
char *atInit;
char *atDExt;
char *atExit;

public:
	RunScilabJob(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
private:
	bool isScilabConfigured();
};
#endif
