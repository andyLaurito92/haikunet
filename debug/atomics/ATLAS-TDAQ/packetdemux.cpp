#include "packetdemux.h"

#include <string>

#include "../../engine/pdevslib.h"

void packetdemux::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	// TODO: using 4 is too rigid. Make the first param to be n and then iterate n times reading args
	for (int i=0;i<4;i++) {
		Labels[i] = va_arg(parameters,char*);
		printLog(LOG_LEVEL_INIT, "[INIT] %s: Label %u = %s \n", this->getName(), i, Labels[i].data());
	};

	char* fvar = va_arg(parameters,char*);
	n = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[INIT] %s: Number of Labels: %u \n", this->getName(), n);

	sigma = INF;
}
double packetdemux::ta(double t) {
	//This function returns a double.
	return sigma;
}
void packetdemux::dint(double t) {
	sigma=INF;
	if(packetBuffer.size() != 0){
		sigma = 0;
	}
}
void packetdemux::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition

	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
	packetBuffer.push(arrivedPacket);

	printLog(10, "[%f] %s_ext: Packet arrived with FlowID=%s \n",t, this->getName(), arrivedPacket->FlowID);

	sigma = 0;
}
Event packetdemux::lambda(double t) {
	auto packetToSend = packetBuffer.front();
	packetBuffer.pop();

	for (int i=0;i<n;i++) {
		if (Labels[i].compare("*") == 0 || Labels[i].compare(packetToSend->FlowID) == 0) {
			printLog(10, "[%f] %s_lamb: forwarding packet %u(%s) by outport %u (matched label: %s) \n", t, this->getName(), packetToSend->ID, packetToSend->FlowID, i, Labels[i].data());
			return Event(packetToSend,i);
		}
	}

	return Event(NULL,0);
}
void packetdemux::exit() {
	//Code executed at the end of the simulation.
}
