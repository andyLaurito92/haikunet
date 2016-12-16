#include "VectorIndexMod2Vector.h"
void VectorIndexMod2Vector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char *fvar= va_arg(parameters,char*);
	mod = readDefaultParameterValue<int>(fvar); //(int) getScilabVar(fvar );

	sigma=INF;
}

double VectorIndexMod2Vector::ta(double t) {
	return sigma;
}

void VectorIndexMod2Vector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void VectorIndexMod2Vector::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	auto value = std::static_pointer_cast<double> (arrivedEvent->valuePtr);

	//printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Arrived event with index %i  and value=%f \n", t, this->getName(), arrivedEvent->index, value.get()[0]);
	if ((arrivedEvent->index % mod) == 0){
			// Change the index of the event to go to the corresponding TcpSender
			auto eventToForward = std::make_shared<VectorialEvent>();
			eventToForward->valuePtr = arrivedEvent->valuePtr;
			eventToForward->index = arrivedEvent->index / mod;

			value = std::static_pointer_cast<double> (eventToForward->valuePtr);
			//printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: new index %i and new value=%f\n", t, this->getName(), eventToForward->index, value.get()[0]);

			outputVectorialEvents.push(eventToForward);
	}

	sigma=INF;  // no dot forward event and wait;
	if(outputVectorialEvents.size() != 0){
		sigma = 0; // forward the event immediately
	}
}

Event VectorIndexMod2Vector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lambda: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void VectorIndexMod2Vector::exit() {
//Code executed at the end of the simulation.
}
