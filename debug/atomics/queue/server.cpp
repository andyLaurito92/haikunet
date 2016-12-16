#include "server.h"
void server::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type
Sigma = 1;
processed = 0;
printLog("(server: Init finised: \n" );
}
double server::ta(double t) {
	//This function returns a double.
	return Sigma;
}
void server::dint(double t) {
// Here I process
	printLog("Server INT. Server asking for data\n");
	Sigma = INF; 
}
void server::dext(Event x, double t) {
	printLog("[%f]Server is processing new element \n", t);
	printLog("[%f]Server. Received event from port: %i with value %i. Is even NULL:  %i \n", t, x.port, x.value, x.isNotNull());

	Sigma = 1;
	if(t > 5) Sigma = 0;
	processed++;
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
}
Event server::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)
	printLog("Server lambda. Server says ready\n");
	return Event(&l_server, 1);
}
void server::exit() {
//Code executed at the end of the simulation.
}
