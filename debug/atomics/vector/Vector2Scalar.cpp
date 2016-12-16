#include "Vector2Scalar.h"
void Vector2Scalar::init(double t,...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char* fvar = va_arg(parameters,char*);
	this->index  = readDefaultParameterValue<int>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%f] %s_init: index : %i \n", t, this->getName(), index);

	sigma=std::numeric_limits<double>::infinity();

}

void Vector2Scalar::dint(double t) {
	sigma=std::numeric_limits<double>::infinity();
	if(outputEvents.size() != 0){
		sigma = 0;
	}
}
void Vector2Scalar::dext(Event x, double t) {
	auto vec = castEventPointer<VectorialEvent>(x);

	debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_ext: vectorial event arrived with index %u (model index value is %i) \n", t, this->getName(), vec->index, index);
	if (vec->index==index || vec->index <0 || this->index < 0){
		outputEvents.push(vec->valuePtr);
		sigma=0;
	}
}
Event Vector2Scalar::lambda(double t) {
	auto output = outputEvents.front();
	outputEvents.pop();

	debugMsg(LOG_LEVEL_DEBUG, "[%f] %s_lamb: forwarding event value. Still pending %i events to be sent.\n", t, this->getName(), outputEvents.size());

	return Event(output,0);
}

