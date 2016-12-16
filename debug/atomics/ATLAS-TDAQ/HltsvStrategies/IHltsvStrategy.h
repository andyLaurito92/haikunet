//CPP:ATLAS-TDAQ/HltsvStrategies/IHltsvStrategy.cpp

#ifndef I_HltsvStrategies_HLTSV_STRATEGY_H_
#define I_HltsvStrategies_HLTSV_STRATEGY_H_

#include <vector>
#include <queue>
#include <memory>

#include "ATLAS-TDAQ/HltsvStrategies/DcmMeasuredStatistics.h"


/**
 * Abstract class for to represent different HLTSV assignment strategies
 */
class IHltsvStrategy {
public:
	IHltsvStrategy() { }

	virtual ~IHltsvStrategy() { }

	virtual void addPuToDcm(int puId, int dcmId, int rackId) =0;
	virtual void init() =0;
	virtual void setIdlePu(int puId, DcmMeasuredStatistics stat) =0;
	virtual int setNextPuBusy(double t) =0;

	virtual int getIdleCores() =0;

};

#endif /* I_HltsvStrategies_HLTSV_STRATEGY_H_ */
