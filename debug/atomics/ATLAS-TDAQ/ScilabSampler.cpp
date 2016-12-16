#include "ScilabSampler.h"

void ScilabSampler::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s: Init: \n", t, this->getName());

	char *fvar;

	fvar = va_arg(parameters, char*);
	samplePeriod = readDefaultParameterValue<double>(fvar);

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s: samplePeriod: %f \n", t, this->getName(), samplePeriod);

	sigma = samplePeriod;

	// reset
	resetCounters();

	this->logger->initSignals({"max", "avg", "min", "count", "sentBytes", "discards"});

	return;
}

void  ScilabSampler::resetCounters(){
	eventCounter = 0;
	weightedSumCounter = 0;
	maxCounter = lastQueueSize;
	minCounter = lastQueueSize;
	discardsCounter = 0;
	sentBytesCounter = 0;
}

void ScilabSampler::dint(double t) {
	sigma = samplePeriod;
	if(samplePeriod < 0){ // use samplePeriod=-1 to log everything
		sigma = INF;
	}
}
void ScilabSampler::dext(Event x, double t) {
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Packet arrived to port %u \n", t, this->getName(), x.port);
	auto value = (std::static_pointer_cast<double> (x.valuePtr)).get();

	double newQueueSize =  value[0];
	double discard =  value[1];

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: value[0]=%f value[1]=%f \n", t, this->getName(), newQueueSize, discard);

	discardsCounter += discard;
	eventCounter++;

	// record sent packets
	if(newQueueSize < lastQueueSize){ // a packet was dequeued
		sentBytesCounter += lastQueueSize - newQueueSize;
	}

	if(lastT != -1 && lastT != t){ // If more than one value arrive in the same T, only sample the last one for that T
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Enter counters calculation \n", t, this->getName());
		weightedSumCounter += lastQueueSize * (t - lastT);
		maxCounter = std::max(maxCounter, lastQueueSize);
		minCounter = std::min(minCounter, lastQueueSize);
	}

	lastT = t;
	lastQueueSize = newQueueSize;

	sigma = sigma - e; // continue as before
	if(samplePeriod < 0){ // use samplePeriod=-1 to log everything
		sigma = 0;
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Counters: events=%i; discards=%i; sentBytes=%i; weightedSum=%f; max=%i; min=%i;  \n", t, this->getName(), eventCounter, discardsCounter, sentBytesCounter, weightedSumCounter, maxCounter, minCounter);
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext:  lastT=%f, lastQueueSize=%i \n", t, this->getName(), lastT, lastQueueSize);

	return;
}
Event ScilabSampler::lambda(double t) {
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Sampling now ... \n",t, this->getName());

	// record sample for last value
	if(lastT != -1 && lastT != t){
		weightedSumCounter += lastQueueSize * (t - lastT);
		maxCounter = std::max(maxCounter, lastQueueSize);
		minCounter = std::min(minCounter, lastQueueSize);
	}
	double avg = samplePeriod>0? weightedSumCounter / samplePeriod : weightedSumCounter;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Counters: events=%i; discards=%i; sentBytes=%i; sum=%f; max=%i; min=%i; avg=%f  \n", t, this->getName(), eventCounter, discardsCounter, sentBytesCounter, weightedSumCounter, maxCounter, minCounter,avg);

	// log to scilab
	logger->logSignal(t,maxCounter,"max");
	logger->logSignal(t,minCounter,"min");
	logger->logSignal(t,avg,"avg");
	logger->logSignal(t,eventCounter,"count");
	logger->logSignal(t,discardsCounter,"discards");
	logger->logSignal(t,sentBytesCounter,"sentBytes");

	// reset
	resetCounters();
        lastT = t;

	return Event();  // no output, just log to scilab
}
