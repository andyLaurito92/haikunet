#include "SamplerLogger.h"


SamplerLogger::SamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger) : SamplerLogger(model, baseLogger, -1){
	printLog(LOG_LEVEL_DEBUG, "Created SamplerLogger \n");
	// the sample period will be read when the variable is initialized (so that we know the variable name)
}

SamplerLogger::SamplerLogger(Simulator* model, std::shared_ptr<IPowerDEVSLogger> baseLogger, double samplePeriod) :  samplePeriod(samplePeriod), nextSampleT(samplePeriod), model(model), baseLogger(baseLogger){
	printLog(LOG_LEVEL_DEBUG, "Created SamplerLogger for model %s \n", this->model->getFullName().data());
	// reset
	resetCounters();
	this->lastValue = std::numeric_limits<double>::quiet_NaN();
	this->maxCounter  = std::numeric_limits<double>::quiet_NaN();
	this->minCounter  = std::numeric_limits<double>::quiet_NaN();
}

void SamplerLogger::initSignals(std::map<std::string,int> variableNamesAndPriorities){
	printLog(LOG_LEVEL_ERROR, "Method 'SamplerLogger::initSignals(std::map<std::string,int> variableNamesAndPriorities)' not implemented. Use instead 'SamplerLogger::initSignals(std::vector<std::string> variableNames)'");
	throw std::runtime_error("Method 'SamplerLogger::initSignals(std::map<std::string,int> variableNamesAndPriorities)' not implemented. Use instead 'SamplerLogger::initSignals(std::vector<std::string> variableNames)'");

}

void SamplerLogger::initSignals(std::vector<std::string> variableNames) {
	if(variableNames.size() != 1){
		printLog(LOG_LEVEL_ERROR, "SamplerLogger::initSignals: Trying to initialize sampler for model %s with %u variables. Each sampler can only sample 1 variable at a time. \n", this->model->getFullName().data(), variableNames.size());
		throw std::runtime_error("SamplerLogger::initSignals: Trying to initialize sampler with variables != 1. Only one variable can be samples. See log for details.");
	}

	this->variableName = variableNames[0];

	if(this->samplePeriod <= 0 ){ // Read samplePeriod from config
		if(!readScopedVariable<double>(this->model, this->variableName + "." +  SAMPLER_PERIOD_VARIABLE_NAME, this->samplePeriod) || this->samplePeriod <= 0){
			printLog(LOG_LEVEL_ERROR, "SamplerLogger::initSignals: Sampler failed to retrieve a positive sampling period. Variable '%s' was not correctly set in configuration for model '%s'. \n", (this->variableName + "." +  SAMPLER_PERIOD_VARIABLE_NAME).data(), this->model->getFullName().data());
			throw std::runtime_error("SamplerLogger::initSignals: Sampler failed to retrieve a positive sampling period. See log for details.");
		}
	}
	this->nextSampleT = this->samplePeriod;

	printLog(1, "SamplerLogger::initSignals: model.variable: %s.%s ;  samplePeriod: %f \n", this->model->getFullName().data(), this->variableName.data(), samplePeriod);

	// init variables in base logger
	std::vector<std::string> vars =   {
			"max",
			"timeAvg",
			"min",
			"sum",
			"avg",
			"evCount"};
	this->baseLogger->initSignals(vars);

}

void SamplerLogger::logSignal(double t, double signalValue, std::string signalName) { // Logger
	if(signalName != this->variableName) return; // just in case

	// record samples if period elapsed
	if(t >= this->nextSampleT){
		this->logSampledValues(t);
	}

	this->eventCounter++;
	if(this->lastT != -1 && this->lastT != t){ // If more than one value arrive in the same T, only sample the last one for that T
		this->weightedSumCounter += lastValue * (t - this->lastT);
		this->sumCounter += std::isnan(lastValue)? 0:lastValue;
		this->avgCounter++;
		this->maxCounter = std::fmax(this->maxCounter, this->lastValue);
		this->minCounter = std::fmin(this->minCounter, this->lastValue);
	}

	this->lastT = t;
	this->lastValue = signalValue;
}

void SamplerLogger::logSampledValues(double t){
	double currentTime = this->nextSampleT;
	double totalSampledTime = this->samplePeriod;
	if(t < this->nextSampleT){ // this happens only when flushing
		currentTime = t; // we use the current time when flushing, so be more accurate with the timeAvg sampled
		totalSampledTime = currentTime - (this->nextSampleT - this->samplePeriod) ; // 'currentTime - this->nextSampleT' is the elapsed time since last sample
	}

	// record sample for last value
	if(this->lastT != -1){
		this->sumCounter += std::isnan(lastValue)? 0:lastValue;
		this->avgCounter++;
		double elapsedTimeForValue = currentTime - std::fmax(this->lastT, currentTime - this->samplePeriod); // lastT is lower in case there was no values in the whole samplingPeriod
		this->weightedSumCounter += this->lastValue * elapsedTimeForValue;

		this->maxCounter = std::fmax(this->maxCounter, this->lastValue);
		this->minCounter = std::fmin(this->minCounter, this->lastValue);
	} else { // there was no events in this period
		this->weightedSumCounter = lastValue;
		this->sumCounter = std::isnan(lastValue)? 0:lastValue;
		this->avgCounter++;
		this->maxCounter = lastValue;
		this->minCounter = lastValue;
	}

	// calculate weigthed avg
	double timeAvg = this->weightedSumCounter / totalSampledTime;

	// record samples
	this->baseLogger->logSignal(currentTime, timeAvg, "timeAvg");
	this->baseLogger->logSignal(currentTime, this->eventCounter, "evCount");
	this->baseLogger->logSignal(currentTime, this->maxCounter, "max");
	this->baseLogger->logSignal(currentTime, this->minCounter, "min");
	this->baseLogger->logSignal(currentTime, this->sumCounter, "sum");
	this->baseLogger->logSignal(currentTime, this->sumCounter / this->avgCounter, "avg");

	// reset
	resetCounters();

	// set next sample time
	this->nextSampleT += this->samplePeriod;

	// record samples again (if more that one period had elapsed)
	this->lastT = currentTime;
	if(t >= this->nextSampleT){
		this->logSampledValues(t);
	}
}

void SamplerLogger::flush(double t) { // Flush
	this->logSampledValues(t); // log last values

	this->baseLogger->flush(t);
}

void  SamplerLogger::resetCounters(){
	this->eventCounter = 0;
	this->weightedSumCounter = 0;
	this->sumCounter = 0;
	this->avgCounter = 0;
	this->maxCounter = this->lastValue;
	this->minCounter = this->lastValue;
}
