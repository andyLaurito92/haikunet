#include "testsink.h"
void testsink::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

}
double testsink::ta(double t) {
//This function returns a double.
return 1e10;
}
void testsink::dint(double t) {

}
void testsink::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition
printLog("Sink: received something \n");


//o = *((std::shared_ptr<A>*)x.value);

o = (std::shared_ptr<A>) ((EventPtr)x).valuePtr;

this->vector.push_back(o);

printLog("received %d\n", o->t);
}
Event testsink::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)


return Event();
}
void testsink::exit() {
	printLog("EXIT:------- \n");
	for(auto ptr : this->vector){
		printLog("acumulated %d\n", ptr->t);
	}

	this->vector.clear();

}
