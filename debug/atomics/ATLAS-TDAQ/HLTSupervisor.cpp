#include "HLTSupervisor.h"

void HLTSupervisor::init(double t, ...) {
	BaseSimulator::init(t);
//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//  %Name% is the parameter name
//	  %Type% is the parameter type

	stochasticSimulation = readDefaultParameterValue<int>(SCILAB_VARNAME_STOCHASTIC_SIMULATION);
	printLog(LOG_LEVEL_IMPORTANT, "[INIT] %s_init: stochasticSimulation: %s \n", this->getName(), stochasticSimulation?"YES":"NO");

	char* fvar;
	fvar = va_arg(parameters, char*);
	rate = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_IMPORTANT, "[%f] %s: rate: %f kHZ (%f per second) \n", t, this->getName(), rate, rate * 1000);
        rate = 1 / (rate * 1000); // rate is in kHZ (rate * 1000 per second)

	fvar = va_arg(parameters, char*);
	totalPuCount = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_IMPORTANT, "[%f] %s: totalPuCount: %i \n", t, this->getName(), totalPuCount);

	fvar = va_arg(parameters, char*);
	double algorithm = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_IMPORTANT, "[%f] %s: assignmentAlgorithm: %f \n", t, this->getName(), algorithm);

	// Check which assignment algorithm was configured
        this -> assignmentAlgorithm = static_cast<ASSIGMET_ALGORITHM>(algorithm);
        if(algorithm >= NUM_TYPES){
            printLog(LOG_LEVEL_ERROR, "Unrecognized assignment algorithm: %i \n", algorithm);
            throw std::runtime_error("Unrecognized assignment algorithm");
        }


        // Strategy assignment parameters
	fvar = va_arg(parameters, char*);
	this->dcmSubsetRatio = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_IMPORTANT, "[%f] %s: dcmSubsetRatio: %f \n", t, this->getName(), this->dcmSubsetRatio);

	fvar = va_arg(parameters, char*);
	this->metric = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_IMPORTANT, "[%f] %s: metric: %d \n", t, this->getName(), this->metric);


	fvar = va_arg(parameters, char*);
	this->priorityQueueSelectionType = readDefaultParameterValue<int>(fvar);

	// create the strategy according to the configured algorithm
	this->strategy = this->createStrategy(this->assignmentAlgorithm);

	mystate = WAITING_RATE;
	sigma = getNextSigma();
	nextEventId = 0;



	// add all PUs ids as idle
	for(int puId=0; puId<totalPuCount; puId++){
		int dcmId = NetworkConfiguration::getDcmIndex(puId);
		int rackId = NetworkConfiguration::getTpuRackIndex(dcmId);

		this->strategy->addPuToDcm(puId, dcmId, rackId);

		printLog(LOG_LEVEL_PRIORITY, "[INIT]%s_init: PUid: %i, dcmId:%i rackId:%i  \n", this->getName(), puId, dcmId, rackId);
	}

	this->strategy->init();

	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: sigma=%f  idlePUs=%u \n", t, this->getName(), sigma, this->strategy->getIdleCores());

	this->logger->initSignals({"sent", "idlePUs", "queuedEventsToSend", "puId", "dcmId"});

	return;
}


void HLTSupervisor::dint(double t) {
	mystate = WAITING_RATE;
	//this->sentPacket.reset();

	sigma = previousSigma; // continue with previous sigma
	if(sigma <= 0) sigma = getNextSigma();
	previousSigma = -1;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: sigma=%f  idlePUs=%u  queuedEventsToSend=%u \n", t, this->getName(), sigma, this->strategy->getIdleCores(), queuedEventsToSend);

	// print a warning message if queuing events.
	if(this->queuedEventsToSend > 0){
		printLog(LOG_LEVEL_ERROR, "[%f] %s_int: WARNING!!! The HLTSV is queuing events, probably badly configured Simulation. There are not free PUs to assign the event. idlePUs=%u  queuedEventsToSend=%u \n", t, this->getName(), this->strategy->getIdleCores(), queuedEventsToSend);
	}

	return;
}

