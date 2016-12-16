#include "BaseDistributionParameter.h"

 BaseDistributionParameter::BaseDistributionParameter() { // Constructor
	 // use the configuration instance
	 this->randomNumberGenerator = STDEVS::getConfigurationInstance();

	 // Get parameter names from concrete class
	 // this->parameterNames = this->getParameterNames(); // Compiler does not allow to do it here :-(
}

double BaseDistributionParameter::nextValue() { // This indirection is in case we want to add: 1) Distributions which are evaluated only once (should call newRandomValue one time and then return always the same number)
	return this->getNewRandomValue();
}

void BaseDistributionParameter::readParameters(std::string distributionParamName){
	 // Get parameter names from concrete class
	this->parameterNames = this->getParameterNames(); // Compiler does not allow to do it here :-(

	// Read the values for each parameter
	for (std::string paramName : this->getParameterNames()){
		parameterValues.push_back(readDefaultParameterValue<double>(distributionParamName + "_" + paramName));
	}
}

std::string BaseDistributionParameter::getFullName() {
	std::string parameters = "";
	if(!this->parameterNames.empty()){
		parameters += this->parameterNames[0] + "=" + std::to_string(this->parameterValues[0]);
	}

	// Parameters
	for (uint i=1; i < this->parameterNames.size(); i++){
		parameters += "," + this->parameterNames[i] + "=" + std::to_string(this->parameterValues[i]);
	}

	std::string fullName = this->getName() + "(" + parameters + ")";
	return fullName;
}
