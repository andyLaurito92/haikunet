#include "tokenbucket.h"

void tokenbucket::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//      %Type% is the parameter type

printLog(10, "[init] tockenBucket_init \n");

char* fvar;

// Maximum amount of tickets
fvar = va_arg(parameters,char*);
maxbucketsize = readDefaultParameterValue<int>(fvar);
if(maxbucketsize < 0) maxbucketsize = INF;

printLog(LOG_LEVEL_INIT, "[%f] tockenBucket_init: maxbucketsize =  %i \n", t, maxbucketsize);

// Starting amount of Tickets
fvar = va_arg(parameters,char*);
Tickets = readDefaultParameterValue<int>(fvar);
if(Tickets < 0) Tickets = 0;

printLog(LOG_LEVEL_INIT, "[%f] tockenBucket_init: Tickets =  %i \n", t, Tickets);

waitingPackets = 0;
sigma=INF;
elapsed=0;

for (int i=0;i<10;i++){
  y[i]=0;
};

	this->logger = createDefaultLogger(SCILAB_LOG_LEVEL_NORMAL, this->getName());
	this->logger->initSignals(2, "Tickets", "waitingPackets");

}
double tokenbucket::ta(double t) {
//This function returns a double.
return sigma;
}
void tokenbucket::dint(double t) {
	Tickets--;
	waitingPackets--;

	if (Tickets > 0 && waitingPackets > 0) {
		sigma = 0; // there are still available tickets and packets
		printLog(10, "[%f] TokenBucket_int: Programmed to KEEP SENDING immediately. Tickets=%i waitingPackets=%i sigma=%f \n",t,Tickets, waitingPackets, sigma);
	} else {
		sigma = INF; // needs to wait for tickets or packets
		printLog(10, "[%f] TokenBucket_int: Programmed to WAIT. Tickets=%i waitingPackets=%i sigma=%f \n",t,Tickets, waitingPackets, sigma);
	}

	this->logger->logSignal(t, (double) Tickets, "Tickets");
	this->logger->logSignal(t, (double) waitingPackets, "waitingPackets");
}
void tokenbucket::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
	if (x.port == 0) { // packet arrived
		waitingPackets++;
		printLog(10, "[%f] TokenBucket_ext: PACKET ARRIVED. Tickets=%i waitingPackets=%i \n",t,Tickets, waitingPackets);
	}

	if (x.port == 1){ // Ticket arrived
		Tickets++;
		if (Tickets > maxbucketsize) Tickets = maxbucketsize;

		printLog(10, "[%f] TokenBucket_ext: TICKET ARRIVED. Tickets=%i waitingPackets=%i \n",t,Tickets, waitingPackets);
	}

	if (Tickets > 0 && waitingPackets > 0) {
		sigma = 0; // there are available tickets and packets, forward packet
	} else {
		sigma = INF; // needs to wait for tickets or packets
	}

	this->logger->logSignal(t, (double) Tickets, "Tickets");
	this->logger->logSignal(t, (double) waitingPackets, "waitingPackets");
}
Event tokenbucket::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)

printLog(10, "[%f] TokenBucket_lamda: Packet forwarded. Tickets=%i waitingPackets=%i \n",t,Tickets, waitingPackets);
y[0]=Tickets;
return Event(&y[0],0);
}
void tokenbucket::exit() {
	this->logger->flush();
}
