#include "VectorDcm2DcmToRVector.h"

void VectorDcm2DcmToRVector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char* fvar = va_arg(parameters,char*);
	dcmPerRack = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: dcmPerRack: %i \n", t, this->getName(), dcmPerRack);

	sigma=INF;
}

double VectorDcm2DcmToRVector::ta(double t) {
	//This function returns a double.
	return sigma;
}

void VectorDcm2DcmToRVector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void VectorDcm2DcmToRVector::dext(Event x, double t) {
	// Event arrived from PU
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);

	// Safety Checks
	if(arrivedEvent == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (VectorialEvent*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	int dcmIndex = arrivedEvent->index;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received arrivedEvent from DCM %i \n", t, this->getName(), dcmIndex);


	// find out DCM to which this PU correspond to
	int dcmToRIndex = dcmIndex / this->dcmPerRack;

	// Change the index of the even to go to the corresponing DCM
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedEvent->valuePtr;
	vec->index = dcmToRIndex;

	outputVectorialEvents.push(vec);

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Received message from DCM=%i directing it to DCM_TOR=%i \n", t, this->getName(), dcmIndex, vec->index);

	sigma=0;
}

Event VectorDcm2DcmToRVector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void VectorDcm2DcmToRVector::exit() {

}
