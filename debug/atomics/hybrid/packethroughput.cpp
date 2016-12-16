#include "packethroughput.h"
void packethroughput::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

// myLoggerToScilab = new aLoggerToScilab(this->myself,this->getName());
// myLoggerToScilab->initSignals(1,"rate");

for (int i=0;i<10;i++) {
	out_rate[i]=0;
	};

lastcount = 0;
runningtime = 0;
rate = 0;
slope_rate=0;

sigma = inf;  			//
return;
//
}
double packethroughput::ta(double t) {
//This function returns a double.
return sigma;
}
void packethroughput::dint(double t) {
sigma=inf;
return;
}
void packethroughput::dext(Event x, double t) {
// For ANY current State do:

			lastcount++;
			runningtime=runningtime+e;
			rate=lastcount/runningtime;
			slope_rate=1/e;
			sigma = 0;
return ;
}
Event packethroughput::lambda(double t) {
// For ANY current State do:
		out_rate[0]=rate ;
		out_rate[1]=slope_rate ;

		return Event(&out_rate,0) ;
}
void packethroughput::exit() {
//Code executed at the end of the simulation.
}
