//CPP:hybrid/DistributionParameters/ConstantDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_ConstantDistributionParameter_H_
#define DistributionParameters_ConstantDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "sinks/ParameterReader.h"

class ConstantDistributionParameter : public BaseDistributionParameter {
#define PARAMETER_VALUE_NAME "value"

public:
	ConstantDistributionParameter();

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
};

#endif /* DistributionParameters_ConstantDistributionParameter_H_ */
