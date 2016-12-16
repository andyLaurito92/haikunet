//CPP:hybrid/DistributionParameters/IDistributionParameter.cpp
#ifndef DistributionParameters_IDistributionParameter_H_
#define DistributionParameters_IDistributionParameter_H_

#include <queue>
#include <memory>

/**
 * Abstract class for to distributions specified in parameter files
 * To add a new Distribution parameters:
 *   - Implement it using BaseDistribution as parent. Look at ExponentialDistributionParameter as an example
 *   - Add it to ParameterReader.h and .cpp so that it can be read from parameters file
 *   - Add it to the //CPP section of SimulationExperimentTracker.h, so that it is included in PowerDEVS compilation (recompile from PowerDEVS GUI needed)
 *   - Add the constant to the scilab params
 *
 */
class IDistributionParameter {
public:
	IDistributionParameter() { }

	virtual ~IDistributionParameter() { }

	// returns the value according the the specified distribution;
	virtual double nextValue()=0;

	// returns the value according the the specified distribution;
    virtual void readParameters(std::string paramName)=0;

	// returns the distribution name (for printing)
	virtual std::string getName()=0;

	// returns the distribution name (including parameters) to be printed
	virtual std::string getFullName()=0;
};

#endif /* DistributionParameters_IDistributionParameter_H_ */
