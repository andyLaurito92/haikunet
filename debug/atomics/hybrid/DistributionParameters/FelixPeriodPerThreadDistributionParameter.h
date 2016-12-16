//CPP:hybrid/DistributionParameters/FelixPeriodPerThreadDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_FelixPeriodPerThreadDistributionParameter_H_
#define DistributionParameters_FelixPeriodPerThreadDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "hybrid/DistributionParameters/IDistributionParameter.h"
#include "sinks/ParameterReader.h"

/**
 * Specific domin distribution (CERN-TDAQ). TODO: ¿how to move it to a model-specific location? (need to be included in the ParameterReader.h for all models)
 * This is a distribution to simulate the period of packet generation of Felix servers: see: https://twiki.cern.ch/twiki/bin/view/Main/Simulation_meetings_jornFelixCharacterization
 *
 */
class FelixPeriodPerThreadDistributionParameter : public BaseDistributionParameter {

// obligatory parameters
#define PARAMETER_PERIOD_NAME "period"            // distribution of the message period arriving at the GBT link
#define PARAMETER_MODE_NAME "mode"            // FELIX_MODE_LOW_LATENCY = 0 or FELIX_MODE_HIGH_THROUGHOUT = 1

// optional parameters
#define PARAMETER_SIZE_NAME "size_bytes"      // distribution of the message size arriving at the GBT link
#define PARAMETER_BUFFER_NAME "buffer_bytes"  // buffer size at the felix server per thread
#define PARAMETER_TIMEOUT_NAME "timeout"      // maximum time to empty buffer
#define PARAMETER_OUTPUT_SIZE_NAME "out_size_bytes" // size of each packet to be sent out (to calculate how many are needed everytime the buffer is emptied)


public:
	enum FELIX_MODE { FELIX_MODE_LOW_LATENCY = 0, FELIX_MODE_HIGH_THROUGHPUT = 1};

protected:
	// parameters
	std::shared_ptr<IDistributionParameter> periodDistribution;
	FELIX_MODE mode;
	std::shared_ptr<IDistributionParameter> sizeDistribution;

	double getBuffer_bytes() { return this->parameterValues[3]; } ;
	double getTimeout() { return this->parameterValues[4]; } ;
	double getOutSize_bytes() { return this->parameterValues[5]; } ;

public:
	FelixPeriodPerThreadDistributionParameter();

	/*override from BaseDistributionParameter*/ void readParameters(std::string distributionParamName);

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
private:
	int timesToEmptyBuffer = 0;

	double getNextPeriodForHighThroughout();
};

#endif /* DistributionParameters_FelixPeriodPerThreadDistributionParameter_H_ */
