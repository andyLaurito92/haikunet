//CPP:ATLAS-TDAQ/HltsvStrategies/LeastBusyDcmHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/DcmMeasuredStatistics.cpp

#ifndef ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_LEAST_BUSY_DCM_H_
#define ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_LEAST_BUSY_DCM_H_

#include "pdevslib.h"
#include "ATLAS-TDAQ/globals.h"
#include "ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.h"

#include "ATLAS-TDAQ/HltsvStrategies/HltsvRackLoad.h"
#include "ATLAS-TDAQ/HltsvStrategies/HltsvDcmLoad.h"
#include "ATLAS-TDAQ/HltsvStrategies/DcmMeasuredStatistics.h"

#include "ATLAS-TDAQ/NetworkConfiguration.h"

#include <vector>
#include <queue>
#include <memory>

class LeastBusyDcmHltsvStrategy : public IHltsvStrategy {
public:

	LeastBusyDcmHltsvStrategy();

	void addPuToDcm(int puId, int dcmId, int rackId);
	void init();
	void setIdlePu(int puId, DcmMeasuredStatistics stat);
	int setNextPuBusy(double t) ;

	int getIdleCores();

private:
	// we use shared_ptr to avoid copying the full HltsvDcmLoad structure
	std::list<std::shared_ptr<HltsvDcmLoad>> dcms; // TODO: // a fixed sized-array would be more efficient.
	unsigned int totalIdlePus; // only used to return the count in O(1)

	void printLoad(int logLevel);

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
