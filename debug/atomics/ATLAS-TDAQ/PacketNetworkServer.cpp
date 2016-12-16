#include "PacketNetworkServer.h"

void PacketNetworkServer::init(double t, ...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s: Init: \n",t, this->getName());

	
	stochasticSimulation = readDefaultParameterValue<int>(SCILAB_VARNAME_STOCHASTIC_SIMULATION);;
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: stochasticSimulation: %s \n", this->getName(), stochasticSimulation?"YES":"NO");

	char* fvar = va_arg(parameters,char*);
	minTransitDelay_ms  = readDefaultParameterValue<double>(fvar);
	minTransitDelay_s = minTransitDelay_ms / 1000;
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: minTransitDelay (ms): %f (%f seconds)\n", t, this->getName(), minTransitDelay_ms, minTransitDelay_s);

	fvar = va_arg(parameters,char*);
	bandwidth_bits_s = readDefaultParameterValue<double>(fvar);
	bandwidth_bits_s_r = 1 / bandwidth_bits_s;
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: bandwidth (bits/sec): %f \n", t, this->getName(), bandwidth_bits_s);

	mystate = REQUESTING;
	counter = 0;
	lastEmitionTime = 0;
	sigma = 0; // Immediately asks for a new packet;

	this->logger->initSignals(std::vector<std::string>{"processingTime", "emitionTime"});
}

void PacketNetworkServer::dint(double t) {
	switch (mystate) {
		case PROCESSING:
			mystate = REQUESTING;
			sigma = 0; // request next job immediately
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f]%s_int: Programmed to request next job (%f). \n", t, this->getName(), sigma);
			break;
		case REQUESTING:
			mystate = WAITING_NEXT_JOB;
			sigma = INF;
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f]%s_int: WAITING for next packet to arrive (%f). \n", t, this->getName(), sigma);
			break;
		default:
			printLog(LOG_LEVEL_FULL_LOGGING, "[%f]1LTrigger_int: Error: Do not know what to do... \n", t, this->getName());
			break;
		}

	return;
}

void PacketNetworkServer::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition
	double minDelay;

	// Request arrived
	incomingPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);

	// Security Check
	if (incomingPacket == NULL ) { // wrong packet format, ignore it
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Error NULL packet received. Packet Ignored and discarded. \n",t, this->getName());
		sigma = sigma - e; // continue as before
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u (%s) \n", t, this->getName(), incomingPacket->ID, incomingPacket->FlowID);

	minDelay = minTransitDelay_s;
	if(stochasticSimulation)
		minDelay = STDEVS::getConfigurationInstance()->exponential(minTransitDelay_s);

	// Calculate sigma according packet size
	double serviceTime_s = bandwidth_bits_s_r * incomingPacket->getLength_bits() + minDelay;


	sigma = serviceTime_s ;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%.10f] %s_ext: Calculated delay for packet: %.10f . PacketSize: %u (bits)  \n", t, this->getName(), sigma, incomingPacket->getLength_bits());

	mystate = PROCESSING; // NOTE: This Server Preempts the previous task if a new packet arrives (unexpectedly) before the completion of the Processing period
	this->logger->logSignal(t,sigma,"processingTime");
}

Event PacketNetworkServer::lambda(double t) {
	switch (mystate) {
	case PROCESSING:
		printLog(LOG_LEVEL_FULL_LOGGING, "[%.10f] %s_lamb: Emitting Packet ID #%u(%s) \n", t, this->getName(), incomingPacket->ID, incomingPacket->FlowID);
		this->logger->logSignal(t,t - lastEmitionTime, "emitionTime");
		lastEmitionTime = t;
		return Event(incomingPacket, 0);
		break;
	case REQUESTING:
		printLog(LOG_LEVEL_FULL_LOGGING, "[%.10f] %s_lamb: Asking for a new packet... \n", t, this->getName());
		return Event(&counter, 1);
		break;
	default:
		printLog(LOG_LEVEL_FULL_LOGGING, "[%.10f] %s_lamb: Error: Do not know what to do... \n", t, this->getName());
		break;
	}
}
