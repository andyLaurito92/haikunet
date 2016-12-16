#include "queue.h"
void queue::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type
	l_queue= 0;
	Sigma = INF;
	pending = true;
  printLog("(Queue: Init finised: \n" );
}
double queue::ta(double t) {

//This function returns a double.
	return Sigma;
}
void queue::dint(double t) {
	if(l_queue > 0){
		printLog("Dequeueing\n");
		l_queue -=1;
	} else{
		printLog("Queue is empty, please wait\n"); 
		pending = true;
	}
	Sigma = INF;
}
void queue::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
if(x.port ==1){
		// dequeue
		printLog("Queue EXT. Server asking for element\n");
		Sigma = 0;
}else{
	printLog("Queue enqueing element\n");
	// enqueue
	l_queue += 1;
	Sigma = INF;
	if(pending){
		printLog("Queue had a pending dequeue operation, flush inmediatly\n");
		pending = false;
		Sigma = 0;
	}
}
}
Event queue::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)
	printLog("Queue lambda: start\n");
	if(l_queue > 0){
		printLog("Queue lambda\n");
		return Event(&Sigma, 0);
	}

	printLog("Queue lambda: Returning Nothing\n");
return Event();
}
void queue::exit() {
	//Code executed at the end of the simulation.
}
