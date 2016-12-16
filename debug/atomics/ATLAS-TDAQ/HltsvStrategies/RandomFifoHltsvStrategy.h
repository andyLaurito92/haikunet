//CPP:ATLAS-TDAQ/HltsvStrategies/RandomFifoHltsvStrategy.cpp
//CPP:ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.cpp

#ifndef ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_RANDOM_FIFO_STRATEGY_H_
#define ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_RANDOM_FIFO_STRATEGY_H_

#include "pdevslib.h"
#include "ATLAS-TDAQ/globals.h"
#include "ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.h"
#include "ATLAS-TDAQ/HltsvStrategies/DcmMeasuredStatistics.h"
#include "sinks/ParameterReader.h"

#include <vector>
#include <queue>
#include <memory>

/*
 * Applies a FIFO strategy, but making a random of the PUs at the begging (to simulate the stabilization of the algorithm)
 */
class RandomFifoHltsvStrategy : public IHltsvStrategy {
public:

	RandomFifoHltsvStrategy();

	void addPuToDcm(int puId, int dcmId, int rackId);
	void init();
	void setIdlePu(int puId, DcmMeasuredStatistics stat);
	int setNextPuBusy(double t) ;

	int getIdleCores();

private:
	std::deque<int> idlePUIds;
};

#endif /* ATOMICS_ATLAS_TDAQ_HLTSV_STRATEGIES_RANDOM_FIFO_STRATEGY_H_ */
