//CPP:hybrid/DistributionParameters/ParetoDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_ParetoDistributionParameter_H_
#define DistributionParameters_ParetoDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "sinks/ParameterReader.h"

class ParetoDistributionParameter : public BaseDistributionParameter {
#define PARAMETER_VALUE1_NAME "val1"
#define PARAMETER_VALUE2_NAME "val2"
#define PARAMETER_MEAN_NAME "mean"

public:
	ParetoDistributionParameter();

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
};

#endif /* DistributionParameters_ParetoDistributionParameter_H_ */
