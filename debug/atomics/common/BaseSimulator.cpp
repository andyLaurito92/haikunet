#include "BaseSimulator.h"

void BaseSimulator::init(double t) {
	// Read parameters
	this->debugLevel = readDebugLevel(this);

	// create a default logger
	//	this->logger = createDefaultLogger(this);
	this->logger = std::make_shared<ConfigurationLogger>(this);
}

double BaseSimulator::ta(double t){
	if(!this->logger && debugLevel <0){
		throw std::runtime_error("BaseSimulator::init method was not called at INIT for model '" + std::string(this->getName()) + "' \n");
	}

	return this->sigma;
}

void BaseSimulator::exit(double t) {
	//Code executed at the end of the simulation.
	this->logger->flush(t);
}

void BaseSimulator::debugMsg(int level, const char *fmt,...){
	// TODO: replace this implementation with something more rebust like boost.log http://www.boost.org/doc/libs/1_61_0/libs/log/doc/html/index.html
	if (this->debugLevel >= level) {
		char fileName[1024];
		sprintf(fileName,"%s/pdevs_run%i.log",getExecutablePath(), SimulationExperimentTracker::getCurrentSimuRun());

		va_list va;
		va_start(va,fmt);
		vprintLogLevel(fmt, fileName, va);
		va_end(va);
	}
}
