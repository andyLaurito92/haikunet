/*
 * CmdLineParameterSweeper.cpp
 *
 *  Created on: Jul 7, 2015
 *      Author: mbonaven
 */

#include "CmdLineParameterSweeper.h"


int CmdLineParameterSweeper::getCurrentExperimentNumber(){
	// Update current simulation run based on the cmdLine parameter
	return getCmdLineParam<int>(simuRunVariableName);
}

int CmdLineParameterSweeper::getTotalNumberOfExperiments(){
	printLog(LOG_LEVEL_IMPORTANT, "Reading ExperimentNumber from Command Line + Configuration file. \n");
	printLog(LOG_LEVEL_FULL_LOGGING, "[INIT] CmdLineParameterSweeper getTotalNumberOfRuns \n");

	// retrieve the vector of parameters to sweep
	std::vector<double> parameterValues = getCmdLineParam<std::vector<double>>(parameterValuesVariableName);
	std::vector<std::string> parameterNames = getCmdLineParam<std::vector<std::string>>(parameterNamesVariableName);

	if(parameterNames.size() != 1){
		printLog(LOG_LEVEL_ERROR, "CmdLineParameterSweeper: Sweeping more than 1 parameter is not supported (no support to define matrices in configuration). %s is configured to sweep %i variables  \n", parameterNamesVariableName.data(), parameterNames.size());
	}

	// the total simulation runs is equal to the amout of parameters defined
	return parameterValues.size();
}


void CmdLineParameterSweeper::UpdateParameters(){
	printLog(LOG_LEVEL_FULL_LOGGING, "[INIT] CmdLineParameterSweeper UpdateParameters \n");

	// retrieve the vector of parameters to sweep
	std::vector<std::string> parameterValues = getCmdLineParam<std::vector<std::string>>(parameterValuesVariableName);
	std::vector<std::string> parameterNames = getCmdLineParam<std::vector<std::string>>(parameterNamesVariableName);

	if(parameterNames.size() != 1){
		printLog(LOG_LEVEL_ERROR, "CmdLineParameterSweeper: Sweeping more than 1 parameter is not supported (no support to define matrices in configuration). %s is configured to sweep %i variables  \n", parameterNamesVariableName.data(), parameterNames.size());
	}

	// update parameters
	printLog(LOG_LEVEL_IMPORTANT, "CmdLineParameterSweeper: CURRENT simulation will use the following parameters: \n");
	for (unsigned int i = 0; i < parameterNames.size(); i++) {
		// Update parameter i
		this->replaceParameterValue(parameterNames.at(i),parameterValues.at(this->getCurrentExperimentNumber()));
		printLog(LOG_LEVEL_IMPORTANT, "          %s=%s\n", parameterNames[i].data(), parameterValues[this->getCurrentExperimentNumber()].data());
	}

}

void CmdLineParameterSweeper::replaceParameterValue(std::string parameterName, std::string parameterValue){
	std::string optionPrefix = "-" + parameterName;

	// remove previous declaration of the parameter
	CmdLineModelParams.erase(std::remove_if(CmdLineModelParams.begin(), CmdLineModelParams.end(),
			[optionPrefix](const std::string & cmdLineOption) {
					return !cmdLineOption.compare(0, optionPrefix.size(), optionPrefix);
			}),
			CmdLineModelParams.end());

	// add the new value for the parameter
	CmdLineModelParams.push_back(std::string("-") + parameterName + std::string("=") + std::string(parameterValue));
}

void CmdLineParameterSweeper::onExit(){
	printLog(LOG_LEVEL_FULL_LOGGING, "[INIT] CmdLineParameterSweeper onExit \n");
}

