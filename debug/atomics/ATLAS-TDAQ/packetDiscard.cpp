#include "packetDiscard.h"

void packetDiscard::init(double t, ...) {
	BaseSimulator::init(t);
//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_init: Initializing \n", t, this->getName());

	char* fvar;

	fvar = va_arg(parameters,char*);
	discardRate = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: discardRate =  %f \n", t, this->getName(), discardRate);

	fvar = va_arg(parameters,char*);
	interleaveDelay = readDefaultParameterValue<double>(fvar);
	if(interleaveDelay < 0) interleaveDelay = 0;
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: interleaveDelay =  %f \n", t, this->getName(), interleaveDelay);

	srand (time(NULL));

	mystate = WAITING;
	sigma = INF;
	arrived, accepted, rejected = 0;

	this->logger->initSignals({ "arrived", "accepted", "rejected"});
}

void packetDiscard::dint(double t) {
	if(arrivedSignals.size() == 0)
	{
		mystate = WAITING;
		sigma = INF;
	}else {
		mystate = SENDING;
		sigma=interleaveDelay;
	}
}
void packetDiscard::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition

	arrived++;
	this->logger->logSignal(t,(double)arrived,"arrived");

	// buffer signal
	arrivedSignals.push(x.valuePtr);

	// forward signal after deay
	if(mystate == WAITING){
		sigma=interleaveDelay;
	} else {
		sigma = sigma - e; // continue as before
	}
}
Event packetDiscard::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)

	auto arrivedSignal = arrivedSignals.front();
	arrivedSignals.pop();

	double value = STDEVS::getConfigurationInstance()->uniform(); // [0, 1]

	//double value = ((double) rand() / (RAND_MAX)); // TODO: use STDEVS here. Check why STDEVS is generating the same number multiple times in a row.
	if((value) >= discardRate)
	{
		accepted++;
		this->logger->logSignal(t,(double)accepted,"accepted");

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lambda: Signal accepted (GeneratedRandomValue=%f). Forwarding accepted signal \n", t, this->getName(), value);
		return Event(arrivedSignal, 0);
	} else{
		rejected++;
		this->logger->logSignal(t,(double)rejected,"rejected");

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lambda: Signal rejected (GeneratedRandomValue=%f). Forwarding rejected signal \n", t, this->getName(), value);
		return Event(arrivedSignal, 1);
	}

}
