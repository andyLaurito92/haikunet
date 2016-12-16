//CPP:ATLAS-TDAQ/ParameterSweeper/ScilabParameterSweeper.cpp

#ifndef ATOMICS_ATLAS_TDAQ_PARAMETERSWEEPER_ScilabParameterSweeper_H_
#define ATOMICS_ATLAS_TDAQ_PARAMETERSWEEPER_ScilabParameterSweeper_H_

#include "pdevslib.h"
#include "ATLAS-TDAQ/globals.h"
#include "ATLAS-TDAQ/ParameterSweeper/IParameterSweeper.h"
#include "ATLAS-TDAQ/SimulationExperimentTracker.h"

/**
 * TODO: unimplemented class. To implement it you should base on ScilabSimulationTracker
 */
class ScilabParameterSweeper : public IParameterSweeper {
public:
	ScilabParameterSweeper(std::string simuRunVariableName, std::string parameterValuesVariableName, std::string parameterNamesVariableName) :
		IParameterSweeper(simuRunVariableName, parameterValuesVariableName, parameterNamesVariableName) {
//		printLog(LOG_LEVEL_FULL_LOGGING, "ScilabParameterSweeper: unimplemented class. To implement it you should base on ScilabSimulationTracker\n");
//		throw std::runtime_error("unimplemented class. To implement it you should base on ScilabSimulationTracker");
	}

	/*
	 * Returns current run number based on the name for simuRunVariableName
	 */
	int getCurrentExperimentNumber();

	/*
	 * Returns total number of runs
	 */
	int getTotalNumberOfExperiments();

	/*
	 * Updates the parameters according to the currentExperimentNumber, parameterValuesVariableName and parameterNamesVariableName
	 */
	void UpdateParameters();

	void onExit();
private:
	void replaceParameterValue(std::string parameterName, std::string parameterValue);

};

#endif /* ATOMICS_ATLAS_TDAQ_PARAMETERSWEEPER_ScilabParameterSweeper_H_ */
