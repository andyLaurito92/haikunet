//CPP:hybrid/DistributionParameters/ExponentialDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_ExponentialDistributionParameter_H_
#define DistributionParameters_ExponentialDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "sinks/ParameterReader.h"

class ExponentialDistributionParameter : public BaseDistributionParameter {
#define PARAMETER_MU_NAME "mu"

public:
	ExponentialDistributionParameter();
	ExponentialDistributionParameter(double mu);

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
};

#endif /* DistributionParameters_ExponentialDistributionParameter_H_ */
