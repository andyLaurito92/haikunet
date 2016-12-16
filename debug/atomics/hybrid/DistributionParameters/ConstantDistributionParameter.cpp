#include "ConstantDistributionParameter.h"

ConstantDistributionParameter::ConstantDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a ConstantDistributionParameter \n");
}

double ConstantDistributionParameter::getNewRandomValue() {
	return this->parameterValues[0];
}

std::vector<std::string> ConstantDistributionParameter::getParameterNames(){
	return std::vector<std::string> { PARAMETER_VALUE_NAME } ;
};

std::string ConstantDistributionParameter::getName() {
	return std::string("Constant");
}
