#include "VectorRosCoreLink2CoreQueueVector.h"

void VectorRosCoreLink2CoreQueueVector::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char* fvar = va_arg(parameters,char*);
	nCoreSwitches = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: nCoreSwitches: %i \n", t, this->getName(), nCoreSwitches);

	fvar = va_arg(parameters,char*);
	nDcmToR = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: nDcmToR: %i \n", t, this->getName(), nDcmToR);

	sigma=INF;
}

double VectorRosCoreLink2CoreQueueVector::ta(double t) {
	//This function returns a double.
	return sigma;
}

void VectorRosCoreLink2CoreQueueVector::dint(double t) {
	sigma=INF;
	if(outputVectorialEvents.size() != 0){
		sigma = 0;
	}
}

void VectorRosCoreLink2CoreQueueVector::dext(Event x, double t) {
	// Event arrived from PU
	auto arrivedEvent = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(arrivedEvent->valuePtr);

	// Safety Checks
	if(arrivedEvent == NULL){// wrong event format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (VectorialEvent*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}
	if(arrivedPacket == NULL){// wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: [WARNING!] Error wrong packet received: unable to cast to (TdaqPacket*). \n",t, this->getName());
		sigma = sigma - e; // continue as before
		throw "Unable to cast to (TdaqPacket*)";
		return;
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received arrivedEvent from ROS link %i \n", t, this->getName(), arrivedEvent->index);

	// even links go to Core 0, uneven links go to Core 1
	int rosLinkIndex = arrivedEvent->index;
	int coreIndex = rosLinkIndex % this->nCoreSwitches;

	// each TPU ToR has a queue associated in each core. Calculate ToR based on DCM id (coming in the IP protocol)
	auto ipProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_IP);
	int dcmId = ipProtocol->headerFields[IP_DST];
	int torIndex = NetworkConfiguration::getTpuRackIndex(dcmId);

	// Even queues are for Core 2, uneven queues are for Core 1
	int queueId = NetworkConfiguration::getCoreQueueIndex(coreIndex, torIndex);


	// Change the index of the even to go to the corresponing DCM
	auto vec = std::make_shared<VectorialEvent>();
	vec->valuePtr = arrivedEvent->valuePtr;
	vec->index = queueId;

	outputVectorialEvents.push(vec);

	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Received message from rosLink=%i directed to DCM=%i (at ToR=%i). It goes to CoreQueue=%i (Core=%i) \n", t, this->getName(), rosLinkIndex, dcmId, torIndex, queueId, coreIndex);

	sigma=0;
}

Event VectorRosCoreLink2CoreQueueVector::lambda(double t) {
	auto sentEvent = outputVectorialEvents.front();
	outputVectorialEvents.pop();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Forwarding event using index %u \n", t, this->getName(), sentEvent->index);
	return Event(sentEvent,0);
}

void VectorRosCoreLink2CoreQueueVector::exit() {

}
