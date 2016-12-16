#include "NormalDistributionParameter.h"

NormalDistributionParameter::NormalDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a NormalDistributionParameter \n");
}

double NormalDistributionParameter::getNewRandomValue() {
	return this->getRandomNumberGenerator()->normal(this->parameterValues[0]/*mu*/, this->parameterValues[1]/*var*/);
}

std::vector<std::string> NormalDistributionParameter::getParameterNames(){
	return std::vector<std::string> { PARAMETER_MU_NAME, PARAMETER_VAR_NAME } ;
};

std::string NormalDistributionParameter::getName() {
	return std::string("Normal");
}
