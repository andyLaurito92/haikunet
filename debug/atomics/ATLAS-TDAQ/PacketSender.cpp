#include "PacketSender.h"

void PacketSender::init(double t, ...) {
	// wrapper to init(double t, va_list parameters) to be able to read parameters in inheriting classes

	va_list parameters;
	va_start(parameters, t);

	this->init(t, &parameters);

	va_end(parameters);
}

void PacketSender::init(double t, va_list* parameters) {
	if(this->isBaseClass){
		sigma =INF;
		return;
	}

	char* fvar;

	fvar = va_arg(*parameters,char*);
	cantRequests = readDefaultParameterValue<int>(fvar);
	printLog(10, "[%f] %s_init: cantRequests =  %i \n", t, this->getName(), cantRequests);

	fvar = va_arg(*parameters,char*);
	requestDelay = readDefaultParameterValue<double>(fvar);
	printLog(10, "[%f] %s_init: requestDelay =  %f \n", t, this->getName(), requestDelay);

	this->logger = createDefaultLogger(SCILAB_LOG_LEVEL_NORMAL, this->getName());
	this->logger->initSignals(1, "sent");

	mystate = WAITING;
	sentRequests = 0;
	totalEventsRequested = 0;
	sigma = INF; // Waits for first signal to arrive

	return;
}

double PacketSender::ta(double t) {
	//This function returns a double.
	return sigma;
}

void PacketSender::dint(double t) {
	if (sentRequests < cantRequests){ // keep sending next request
		sigma = requestDelay;
	} else { // already sent all requests. Wait for next signal to restart
		totalEventsRequested++;
		mystate = WAITING;
		sigma = INF;
	}

	printLog(10, "[%f] %s_int: sigma: %f ; sentRequests:%i \n", t, this->getName(), sigma, sentRequests);

	return;
}
void PacketSender::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition

	// Signal to request for new event arrived
	if (mystate == REQUESTING) {
		printLog(2, "[%f] %s_ext: WARNING!!! A signal to start requesting arrived while the model was still sending previous packets. CANCELING previous job and restarting with the new one  \n", t,this->getName());
		totalEventsRequested++;
	}

	mystate = REQUESTING;
	sentRequests = 0;
	sigma = 0;

	printLog(10, "[%f] %s_ext: A signal to start requesting arrived  \n", t,this->getName());
}
Event PacketSender::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)
	Packet* p = createPacketToSend(t);
	if(p == NULL) return Event();

	sentRequests++;
	printLog(10, "[%f] %s_lamb: Sending request %i (of %i) with the following packet: \n", t, this->getName(), sentRequests, cantRequests);
	p->printPacketInfo(10);

	this->logger->logSignal(t,(double)sentRequests,"sent");

	return Event(p, 0);
}

void PacketSender::exit() {
	this->logger->flush();
}
