//CPP:ATLAS-TDAQ/HltsvStrategies/FifoHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.cpp

#ifndef ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_FIFO_STRATEGY_H_
#define ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_FIFO_STRATEGY_H_

#include "pdevslib.h"
#include "ATLAS-TDAQ/globals.h"
#include "ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.h"
#include "ATLAS-TDAQ/HltsvStrategies/DcmMeasuredStatistics.h"
#include "sinks/ParameterReader.h"

#include <vector>
#include <queue>
#include <memory>

class FifoHltsvStrategy : public IHltsvStrategy {
public:

	FifoHltsvStrategy();

	void addPuToDcm(int puId, int dcmId, int rackId);
	void init();
	void setIdlePu(int puId, DcmMeasuredStatistics stat);
	int setNextPuBusy(double t) ;

	int getIdleCores();

private:
	std::deque<int> idlePUIds; // used for RANDOM and FIFO
};

#endif /* ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_RANDOM_STRATEGY_H_ */
