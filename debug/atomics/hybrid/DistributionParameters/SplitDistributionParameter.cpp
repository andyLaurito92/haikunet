#include "SplitDistributionParameter.h"

SplitDistributionParameter::SplitDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a SplitDistributionParameter \n");
}

double SplitDistributionParameter::getNewRandomValue() {
	double value1 = this->parameterValues[0];
	double value2 = this->parameterValues[1];
	double mean = this->parameterValues[2];
	double probability1 = (mean - value2) / (value1 - value2); // TODO: this can be calculated only once for better performance

	double random = this->getRandomNumberGenerator()->bernoulli(probability1);

	//printLog(1, "The random value was %f, with probability %f \n", random, probability1);

	if(random == 0) return value2;
	if(random == 1) return value1;

	throw std::runtime_error("Bernulli distribution return something different from 0 or 1");
}

std::vector<std::string> SplitDistributionParameter::getParameterNames(){
	return std::vector<std::string> { PARAMETER_VALUE1_NAME, PARAMETER_VALUE2_NAME, PARAMETER_MEAN_NAME } ;
};

std::string SplitDistributionParameter::getName() {
	return std::string("Split");
}
