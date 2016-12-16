//CPP:sinks/ParameterSweeper/CmdLineParameterSweeper.cpp

#ifndef ATOMICS_ATLAS_TDAQ_PARAMETERSWEEPER_CMDLINEPARAMETERSWEEPER_H_
#define ATOMICS_ATLAS_TDAQ_PARAMETERSWEEPER_CMDLINEPARAMETERSWEEPER_H_

#include "pdevslib.h"
#include "sinks/ParameterReader.h"
#include "sinks/ParameterSweeper/IParameterSweeper.h"

class CmdLineParameterSweeper : public IParameterSweeper {
public:
	CmdLineParameterSweeper(std::string simuRunVariableName, std::string parameterValuesVariableName, std::string parameterNamesVariableName) : IParameterSweeper(simuRunVariableName, parameterValuesVariableName, parameterNamesVariableName) {
//		printLog(LOG_LEVEL_FULL_LOGGING, "[INIT] CmdLineParameterSweeper constructor \n");
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

#endif /* ATOMICS_ATLAS_TDAQ_PARAMETERSWEEPER_CMDLINEPARAMETERSWEEPER_H_ */
