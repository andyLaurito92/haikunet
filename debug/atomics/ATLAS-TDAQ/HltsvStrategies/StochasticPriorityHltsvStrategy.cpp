#include "StochasticPriorityHltsvStrategy.h"

StochasticPriorityHltsvStrategy::StochasticPriorityHltsvStrategy() :
 IHltsvStrategy(), totalIdlePus(0), doWarmup(true) {
	this->warmupStrategy =  std::make_shared<RandomRoundRobinHltsvStrategy>();

	this->logger = std::make_shared<ScilabLogger>("StochasticPriorityHltsvStrategy", false);
	this->logger->initSignals({"dcmId", "puId", "busyPUs", "trackedMetric","latLowPass","avgLat","avgLatPerByte", "power"});
        this->dcmSubsetRatio = 0.25; // Default value
}

StochasticPriorityHltsvStrategy::StochasticPriorityHltsvStrategy(double dcmQueueSelectionRatioParam, 
        int metricParam, int prioritySelectionTypeParam):
    StochasticPriorityHltsvStrategy()
    {
	printLog(LOG_LEVEL_INIT, "Instanciating StochasticStrategy. Metric[%d], Ratio[%f], priority[%d].", metricParam, dcmQueueSelectionRatioParam,  prioritySelectionTypeParam); 
        this -> USED_METRIC = static_cast<metrics>(metricParam);
        if(metricParam >= NUM_TYPES){
            printLog(LOG_LEVEL_ERROR, "Unrecognized Stochastic metricParam: %i.\n", metricParam);
            throw std::runtime_error("Unrecognized Stochastic metricParam.");
        }
        this->dcmSubsetRatio = dcmQueueSelectionRatioParam;
        this->priorityQueueSelectionType = static_cast<QueueSelectionType>(prioritySelectionTypeParam);
	printLog(LOG_LEVEL_IMPORTANT, "Configured with parameters: Metric=[%d], DcmSubsetRatio=[%f], PriorityQueueSelectionTye=[%d]\n", this->USED_METRIC, this->dcmSubsetRatio,  prioritySelectionTypeParam); 
    }

void StochasticPriorityHltsvStrategy::addPuToDcm(int puId, int dcmId, int rackId){
	auto existingDcmIt = this->findDcm(dcmId);

	std::shared_ptr<HltsvDcmLoad> dcmLoad;
	if(existingDcmIt == dcms.end()){ // does not exist
		auto rack = this->findRackOrCreate(rackId);

		// create a new HltsvDcmLoad
		dcmLoad = std::make_shared<HltsvDcmLoad>(dcmId, rack);

		// add it to the list
		dcms.push_back(dcmLoad);
	}else {
		dcmLoad = *existingDcmIt;
	}

	// add the PU
	dcmLoad->addPU(puId);
	this->totalIdlePus++;

	// update the warm up strategy
	this->warmupStrategy->addPuToDcm(puId, dcmId, rackId);
}

void StochasticPriorityHltsvStrategy::init(){
	printIdlePus(LOG_LEVEL_PRIORITY);
	this->pendingDcmsToStopWarmUp = this->dcms.size();
	this->warmupStrategy->init();
}

