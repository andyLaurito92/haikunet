#include "packetsink.h"

void packetsink::init(double t, ...) {
	BaseSimulator::init(t);
//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

	char *fvar;

	fvar = va_arg(parameters,char*);

	sigma = 1E10;
	counter = 0;


    /*this->logger->initSignals({ "eventId", "arrived", "packetLatency",
                "roundTripLatency", "evFilterLatency", "L2Latency",
                "l2ProcTime", "puId", "dcmId", "rackId", "eventSize",
                "birthTime"});*/

    this->logger->initSignals(std::map<std::string,int>{
		{"eventId", SCILAB_LOG_LEVEL_DEBUG},
		{"L2Latency", SCILAB_LOG_LEVEL_ALWAYS},
		{"puId", SCILAB_LOG_LEVEL_ALWAYS},
		{"dcmId", SCILAB_LOG_LEVEL_ALWAYS},
		{"rackId", SCILAB_LOG_LEVEL_ALWAYS},
		{"eventSize", SCILAB_LOG_LEVEL_ALWAYS},
	});

    this->logger->initSignals(std::map<std::string,double>{
    	{"packetLatency", SCILAB_LOG_LEVEL_ALWAYS},
    	{"roundTripLatency", SCILAB_LOG_LEVEL_ALWAYS},
		{"evFilterLatency", SCILAB_LOG_LEVEL_ALWAYS},
		{"l2ProcTime", SCILAB_LOG_LEVEL_ALWAYS},
		{"arrived", SCILAB_LOG_LEVEL_PRIORITY},
		{"birthTime", SCILAB_LOG_LEVEL_ALWAYS}
	});

	return;
}

void packetsink::dint(double t) {
	sigma = 1E10;
}

void packetsink::dext(Event x, double t) {
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: received Packet ID %u \n", t, this->getName(), arrivedPacket->ID);

	counter++;
	this->logger->logSignal(t, (double) counter, "arrived");
	this->logger->logSignal(t, t - arrivedPacket->birthTime, "packetLatency");

	// every 50 packets print a message to the log to be able to keep track during simulation
	if(counter % 50 == 0){
		printLog(LOG_LEVEL_IMPORTANT, "[%f] %s_ext: already received %i packets \n", t, this->getName(), counter);
	}

	/*
	- "packetLatency": Latencia de un paquete. Tiempo desde que fue creado hasta el momento en que se esta midiendo. (t-birthTime). FlowID=Any.
	- "roundTripLatency": Latencia de un pedido al 1st level Trigger (1LT). Tiempo desde que el request fue creado hasta que llega la respuesta. (t - requestCreationTime). FlowID=FLOW_ID_1LTRIGGER_RESPONSE
	- "L2Latency": Latencia en aceptar/rechazar un evento. Tiempo desde que se envia el PRIMER pedido al 1LT hasta que el evento sale del PU. (t - eventBirthTime).FlowID=FLOW_ID_EVENT_BUILD_REQUEST
	- "evBuiltLatency": Latencia en construir un evento completo. Tiempo desde que se envia el PRIMER pedido al 1LT (desde la PU) hasta que se envia el evento al MassStorage. (t - eventBirthTime).FlowID=FLOW_ID_BUILT_EVENT
    */

	if (strcmp (arrivedPacket->FlowID, FLOW_ID_1LTRIGGER_RESPONSE) == 0) {
		auto requestProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_1LTRIGGER_REQUEST);
		//protocol responseProtocol = arrivedPacket->getProtocol(PROTOCOL_ID_1LTRIGGER_RESPONSE);

		double latency = t - requestProtocol->metadata[CREATION_TIME];

		this->logger->logSignal(t, (double) latency, "roundTripLatency");
	}

        if (strcmp (arrivedPacket->FlowID, FLOW_ID_EVENT_BUILD_REQUEST) == 0 ||
                strcmp (arrivedPacket->FlowID, FLOW_ID_BUILT_EVENT) == 0) {
		auto info = std::static_pointer_cast<EventInfo>(arrivedPacket->payload);


        int eventSize = info->calculateFragmentsSize();


		double latency = t - info->birthTime;

                int puId = info -> puId;
                int dcmId = NetworkConfiguration::getDcmIndex(puId);
                int rackId = NetworkConfiguration::getTpuRackIndex(dcmId);

                this->logger->logSignal(t, (double) latency, "evFilterLatency");
                this->logger->logSignal(t, (double) info -> l2ProcTime, "l2ProcTime");
		this->logger->logSignal(t, (int) puId, "puId");
		this->logger->logSignal(t, (int) dcmId, "dcmId");
		this->logger->logSignal(t, (int) rackId, "rackId");
		this->logger->logSignal(t, (int) eventSize, "eventSize");
		this->logger->logSignal(t, (double) info -> birthTime, "birthTime");
	}
}

Event packetsink::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)
}

