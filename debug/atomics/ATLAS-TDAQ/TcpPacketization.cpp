#include "TcpPacketization.h"

void TcpPacketization::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type

	char* fvar = va_arg(parameters,char*);
	maxPacketSize = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: maxPacketSize: %f \n", t, this->getName(), maxPacketSize);

	fragmentSize = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: fragmentSize: %f \n", t, this->getName(), fragmentSize);

	fvar = va_arg(parameters,char*);
	fragmentSd = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: fragmentSd: %f \n", t, this->getName(), fragmentSd);

	fvar = va_arg(parameters,char*);
	minFragmentSize = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: minFragSize: %f \n", t, this->getName(), minFragmentSize);

	this->logger->initSignals({"fragmentSize"});


	sigma = INF; // wait
}

void TcpPacketization::dint(double t) {
	if(waitingResponseToBeSent.size() == 0)  sigma = INF; // nothing else to send
	else sigma = 0; // send next packet
}

void TcpPacketization::dext(Event x, double t) {
	// Request arrived
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received information to packetice \n", t, this->getName());
	requestPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);

	// Retrieve #ROB & #ROS
	auto requestInfo = requestPacket->getFragmentRequestInfo();

	if (requestInfo == NULL ) { // wrong packet format, ignore it
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: Error wrong packet received: unable to find #ROB or #ROS in arrived packets. \n",t, this->getName());
		sigma = sigma - e; // continue as before ツ
		throw "invalid model state";
		return;
	}

	// TODO: the response is calculated in this model, which should only packetize.
	auto responseFragmentInfo = requestInfo->toFragmentInfo();

	if(waitingResponseToBeSent.size() > 0) {
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: Model was not expecting a new job yet. Did you forget to put a queue before this model? Joining outputs anyway.. \n",t, this->getName());
		waitingResponseToBeSent.reserve(waitingResponseToBeSent.size() + responseFragmentInfo.size());
		waitingResponseToBeSent.insert(waitingResponseToBeSent.end(), responseFragmentInfo.begin(), responseFragmentInfo.end());
	}  else {
		waitingResponseToBeSent = responseFragmentInfo;
	}
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received %i fragments to be packetized \n", t, this->getName(), waitingResponseToBeSent.size());

	sigma = 0; // start sending packets immediately
}

Event TcpPacketization::lambda(double t) {
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb \n", t, this->getName());

	auto packetToSend = getNextPacketToSend(t);

	std::shared_ptr<FragmentInfo> payload = std::static_pointer_cast<FragmentInfo> (packetToSend->payload);
	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lamb: Emitting Packet ID #%u(%s) with ROB=%i (eventId=%i). Directed to PU=%i (DCM=%i)\n", t, this->getName(), packetToSend->ID, packetToSend->FlowID,payload->robId, payload->eventId,(int)packetToSend->getProtocol(PROTOCOL_ID_TCP)->headerFields[DST_PORT], (int) packetToSend->getProtocol(PROTOCOL_ID_IP)->headerFields[IP_DST]);

	// send response
	return Event(packetToSend, 0);
}

std::shared_ptr<TdaqPacket> TcpPacketization::getNextPacketToSend(double t) {
	auto info = waitingResponseToBeSent.back();
	waitingResponseToBeSent.pop_back(); // remove from waiting list

	// TODO: this assumes 1 message/packet per fragment. This is not always the case in TDAQ depending on fragmentSize and TCP config, some fragments go in 2 TCP packets.
        int fsize = static_cast <int> (std::max(minFragmentSize, std::floor( STDEVS::getConfigurationInstance()->normal(fragmentSize, fragmentSd))));

	auto p = TdaqPacket::createFirstLevelTriggerResponsePacket(t, fsize, info, requestPacket);

	printLog(LOG_LEVEL_PRIORITY, "[%f]%s: Created packet #%i to be emitted from ROS=%i with ROB=%i size: %i\n", t, this->getName(), p->ID, info->rosId,  info->robId, fsize);

        if(fsize <=0) 
            printLog(LOG_LEVEL_IMPORTANT, "[%f]%s: Created packet #%i to be emitted from ROS=%i with ROB=%i size: %i, mean %f, sd %f\n", t, this->getName(), p->ID, info->rosId,  info->robId, fsize,  fragmentSize, fragmentSd);

        this->logger->logSignal(t, (double) fsize, "fragmentSize");

	p->printPacketInfo(10);

	return p;
}
