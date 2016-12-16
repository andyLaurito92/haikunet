//CPP:sinks/logtoscilabtool.cpp
//CPP:PhaseI/IValueSampler.cpp
#if !defined IValueSampler_h
#define IValueSampler_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/ParameterReader.h"
#include "BaseSimulator.h"

#include <algorithm> // max, min

/**
 * This model should not be necesary any more. Use the SampleLogger which directly samples without the need of an atomic model.
 */
class IValueSampler: public BaseSimulator {
protected:
	// parameters
	double samplePeriod = 0;

	// State Variables
//	double sigma = INF;
	double lastT = -1;
	double lastSampleT = 0; // records the last time a sample was logged (so that last log can be weighted accuratly)

	double weightedSumCounter = 0;
	double lastValue = 0;
	double maxCounter  = 0;
	double minCounter  = 0;
	double sumCounter  = 0;
	uint eventCounter  = 0;

	// Helpers
	std::shared_ptr<IPowerDEVSLogger> logger;
public:
	IValueSampler(const char *n) : BaseSimulator(n) {	};
	void init(double, ...);
	void init(double, va_list args);
//	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit(double t);
protected:
	virtual double getValue(Event x) =0;

private:

	void resetCounters();
};
#endif
