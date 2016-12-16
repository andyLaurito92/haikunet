#include "ExponentialDistributionParameter.h"

ExponentialDistributionParameter::ExponentialDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a ExponentialDistributionParameter \n");
}

ExponentialDistributionParameter::ExponentialDistributionParameter(double mu) : ExponentialDistributionParameter(){
	this->parameterNames = this->getParameterNames();
	this->parameterValues.push_back(mu);
}

double ExponentialDistributionParameter::getNewRandomValue() {
	return this->getRandomNumberGenerator()->exponential(this->parameterValues[0]);
}

std::vector<std::string> ExponentialDistributionParameter::getParameterNames(){
	return std::vector<std::string> { PARAMETER_MU_NAME } ;
};

std::string ExponentialDistributionParameter::getName() {
	return std::string("Exponential");
}
