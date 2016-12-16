#include "EBProcessing.h"


void EBProcessing::init(double t, ...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char* fvar;

	stochasticSimulation = readDefaultParameterValue<int>(SCILAB_VARNAME_STOCHASTIC_SIMULATION);
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: stochasticSimulation: %s \n", this->getName(), stochasticSimulation?"YES":"NO");

	fvar = va_arg(parameters,char*);
	processingPeriod = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: processingPeriod =  %f \n", this->getName(), processingPeriod);

	this->logger->initSignals(std::vector<std::string> { "procTime" });

	sigma = INF;  // wait for first request to arrive
}

void EBProcessing::dint(double t) {
	sigma = INF;

	return;
}
void EBProcessing::dext(Event x, double t) {
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: packet arrived. Port:%i \n",t, this->getName(), x.port);
	this->packetBeingProcessed = std::static_pointer_cast<TdaqPacket> (x.valuePtr);

	sigma = processingPeriod;
	if(stochasticSimulation){
		sigma = STDEVS::getConfigurationInstance()->exponential(processingPeriod);
		//sigma = STDEVS::StDevsDefaultInstance->uniform() * processingPeriod * 2;
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: procTime=%f \n",t, this->getName(), sigma);
	this->logger->logSignal(t, sigma, "procTime");
}

Event EBProcessing::lambda(double t) {
	printLog(LOG_LEVEL_PRIORITY, "[%f]%s_lamb: EB finished processing. Forwarding to packet discard. \n", t, this->getName());

	return Event(this->packetBeingProcessed, 0);
}



