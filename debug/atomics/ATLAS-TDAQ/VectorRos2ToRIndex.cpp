#include "VectorRos2ToRIndex.h"

void VectorRos2ToRIndex::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type


	sigma=INF;
}

double VectorRos2ToRIndex::ta(double t) {
	//This function returns a double.
	return sigma;
}

void VectorRos2ToRIndex::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void VectorRos2ToRIndex::dext(Event x, double t) {
	// Event arrived from the ROS
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(arrivedEvent->valuePtr);

	int rosIndex = arrivedEvent->index;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u from ROS=%i\n", t, this->getName(), arrivedPacket->ID, rosIndex);

	int rosToRId = L1TConfiguration::getRosToRIndex(rosIndex); // get TOR to which the message should go to

	// Change the index of the event to go to the corresponding ToR
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedEvent->valuePtr;
	vec->index = rosToRId;

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Received event from ROS=%i. Sending it to ROS_ToR=%i \n", t, this->getName(), rosIndex, vec->index);

	outputVectorialEvents.push(vec);

	sigma=0;
}

Event VectorRos2ToRIndex::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void VectorRos2ToRIndex::exit() {

}
