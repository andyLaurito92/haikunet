#ifndef ATOMICS_ATLAS_TDAQ_HltsvStrategies_DcmMeasuredStatistics_H_
#define ATOMICS_ATLAS_TDAQ_HltsvStrategies_DcmMeasuredStatistics_H_

#include "simulator.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "ATLAS-TDAQ/globals.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <list>

class DcmMeasuredStatistics {
	double currentTime;
	double totalFilteringTime;
	double processingTime;
	double networkLatency;
	unsigned int requestedFragmentsSize;

public:
	DcmMeasuredStatistics(double currentTime, double totalFilteringTime, double processingTime, double networkLatency, unsigned int requestedFragmentsSize) :
		currentTime(currentTime),
		totalFilteringTime(totalFilteringTime),
		processingTime(processingTime),
		networkLatency(networkLatency),
		requestedFragmentsSize(requestedFragmentsSize){ };

	double getCurrentTime() { return this->currentTime;};
	double getTotalFilteringTime() { return this->totalFilteringTime;};
	double getProcessingTime() { return this->processingTime;};
	double getNetworkLatency() { return this->networkLatency;};
	unsigned int getRequestedFragmentsSize() { return this->requestedFragmentsSize;};
};

#endif /* ATOMICS_ATLAS_TDAQ_HltsvStrategies_DcmMeasuredStatistics_H_ */
