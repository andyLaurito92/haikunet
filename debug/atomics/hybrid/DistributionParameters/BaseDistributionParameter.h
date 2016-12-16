//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
#ifndef DistributionParameters_BaseDistributionParameter_H_
#define DistributionParameters_BaseDistributionParameter_H_


#include "hybrid/DistributionParameters/IDistributionParameter.h"
#include "hybrid/stdevstool.h"
/* forward declare the reference */
//class STDEVS;

//* Forward declare the functions in ParameterReader.h. Including ParameterReader.h creates circular inclusion problems: BaseDistri -> ParamReader -> ConstDistri -> BaseDistri*/
//#include "sinks/ParameterReader.h"
void printLog(int level, const char *fmt,...);
template<typename T> T readDefaultParameterValue(std::string paramName);


class BaseDistributionParameter : public IDistributionParameter {

protected:
	virtual double getNewRandomValue()=0;
	virtual std::vector<std::string> getParameterNames()=0;
	virtual std::string getName()=0;
	STDEVS* randomNumberGenerator;

	std::vector<std::string> parameterNames;
	std::vector<double> parameterValues;

	STDEVS* getRandomNumberGenerator() { return this->randomNumberGenerator; }

public:
	BaseDistributionParameter();

	double nextValue();// This indirection is in case we want to add: 1) Distributions which are evaluated only once (should call newRandomValue one time and then return always the same number)
	void readParameters(std::string distributionParamName);
	std::string getFullName();
};

#endif /* DistributionParameters_IDistributionParameter_H_ */