void StochasticPriorityHltsvStrategy::setIdlePu(int puId, DcmMeasuredStatistics stats ){
	int dcmId = NetworkConfiguration::getDcmIndex(puId);
	auto dcmIt = this->findDcm(dcmId);

	printLog(LOG_LEVEL_PRIORITY, "[MinLatency_setIdlePu] stats.getNetworkLatency=%f ; stats.getProcessingTime()=%f ; stats.getTotalFilteringTime()=%f \n", stats.getNetworkLatency(), stats.getProcessingTime(), stats.getTotalFilteringTime());

	// validation
	if(dcmIt == dcms.end()){
		printLog(LOG_LEVEL_ERROR, "removeBusyPuFromDcm: DCM %i (with PU #%i) was no previously in the system, PU can not be set idle : \n",  dcmId, puId);
		for(auto dcm : dcms){
			printLog(LOG_LEVEL_ERROR, "DCM id=%i \n", dcm->getDcmId());
		}
		throw "Invalid Operation: removeBusyPuFromDcm";
	}

	auto dcm = *dcmIt;

	// set PU as idle and update statistics
	dcm->markPUAsIdle(puId, stats);
	this->totalIdlePus++;
	printLog(LOG_LEVEL_PRIORITY, "[MinLatency_setIdlePu] PU=%u(DCM=%i) is now IDLE (measured--> net.latency=%f; procTime=%f;eventSize:%u ). DCM has %i busy PUs and trackedLatencty:%f  \n", puId, dcm->getDcmId(), stats.getNetworkLatency(), stats.getProcessingTime(), stats.getRequestedFragmentsSize(), dcm->countBusyPus(), dcm->getStatistics().getAverageLatency());

	// Check if we already received enough metrics
	if(this -> doWarmup &&
                dcm->getStatistics().getLatenciesCount() == METRICS_TO_STOP_WARM_UP){

		this->pendingDcmsToStopWarmUp--;

		printLog(LOG_LEVEL_PRIORITY, "[MinLatency_setIdlePu] pendingDcmsToStopWarmUp=%i \n", this->pendingDcmsToStopWarmUp);
		if(this->pendingDcmsToStopWarmUp == 0){
			this->doWarmup = false; // Stop the warm up period
			this->warmupStrategy.reset(); // remove it from memory
			printLog(LOG_LEVEL_IMPORTANT, "[MinLatency_setIdlePu] Finished Warm up period  \n");
		}
	}

	// update the warm up strategy
	if(this->warmupStrategy) this->warmupStrategy->setIdlePu(puId, stats);

	printLoad(LOG_LEVEL_PRIORITY);
        if(dcm->getDcmId() % 10 == 0)
            this->logMetrics(stats.getCurrentTime(), puId, dcm);
}

int StochasticPriorityHltsvStrategy::setNextPuBusy(double t) {
	printLog(LOG_LEVEL_PRIORITY, "[MinLatency_setNextPuBusy] \n");

	int nextPuId;

	if(this->doWarmup){
		nextPuId = this->warmupStrategy->setNextPuBusy(t);
		printLog(LOG_LEVEL_PRIORITY, "[MinLatency_setNextPuBusy] warmup strategy returned PUid=%i \n", nextPuId);
        } else {
            if(this->dcmsSubset.empty()){
                // calculate frequencies for the weighted random algorithm, using the tracked latencies
                std::vector<double> dcmIds;
                std::vector<double> dcmLatencies;
                for(auto dcmLoad : this->dcms){
                    if(dcmLoad->countIdlePus() <= 0) continue; // No Available PUs continue...

                    dcmIds.push_back(dcmLoad->getDcmId());
                    dcmLatencies.push_back( this -> getMetric(dcmLoad, this -> USED_METRIC));
                }

                // get a DCM using the weighted random algorithm based on the tracked latencies
                auto subset = this->priorityQueueSelectionType == WEIGHTED_SUBSET ?
                                    STDEVS::getConfigurationInstance()->getWeightedSubset(dcmIds,
                                            dcmLatencies, dcmIds.size() * this -> dcmSubsetRatio) :
                                    STDEVS::getConfigurationInstance()->getSortedByWeightSubset(dcmIds,
                                            dcmLatencies, dcmIds.size() * this -> dcmSubsetRatio) 
                    ;

                printLog(LOG_LEVEL_PRIORITY, "subset returned (size=%i):[", subset.size());
                for(auto dcmId : subset){
                    printLog(LOG_LEVEL_PRIORITY, "%i, ", dcmId);
                }
                printLog(LOG_LEVEL_PRIORITY, "] \n\n");

                this->dcmsSubset = std::deque<int>(subset.begin(), subset.end());
            }

            // get a DCM from the subset & remove it
            auto dcmId = this->dcmsSubset.front();
            this->dcmsSubset.pop_front();

            // find DCM
            auto dcm = (*this->findDcm(dcmId));

            // get the next puId
            nextPuId = dcm->getNextPuId();
        }

	int dcmId = NetworkConfiguration::getDcmIndex(nextPuId);
	auto dcm = (*this->findDcm(dcmId));
	dcm->markPUAsBusy(nextPuId);
	this->totalIdlePus--;
	printLog(LOG_LEVEL_PRIORITY, "[MinLatency_setNextPuBusy]: PU=%u(DCM=%i) is now BUSY. DCM has %i busy PUs and trackedLatencty:%f  \n", nextPuId, dcm->getDcmId(), dcm->countBusyPus(), dcm->getStatistics().getAverageLatency());

	printLoad(LOG_LEVEL_PRIORITY);
        if(dcm->getDcmId() % 10 == 0)
            this->logMetrics(t, nextPuId, dcm);


    return nextPuId;
}

