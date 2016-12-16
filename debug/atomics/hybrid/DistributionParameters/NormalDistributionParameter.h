//CPP:hybrid/DistributionParameters/NormalDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_NormalDistributionParameter_H_
#define DistributionParameters_NormalDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "sinks/ParameterReader.h"

class NormalDistributionParameter : public BaseDistributionParameter {
#define PARAMETER_MU_NAME "mu"
#define PARAMETER_VAR_NAME "var"

public:
	NormalDistributionParameter();

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
};

#endif /* DistributionParameters_NormalDistributionParameter_H_ */
