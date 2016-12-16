#include "DcmLoadSampler.h"

void DcmLoadSampler::init(double t, ...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s: Init: \n", t, this->getName());

	char *fvar;

	fvar = va_arg(parameters, char*);
	samplePeriod = (double) getScilabVar(fvar, true);

	printLog(LOG_LEVEL_INIT, "[%f] %s: samplePeriod: %f \n", t, this->getName(), samplePeriod);

	sigma = samplePeriod;

	// reset
	resentCounters();

	this->logger->initSignals({"max", "min", "count", "proccessedEvents", "assignedEvents"});

	return;
}

void  DcmLoadSampler::resentCounters(){
	eventCounter = 0;
	processedEvents=0;
//	busyPus = 0; // DO not reset as it has to keep previous value
	weightedSumCounter = 0;
	maxCounter = busyPus;
	minCounter = busyPus;
        assignedEvents = 0; 

	lastT = -1;
}

void DcmLoadSampler::dint(double t) {
	sigma = samplePeriod;
	if(samplePeriod < 0){ // use samplePeriod=-1 to log everything
		sigma = INF;
	}
}
void DcmLoadSampler::dext(Event x, double t) {
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Packet arrived to port %u \n", t, this->getName(), x.port);

	eventCounter++;

	if(x.port == 0){ // new assignment being set from HLTSV to PU
                assignedEvents ++;
		busyPus++;
	}

	if (x.port == 1){ // PU finished processing event
		processedEvents++;
		busyPus--;
	}

	if(lastT != -1 && lastT != t){ // If more than one value arrive in the same T, only sample the last one for that T
		weightedSumCounter += busyPus * (t - lastT);
		maxCounter = std::max(maxCounter, busyPus);
		minCounter = std::min(minCounter, busyPus);
	}

	lastT = t;

	sigma = sigma - e; // continue as before
	if(samplePeriod < 0){ // use samplePeriod=-1 to log everything
		sigma = 0;
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Counters: events=%i; busyPus=%i; processedEvents=%i; weightedSum=%f; max=%i; min=%i;  \n", t, this->getName(), eventCounter, busyPus, processedEvents, weightedSumCounter, maxCounter, minCounter);

	return;
}
Event DcmLoadSampler::lambda(double t) {
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lambda: Sampling now ... \n",t, this->getName());

	// record sample for last value
	if(lastT != -1 && lastT != t){
		weightedSumCounter += busyPus * (t - lastT);
		maxCounter = std::max(maxCounter, busyPus);
		minCounter = std::min(minCounter, busyPus);
	}

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lambda: Counters: events=%i; busyPus=%i; processedEvents=%i; sum=%f; max=%i; min=%i;  \n", t, this->getName(), eventCounter, busyPus, processedEvents, weightedSumCounter, maxCounter, minCounter);

	// log to scilab
	this->logger->logSignal(t,this -> maxCounter,"max");
	this->logger->logSignal(t,this -> minCounter,"min");
	this->logger->logSignal(t,this -> eventCounter,"count");
	this->logger->logSignal(t,this -> assignedEvents,"assignedEvents");
	//this->logger->logSignal(t,busyPus,"lastVal");
	this->logger->logSignal(t,this -> processedEvents,"proccessedEvents");

	// reset
	resentCounters();

	return Event();  // no output, just log to scilab
}