double StochasticPriorityHltsvStrategy::getMetric(std::shared_ptr<HltsvDcmLoad>& dcmLoad, int metric){
    double return_metric;
    auto statistics = dcmLoad->getStatistics();
    double avgLatency = statistics.getAverageLatency();
    double power = statistics.getPower();
    printLog(LOG_LEVEL_PRIORITY, "Calculating metric %i, load: [%f] avgLat: [%f], power: [%f].\n",
            metric, statistics.getAverageLoad(),
            avgLatency, power);
    switch(metric) {
        case METRIC_MINLATENCY:
            {  
                return_metric = 1 / avgLatency;
                break;
            }
        case METRIC_POWER:
            {
                return_metric = power;
                break;
            }
        case METRIC_NORMALIZED_LATENCY:
            {
                return_metric = statistics.getAverageLatencyPerByte();
                break;
            }
        case METRIC_LOWPASS_FILTER:
            {
                return_metric = 1 / statistics.getLatencyLowPassFilter();
                break; 
            }
        default:
            {
                printLog(LOG_LEVEL_IMPORTANT, "Error while procesing metric, no default metric\n");
                break;
            }
    }
    return return_metric;
}
int StochasticPriorityHltsvStrategy::getIdleCores(){
	return this->totalIdlePus;
}
int countPrints =0;
void StochasticPriorityHltsvStrategy::printLoad(int logLevel){
	countPrints++;
	if(countPrints == 500){
		logLevel = 10000;
		countPrints = 0;
	}
	printLog(logLevel, "System Load: IdlePUs=%i \n", this->getIdleCores());

	printLog(logLevel, "DCM Ids :[");
	for(auto dcm : dcms){
		printLog(logLevel, "%i (%i), ", dcm->getDcmId(), dcm->getRack()->getRackId());
	}
	printLog(logLevel, "] \nDCM Load:[");

	for(auto dcm : dcms){
		printLog(logLevel, "%i: %i (%i), ", dcm->getDcmId(),dcm->countBusyPus(), dcm->getRack()->countBusyPUs());
	}
	printLog(logLevel, "] \n");

	// latencies
	printLog(logLevel, "DCM Run Latencies:[");
	for(auto dcm : dcms){
		printLog(logLevel, "%i: %f (%i), ", dcm->getDcmId(), dcm->getStatistics().getAverageLatency(), dcm->getStatistics().getLatenciesCount());
	}
	printLog(logLevel, "] \n");

	// subset of DCMs
	printLog(logLevel, "next subset of DCMs to be assigned (size=%i):[", this->dcmsSubset.size());
	for(auto dcmId : this->dcmsSubset){
		printLog(logLevel, "%i, ", dcmId);
	}
	printLog(logLevel, "] \n\n");
}
void StochasticPriorityHltsvStrategy::printIdlePus(int logLevel){
	printLog(logLevel, "System Idle Pus: IdlePUs=%i \n", this->getIdleCores());

	printLog(logLevel, "DCM Ids :[");
	for(auto dcm : dcms){
		printLog(logLevel, "%i (%i), ", dcm->getDcmId(), dcm->getRack()->getRackId());
	}
	printLog(logLevel, "] \nDCM Idle Pus:[");

	for(auto dcm : dcms){
		printLog(logLevel, "%i, ", dcm->countIdlePus());
	}
	printLog(logLevel, "] \n\n");
}

void StochasticPriorityHltsvStrategy::logMetrics(double t, int puId, std::shared_ptr<HltsvDcmLoad> dcm){
	this->logger->logSignal(t, dcm->getDcmId(),"dcmId");
	this->logger->logSignal(t, puId,"puId");
	this->logger->logSignal(t, dcm->countBusyPus(),"busyPUs");
	this->logger->logSignal(t, this->getMetric(dcm, USED_METRIC), "trackedMetric");
	this->logger->logSignal(t, dcm->getStatistics().getLatencyLowPassFilter(), "latLowPass");
	this->logger->logSignal(t, dcm->getStatistics().getAverageLatency(), "avgLat");
	this->logger->logSignal(t, dcm->getStatistics().getAverageLatencyPerByte(), "avgLatPerByte");
	this->logger->logSignal(t, dcm->getStatistics().getPower(), "power");
}


