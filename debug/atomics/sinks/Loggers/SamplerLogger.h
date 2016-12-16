#if !defined SamplerLogger_h
#define SamplerLogger_h

#include "sink/ParameterReader.h"
#include "sink/Loggers/IPowerDEVSLogger.h"

#include <limits>

/**
 *  The sampler receives values every time a variable is logged (it changes), but logs only every a fixed time period (ej: 1s). When this period elapses the sampler logs the following metrics
    - max: maximum value received during the period
    - min: minimum value received during the period
    - sum: sum of values received during the period
    - avg: average of received values (sum/nValues)
    - timeAvg: time weighted average of the received values
       I.E: timeAvg=SUM_1_N( valueN * etN) / samplingPeriod, where value1.. valueN are the received values, and et1 ..et N are the elapsed time where the valueN was observed.

 For all previous metrics, the Sampler only records one value per timestamp. I.E: 2 values for the same simulated time, it only records the last one

    - evCount: count of calls to logSignal, even it they are in the same timestamp
 */
class SamplerLogger : public IPowerDEVSLogger {

#define SAMPLER_PERIOD_VARIABLE_NAME "sample_period"


private:
	// parameters
	double samplePeriod = 0;

	// State Variables
	double lastT = -1;
//	double lastSampleT = 0; // records the last time a sample was logged (so that last log can be weighted accurately)
	double nextSampleT = -1;

	// counters
	double weightedSumCounter = 0;
	double lastValue = std::numeric_limits<double>::quiet_NaN();
	double maxCounter  = std::numeric_limits<double>::quiet_NaN();
	double minCounter  = std::numeric_limits<double>::quiet_NaN();
	double sumCounter  = 0;
	double avgCounter = 0;
	uint eventCounter  = 0;

	// base logger to use
	Simulator* model;
	std::shared_ptr<IPowerDEVSLogger> baseLogger;
	std::string variableName; // name of the variable we are sampling

public:
	/*! read sample period from configuration using model's name */
	SamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger);
	SamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger, double samplePeriod);
	void initSignals(std::vector<std::string> variableNames);
	void initSignals(std::map<std::string,int> variableNamesAndPriorities);
	void logSignal(double, double, std::string);
	void flush(double t);

private:
	void resetCounters();
	void logSampledValues(double t);

};

#endif
