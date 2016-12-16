#include "clients.h"
void clients::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//time = va_arg(parameters, double);
	//printLog("(Clients: Time is: %d\n" , time);
	Sigma = 0.5;
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type
printLog("(Clients: Init Finished \n");

}
double clients::ta(double t) {
	return Sigma;
}
void clients::dint(double t) {
printLog("(Clients: Int \n");
}
void clients::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
printLog("Client ext\n");

}
Event clients::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)

	printLog("Client enters the show\n"); 
	return Event(&Sigma, 0);
}
void clients::exit() {
	//Code executed at the end of the simulation.

}
