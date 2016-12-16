#include "FelixPeriodPerThreadDistributionParameter.h"

FelixPeriodPerThreadDistributionParameter::FelixPeriodPerThreadDistributionParameter() : BaseDistributionParameter() {
	printLog(LOG_LEVEL_DEBUG, "Using a FelixPeriodPerThreadDistributionParameter \n");
}

double FelixPeriodPerThreadDistributionParameter::getNewRandomValue() {
	// low latency: just forwards according to period distributtion, no buffering
	if(this->mode == FELIX_MODE_LOW_LATENCY){
		return this->periodDistribution->nextValue();
	}

	// high-throughput: calculate when the buffer will get full
	return getNextPeriodForHighThroughout();

}

double FelixPeriodPerThreadDistributionParameter::getNextPeriodForHighThroughout(){
	if(this->timesToEmptyBuffer != 0 ){ // we still need to empty buffer
		this->timesToEmptyBuffer--;
		return 0; // with no delay => this generates several outputs in the same t
	}

    // 'simulate' when the buffer will get full
	double time = 0;
	double currentBufferSize = 0;
	while(currentBufferSize < this->getBuffer_bytes() && time < this->getTimeout()){ // TODO: this might have performance issues (specially if buffer>> and timeout >> )
		// next GBT message will arrive in 'periodDistribution->nextValue()' with size 'sizeDistribution->nextValue()'
		time += this->periodDistribution->nextValue();
		currentBufferSize  += this->sizeDistribution->nextValue();
	}

	this->timesToEmptyBuffer = (currentBufferSize + this->getOutSize_bytes() - 1) / this->getOutSize_bytes();   // (rounds up buffer/outSize): http://www.cs.nott.ac.uk/~rcb/G51MPC/slides/NumberLogic.pdf
	this->timesToEmptyBuffer--; // we are now sending the first, so substract 1

	return time;
}

void FelixPeriodPerThreadDistributionParameter::readParameters(std::string distributionParamName){
	BaseDistributionParameter::readParameters(distributionParamName);

	// validate mode
	if(this->parameterValues[1] != FELIX_MODE_LOW_LATENCY && this->parameterValues[1] != FELIX_MODE_HIGH_THROUGHPUT){
		throw std::runtime_error("FelixPeriodPerThreadDistributionParameter:: '" + distributionParamName + "_" + this->parameterNames[1] + "' was set incorrectly. Only accepts {FELIX_MODE_LOW_LATENCY, FELIX_MODE_HIGH_THROUGHOUT} \n");
	}
	this->mode = static_cast<FELIX_MODE>(this->parameterValues[1]);
	this->periodDistribution = readDistributionParameter(distributionParamName + "_" + PARAMETER_PERIOD_NAME);

	// in high throughput mode, more parameters are required. If not return
	if(this->mode == FELIX_MODE_LOW_LATENCY) return;

	// Read the values for each parameter (in the case of mode == FELIX_MODE_HIGH_THROUGHOUT)
	auto otherParams = std::vector<std::string> {PARAMETER_SIZE_NAME, PARAMETER_BUFFER_NAME, PARAMETER_TIMEOUT_NAME, PARAMETER_OUTPUT_SIZE_NAME};
	for (std::string paramName : otherParams){
		this->parameterNames.push_back(paramName);
		this->parameterValues.push_back(readDefaultParameterValue<double>(distributionParamName + "_" + paramName));
	}

	// now read distributions
	this->sizeDistribution = readDistributionParameter(distributionParamName + "_" + PARAMETER_SIZE_NAME);
}

std::vector<std::string> FelixPeriodPerThreadDistributionParameter::getParameterNames(){
	// only obligatory parameters here
	return std::vector<std::string> { PARAMETER_PERIOD_NAME, PARAMETER_MODE_NAME} ;
};

std::string FelixPeriodPerThreadDistributionParameter::getName() {
	return std::string("FelixGBTLink");
}
