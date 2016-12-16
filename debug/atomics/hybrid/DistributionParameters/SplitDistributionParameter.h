//CPP:hybrid/DistributionParameters/SplitDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_SplitDistributionParameter_H_
#define DistributionParameters_SplitDistributionParameter_H_

#include "hybrid/DistributionParameters/BaseDistributionParameter.h"
#include "sinks/ParameterReader.h"

class SplitDistributionParameter : public BaseDistributionParameter {
#define PARAMETER_SHAPE_NAME "shape"
#define PARAMETER_SCALE_NAME "scale"

public:
	SplitDistributionParameter();

	double getNewRandomValue();
	std::vector<std::string> getParameterNames();
	std::string getName();
};

#endif /* DistributionParameters_SplitDistributionParameter_H_ */
