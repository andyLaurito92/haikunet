#include "packetgen.h"

void packetgen::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	  %Type% is the parameter type

	//////////////////////////////// STDEVS
	//    mySTDEVS = new STDEVS(_STDEVS_DEFAULT_SEED);
	
	//    mySTDEVS = new STDEVS(12345678);
	//printLog ("%s: STDEVS Init Poisson %u \n", STDEVS::StDevsDefaultInstance->poisson(500));
	//printLog ("%s: STDEVS Init Bernoulli %f \n", STDEVS::StDevsDefaultInstance->bernoulli(0.5));
	stochasticSimulation = readDefaultParameterValue<int>(SCILAB_VARNAME_STOCHASTIC_SIMULATION);;
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: stochasticSimulation: %s \n", this->getName(), stochasticSimulation?"YES":"NO");


	FlowID = va_arg(parameters,char*);
	printLog(LOG_LEVEL_INIT, "[%f] %s: FlowID: %s \n",t, this->getName(),FlowID);

	char* fvar;

	fvar = va_arg(parameters,char*);
	Period = readDefaultParameterValue<double>(fvar);;
	printLog(LOG_LEVEL_INIT, "[%f] %s: Period: %f \n",t, this->getName(),Period);

	fvar = va_arg(parameters,char*);
	Psize = readDefaultParameterValue<double>(fvar);;
	printLog(LOG_LEVEL_INIT, "[%f] %s: Packet Size: %f bits \n",t, this->getName(),Psize);

	fvar = va_arg(parameters,char*);
	StartTime = readDefaultParameterValue<double>(fvar);;
	printLog(LOG_LEVEL_INIT, "[%f] %s: StartTime: %f secs. \n",t, this->getName(),StartTime);

	fvar = va_arg(parameters,char*);
	packetType = (double)getScilabVar(fvar, true);

	sigma=StartTime;

	counter=0;

	this->logger = createDefaultLogger(SCILAB_LOG_LEVEL_NORMAL, this->getName());
	this->logger->initSignals(3,"count", "pktlength","intergen");

	return;
}

double packetgen::ta(double t) {
	//This function returns a double.
	printLog(10, "[%f]%s_ta: sigma= %f \n",t, this->getName(),sigma);
	return sigma;
}
void packetgen::dint(double t) {
	// FOR TESTING PURPOSES ONLY:
	// for ( it=mymap.begin() ; it != mymap.end(); it++ )
	//     printLog(10, "[%f] %s: key = %u val = %u \n",t, this->getName(), this->getName(),(*it).first,(*it).second);
	// 	  sigma=Period; // Forces a Fixed Period


	sigma = Period;
	if(stochasticSimulation)
		sigma = STDEVS::STDEVS::StDevsDefaultInstance->exponential(Period);

	this->logger->logSignal(t,sigma,"intergen");

	printLog(10, "[%f]%s_int: intergen = %f \n",t, this->getName(),sigma);

	// FOR TESTING PURPOSES ONLY:
	//if (t>200) {sigma=STDEVS::StDevsDefaultInstance->exponential(Period*2.05);}; // FORCES A STEP IN THE INTERARRIVAL TIMES

	return;
}
void packetgen::dext(Event x, double t) {
	return; // no input ports
}

int eventId= 0;
Event packetgen::lambda(double t) {

	// General Packet
//	auto packet = TdaqPacket::createPacket(t, this->FlowID);
//	auto responseProtocol = std::make_shared<protocol>();
//	responseProtocol->protoID = PROTOCOL_ID_1LTRIGGER_RESPONSE;
//	responseProtocol->layer = 7;
//	responseProtocol->METADATA[PAYLOADSIZE_BITS] = this->Psize;
//	responseProtocol->METADATA[CREATION_TIME] = t;
//	packet->ProtocolList.push_back(responseProtocol);

	/// ROS response packet
	int rosId = 1;//rand() % 150; //[0,149]
	int dcmId = rand() % 39; //[0,38];
	auto info = std::make_shared<FragmentInfo>(eventId++, rosId /*rosID*/, 3 /*robID*/);
	auto requestInfo = std::make_shared<FragmentRequestInfo> (1 /*eventID*/, rosId /*rosID*/, std::vector<int> {1, 2, 3, 4}, 1 /*puID*/, dcmId /*dcmID*/);

        int eventId = counter;
        int puId = 0;
        if (packetType == HLTSV_REQUEST_PACKET){
            auto returnPacket = TdaqPacket::createHltsvRequestPacket(t,
                    std::make_shared<HltsvRequest>(puId, eventId));

            counter++;

            // this->logger->logSignal(t,returnPacket->getLength_bits(),"pktlength");
            // this->logger->logSignal(t,counter,"count");
            // printLog(LOG_LEVEL_PRIORITY, "[%f] %s: Generated packet (#%i): \n", t, this->getName(), returnPacket->ID);
            // this->logger->logSignal(t,counter,"count");
            printLog(LOG_LEVEL_IMPORTANT, "[%f] %s: Generated packet (#%i) of type [%f]\n", t, this->getName(), returnPacket->ID, packetType);
            return Event(returnPacket, 0); 
        }
        if (packetType == FRAGMENT_PACKET){

            int fragmentSize = 100* 1024;
            auto requestPacket = TdaqPacket::createFirstLevelTriggerRequestPacket(t, requestInfo, 188);
            auto packet = TdaqPacket::createFirstLevelTriggerResponsePacket(t, fragmentSize, info, requestPacket);

            counter++;

            // this->logger->logSignal(t,returnPacket->getLength_bits(),"pktlength");
            // this->logger->logSignal(t,counter,"count");
            printLog(LOG_LEVEL_IMPORTANT, "[%f] %s: Generated packet (#%i) of type [%f]\n", t, this->getName(), packet->ID, packetType);
            return Event(packet, 0); 
        }
        return Event(NULL, 0); 



}
void packetgen::exit() {
	//Code executed at the end of the simulation.
	this->logger->flush();
}