void HLTSupervisor::dext(Event x, double t) {
	if (x.port==0) {	    // PU notified it is now IDLE (finished processing an event)
		auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
		auto eventInfo = std::static_pointer_cast<EventInfo> (arrivedPacket->payload);
		int puId = eventInfo->puId;
		double latency = t- eventInfo->birthTime;
		double netLatency = latency - eventInfo->l2ProcTime;

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: PU %u finished processing event %i and is now IDLE. Measured latency=%f \n", t, this->getName(), puId, eventInfo->eventId, latency);

		// set PU idle
		this->strategy->setIdlePu(puId, DcmMeasuredStatistics {t, latency, eventInfo->l2ProcTime, netLatency, eventInfo->calculateFragmentsSize() });

		sigma=sigma-e; // keep waiting for rate
		mystate = WAITING_RATE;
		if(queuedEventsToSend > 0){
			previousSigma = sigma; // save the elapsed time.
			sigma = 0; // send event to this new idle PU
			mystate = SENDING_TO_IDLE_PU;
		}

		this->logger->logSignal(t,this->strategy->getIdleCores(),"idlePUs");
		this->logger->logSignal(t,queuedEventsToSend,"queuedEventsToSend");
	    this->logger->logSignal(t,nextEventId,"sent");

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: next sigma=%f (previousSigma=%f) \n", t, this->getName(), sigma, previousSigma);
	}
}

Event HLTSupervisor::lambda(double t) {
	if(mystate == WAITING_RATE) {// rate time expired, need to increment available events to send
		queuedEventsToSend++;
	}

	// if there is an event to send and idle PUs, send event
	if(this->strategy->getIdleCores() > 0 && queuedEventsToSend > 0) {
		printLog(LOG_LEVEL_DEBUG, "[%f] %s_lambda: Search for next PU. \n ", t, this->getName());
		int puId = this->strategy->setNextPuBusy(t);
		int eventId = nextEventId;
		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lambda: sending new event %u to PU %u (in DCM %i, rack %i) \n", t, this->getName(), nextEventId, puId, NetworkConfiguration::getDcmIndex(puId), NetworkConfiguration::getTpuRackIndex(NetworkConfiguration::getDcmIndex(puId)));

		queuedEventsToSend--;

		nextEventId++;

		this->logger->logSignal(t,this->strategy->getIdleCores(),"idlePUs");
		this->logger->logSignal(t,queuedEventsToSend,"queuedEventsToSend");
		this->logger->logSignal(t,nextEventId,"sent");
		this->logger->logSignal(t,puId,"puId");

		int dcmId = NetworkConfiguration::getDcmIndex(puId);

		this->logger->logSignal(t,dcmId,"dcmId");

		auto packetToSend = TdaqPacket::createHltsvRequestPacket(t, std::make_shared<HltsvRequest>(puId, eventId));
		return Event(packetToSend, 0);
	}else{

		this->logger->logSignal(t,this->strategy->getIdleCores(),"idlePUs");
		this->logger->logSignal(t,queuedEventsToSend,"queuedEventsToSend");
		this->logger->logSignal(t,nextEventId,"sent");

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lambda: nothing to send (idlePUIds=%u   ; queuedEventsToSend=%u) : \n", t, this->getName(), this->strategy->getIdleCores(), queuedEventsToSend);
		return Event(); // nothing to send
	}
}

void HLTSupervisor::exit(double t) {
	this->strategy.reset();
	BaseSimulator::exit(t);
}

double HLTSupervisor::getNextSigma(){
	double mu = rate;
//	if(stochasticSimulation)
//		mu = STDEVS::StDevsDefaultInstance->exponential(rate);

	return mu;
}

std::shared_ptr<IHltsvStrategy>  HLTSupervisor::createStrategy(ASSIGMET_ALGORITHM assignmentAlgorithm){
	switch (assignmentAlgorithm) {
			case FIFO: {
				return std::make_shared<FifoHltsvStrategy>();
				break;
			}
			case RANDOM_FIFO: {
				return std::make_shared<RandomFifoHltsvStrategy>();
				break;
			}
			case RANDOM: {
				return std::make_shared<RandomHltsvStrategy>();
				break;
			}
			case LESS_LOADED_DCM: {
				return std::make_shared<LeastBusyDcmHltsvStrategy>();
				break;
			}
			case MIN_LATENCY_DCM: {
                                return
                                    std::make_shared<StochasticPriorityHltsvStrategy>(this ->dcmSubsetRatio,
                                            this->metric, this->priorityQueueSelectionType);
				break;
			}
			default: {
				printLog(LOG_LEVEL_ERROR, "[HLTSupervisor] createStrategy: unknown assignment algorithm: %i  \n", assignmentAlgorithm);
				throw std::runtime_error("setIdlePu: unknown assignment algorithm");
				break;
			}
		}
}
