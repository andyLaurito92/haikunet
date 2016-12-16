#include "testgen.h"
void testgen::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

}
double testgen::ta(double t) {
//This function returns a double.
return 1;
}
void testgen::dint(double t) {

}
void testgen::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition

}
Event testgen::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)
printLog("Gen::Lambda start \n");

//std::shared_ptr<A> p(new A(t));
p.reset(new A(t));


//Event ev = Event(&p, 0);
//
//std::shared_ptr<A>o = *((std::shared_ptr<A>*)x.value);
//printLog("GEN: sending event to port %i \n", ev.port);



return EventPtr(p,0);
//ev.port = 0;
//printLog("GEN: sending event to port %i \n", ev.port);


//return ev;
}
void testgen::exit() {
//Code executed at the end of the simulation.

}
