//CPP:ATLAS-TDAQ/HltsvStrategies/StochasticPriorityHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/HltsvDcmLoad.cpp

#ifndef ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_MIN_LANTENCY_DCM_H_
#define ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_MIN_LANTENCY_DCM_H_

#include "pdevslib.h"
#include "hybrid/stdevstool.h"
#include "ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.h"

#include "ATLAS-TDAQ/HltsvStrategies/HltsvRackLoad.h"
#include "ATLAS-TDAQ/HltsvStrategies/HltsvDcmLoad.h"
#include "ATLAS-TDAQ/HltsvStrategies/RandomRoundRobinHltsvStrategy.h"

#include "ATLAS-TDAQ/NetworkConfiguration.h"
#include "ATLAS-TDAQ/HltsvStrategies/DcmMeasuredStatistics.h"

#include <vector>
#include <deque>
#include <queue>
#include <memory>

class StochasticPriorityHltsvStrategy : public IHltsvStrategy {

// Minimum amount of metrics which each DCM should have in order to stop the warmup period
#define METRICS_TO_STOP_WARM_UP 1
#define SUBSET_RATIO 0.25



public:
	StochasticPriorityHltsvStrategy();
	StochasticPriorityHltsvStrategy(double, int, int);
        ~StochasticPriorityHltsvStrategy(){
            this->logger->flush(INF);
            this->logger.reset();
        }

	void addPuToDcm(int puId, int dcmId, int rackId);
	void init();
	void setIdlePu(int puId, DcmMeasuredStatistics stat);
	int setNextPuBusy(double t) ;

        double dcmSubsetRatio; 
	int getIdleCores();

        enum metrics{
            METRIC_MINLATENCY,
            METRIC_POWER,
            METRIC_NORMALIZED_LATENCY,
            METRIC_LOWPASS_FILTER,
            NUM_TYPES 
        };
        metrics USED_METRIC;

        enum QueueSelectionType{ 
            WEIGHTED_SUBSET,
            ORDERED_SUBSET
        };
        QueueSelectionType priorityQueueSelectionType;

private:
	// HELPERS
	std::shared_ptr<IPowerDEVSLogger> logger;


	// we use shared_ptr to avoid copying the full HltsvDcmLoad structure
	std::list<std::shared_ptr<HltsvDcmLoad>> dcms; // TODO: // a fixed sized-array would be more efficient.
	std::deque<int> dcmsSubset; // dcms to be assigned in the following round
	unsigned int totalIdlePus; // only used to return the count in O(1)

	// to track the warmup period
	bool doWarmup;
	unsigned int pendingDcmsToStopWarmUp;
	std::shared_ptr<RandomRoundRobinHltsvStrategy> warmupStrategy;
        double getMetric(std::shared_ptr<HltsvDcmLoad>& dcmLoad, int metric);

	void printLoad(int logLevel);
	void printIdlePus(int logLevel);
	void logMetrics(double t, int puId, std::shared_ptr<HltsvDcmLoad> dcm);

	std::list<std::shared_ptr<HltsvDcmLoad>>::iterator findDcm(int dcmId){
		return std::find_if(dcms.begin(), dcms.end(), [&](std::shared_ptr<HltsvDcmLoad> const& p) {
			return p->getDcmId() == dcmId;
		});
	}

	std::shared_ptr<HltsvRackLoad> findRackOrCreate(int rackId){
		auto dcmIt = std::find_if(dcms.begin(), dcms.end(), [&](std::shared_ptr<HltsvDcmLoad> const& p) {
			return p->getRack()->getRackId() == rackId;
		});

		if(dcmIt == dcms.end()) return std::make_shared<HltsvRackLoad>(rackId);
		else return (*dcmIt)->getRack();
	}


	// TODO: do this with a comparator, so that the comparator can be passed as parameter to the class
	static bool LessBusyDcm(const std::shared_ptr<HltsvDcmLoad>& lhs, const std::shared_ptr<HltsvDcmLoad>& rhs) {
		//				  if(lhs->countIdlePus() != rhs->countIdlePus())
		return lhs->countBusyPus() < rhs->countBusyPus();
		//				  else
		//					  return lhs->getRack()->countBusyPUs() < rhs->getRack()->countBusyPUs();
	}
};

#endif /* ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_LEAST_BUSY_DCM_H_ */
