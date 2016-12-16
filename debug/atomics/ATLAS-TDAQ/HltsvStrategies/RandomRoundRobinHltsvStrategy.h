//CPP:ATLAS-TDAQ/HltsvStrategies/RandomRoundRobinHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/HltsvDcmLoad.cpp

#ifndef ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_RANDOM_ROUND_ROBIN_LANTENCY_DCM_H_
#define ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_RANDOM_ROUND_ROBIN_LANTENCY_DCM_H_

#include "pdevslib.h"
#include "hybrid/stdevstool.h"


#include "ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.h"

#include "ATLAS-TDAQ/HltsvStrategies/HltsvRackLoad.h"
#include "ATLAS-TDAQ/HltsvStrategies/HltsvDcmLoad.h"

#include "ATLAS-TDAQ/NetworkConfiguration.h"
#include "ATLAS-TDAQ/HltsvStrategies/DcmMeasuredStatistics.h"

#include <vector>
#include <queue>
#include <memory>
#include <algorithm>    // std::random_shuffle

#include "sinks/ParameterReader.h"

class RandomRoundRobinHltsvStrategy : public IHltsvStrategy {
public:

	RandomRoundRobinHltsvStrategy();

	void addPuToDcm(int puId, int dcmId, int rackId);
	void init();
	void setIdlePu(int puId, DcmMeasuredStatistics stat);
	int setNextPuBusy(double t) ;

	int getIdleCores();

private:
	std::vector<std::shared_ptr<HltsvDcmLoad>> dcms; // TODO: // a fixed sized-array would be more efficient.
	unsigned int totalIdlePus; // only used to return the count in O(1)

	unsigned int nextDcmIndex;

	/// HELPERS
	std::vector<std::shared_ptr<HltsvDcmLoad>>::iterator findDcm(int dcmId){
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

};

#endif /* ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_RANDOM_ROUND_ROBIN_LANTENCY_DCM_H_ */
