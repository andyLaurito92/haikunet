#include "ParetoDistributionParameter.h"

ParetoDistributionParameter::ParetoDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a ParetoDistributionParameter \n");
}

double ParetoDistributionParameter::getNewRandomValue() {
	return this->getRandomNumberGenerator()->pareto(this->parameterValues[0]/*shape*/, this->parameterValues[1]/*shape*/);
}

std::vector<std::string> ParetoDistributionParameter::getParameterNames(){
	return std::vector<std::string> { PARAMETER_SHAPE_NAME, PARAMETER_SCALE_NAME } ;
};

std::string ParetoDistributionParameter::getName() {
	return std::string("Pareto");
}
