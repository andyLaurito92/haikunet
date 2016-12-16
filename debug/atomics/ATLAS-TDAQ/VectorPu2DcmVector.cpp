#include "VectorPu2DcmVector.h"

void VectorPu2DcmVector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	sigma=INF;
}

double VectorPu2DcmVector::ta(double t) {
	//This function returns a double.
	return sigma;
}

void VectorPu2DcmVector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void VectorPu2DcmVector::dext(Event x, double t) {
	// Event arrived from PU
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	int puId = arrivedEvent->index;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received arrivedEvent from PU %i \n", t, this->getName(), puId);

	// Safety Checks
	if(arrivedEvent == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (VectorialEvent*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	// find out DCM to which this PU correspond to
	int dcmId = NetworkConfiguration::getDcmIndex(puId);

	// Change the index of the event to go to the corresponding DCM
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedEvent->valuePtr;
	vec->index = dcmId;

	outputVectorialEvents.push(vec);

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received message from PU=%i directing it to DCM=%i \n", t, this->getName(), puId, vec->index);

	sigma=0;
}

Event VectorPu2DcmVector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void VectorPu2DcmVector::exit() {

}
