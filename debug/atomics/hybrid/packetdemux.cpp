#include "packetdemux.h"
void packetdemux::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type


for (int i=0;i<8;i++) {
	Labels[i] = va_arg(parameters,char*);
 	printLog("[%g] PktDemux: Label %u = %s \n",t,i,Labels[i].data());
};

n=(int) va_arg(parameters,double);
printLog("[%g] PktDemux: Labels: %u \n",t,n);

sigma = INF;
}
double packetdemux::ta(double t) {
//This function returns a double.
return sigma;
}
void packetdemux::dint(double t) {
sigma=INF;
p=NULL;
}
void packetdemux::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition

p = (Packet*)x.value;
FlowID = p->FlowID;

sigma = 0;
}
Event packetdemux::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)

for (int i=0;i<8;i++) {
	if (Labels[i].compare(FlowID) == 0) {
			return Event(p,i);
		};
};

return Event(NULL,0);
}
void packetdemux::exit() {
//Code executed at the end of the simulation.
}
