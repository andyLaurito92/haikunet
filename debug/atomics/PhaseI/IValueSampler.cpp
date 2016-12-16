#include "IValueSampler.h"

void IValueSampler::init(double t, ...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);

	init(t, parameters);
}

void IValueSampler::init(double t, va_list parameters) {
	BaseSimulator::init(t);
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: Init: \n", t, this->getFullName().data());

	char *fvar;
	fvar = va_arg(parameters, char*);
	this->samplePeriod = readDefaultParameterValue<double>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: samplePeriod: %f \n", t, this->getFullName().data(), samplePeriod);

	// if this->samplePeriod < 0, this model is not a sampler and only logs what it receives

	this->sigma = this->samplePeriod;
	if(this->samplePeriod < 0){
		this->sigma = INF;
	}

	// reset
	resetCounters();

	this->logger = createDefaultLogger(this->getFullName());
	this->logger->initSignals({"max", "timeAvg", "min", "sum", "count"});
}


void  IValueSampler::resetCounters(){
	this->eventCounter = 0;
	this->weightedSumCounter = 0;
	this->sumCounter = 0;
	this->maxCounter = this->lastValue;
	this->minCounter = this->lastValue;

}

//double  IValueSampler::ta(double t){
//	return this->sigma;
//}

void IValueSampler::dint(double t) {
	this->sigma = this->samplePeriod;
	if(this->samplePeriod < 0){ // use samplePeriod=-1 to log everything
		this->sigma = INF;
	}
}

void IValueSampler::dext(Event x, double t) {
	//debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Event arrived to port %u \n", t, this->getFullName(), x.port);
	//auto value = (std::static_pointer_cast<double> (x.valuePtr)).get();


	if(this->lastT != -1 && this->lastT != t){ // If more than one value arrive in the same T, only sample the last one for that T
			debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Enter counters calculation \n", t, this->getFullName().data());
			this->eventCounter++;
			this->weightedSumCounter += lastValue * (t - this->lastT);
			this->sumCounter += lastValue;
			this->maxCounter = std::max(this->maxCounter, this->lastValue);
			this->minCounter = std::min(this->minCounter, this->lastValue);
	}

	this->lastT = t;
	this->lastValue = this->getValue(x);

	//debugMsg(1, "[%f] ext: Received Value is %f \n", t, this->lastValue);

	this->sigma = this->sigma - this->e; // continue as before
	if(this->samplePeriod < 0){ // use samplePeriod=-1 to log everything
		this->sigma = 0;
	}
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received new value=%f. Sampler counter:%i weightedSum=%f; \n", t, this->getFullName().data(), this->lastValue, this->eventCounter, this->weightedSumCounter);

	return;
}
Event IValueSampler::lambda(double t) {
	//debugMsg(LOG_LEVEL_IMPORTANT, "[%f] %s_ext: Sampling now ... \n",t, this->getFullName());

	// record sample for last value
	if(this->lastT != -1 && this->lastT != t){
		this->eventCounter++;
		this->sumCounter += lastValue;
		this->weightedSumCounter += this->lastValue * (t - this->lastT);

		this->maxCounter = std::max(this->maxCounter, this->lastValue);
		this->minCounter = std::min(this->minCounter, this->lastValue);
	}

	if(this->samplePeriod < 0 ){
		this->weightedSumCounter = lastValue;
		this->sumCounter = lastValue;
		this->maxCounter = lastValue;
		this->minCounter = lastValue;
	}

    double totalSampledTime = t - this->lastSampleT;
	double timeAvg = this->samplePeriod>0? this->weightedSumCounter / totalSampledTime : this->weightedSumCounter;

//	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: counter:%i weightedSum=%f; \n", t, this->getFullName(), this->eventCounter, this->weightedSumCounter);

	// log to scilab
	debugMsg(LOG_LEVEL_PRIORITY, "[%f] : Logging Values: count=%u; max=%f; min=%f; timeAvg=%f \n", t, this->eventCounter, this->maxCounter, this->minCounter, timeAvg);

	this->logger->logSignal(t, timeAvg, "timeAvg");
	this->logger->logSignal(t, this->eventCounter, "count");
	this->logger->logSignal(t, this->maxCounter, "max");
	this->logger->logSignal(t, this->minCounter, "min");
	this->logger->logSignal(t, this->sumCounter, "sum");

	// reset
	resetCounters();
	this->lastT = t;
	this->lastSampleT = t;

	return Event();  // no output, just log to scilab
}

void IValueSampler::exit(double t) {
	if(t > this->lastT){
		this->lambda(t); //  make sure we log the last entry
	}

	this->logger->flush(t);
}
