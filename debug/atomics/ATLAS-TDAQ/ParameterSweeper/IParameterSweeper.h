//CPP:ATLAS-TDAQ/ParameterSweeper/IParameterSweeper.cpp

/*
 * IParameterSweeper.h
 *
 *  Created on: Jun 3, 2015
 *      Author: mbonaven
 */

#ifndef I_PARAMETER_SWEEPER_H_
#define I_PARAMETER_SWEEPER_H_


/**
 * Abstract class for Parameter Sweepers. Known implementations: ScilabParameterSweeper, CmdLineParameterSweeper
 */
class IParameterSweeper {
public:
	IParameterSweeper(std::string simuRunVariableName, std::string parameterValuesVariableName, std::string parameterNamesVariableName) :
		simuRunVariableName(simuRunVariableName),
		parameterValuesVariableName(parameterValuesVariableName),
		parameterNamesVariableName(parameterNamesVariableName){ }

	virtual ~IParameterSweeper() { }

	/*
	 * Returns current run number based on the name for simuRunVariableName
	 */
	virtual int getCurrentExperimentNumber()=0;

	/*
	 * Returns total number of runs
	 */
	virtual int getTotalNumberOfExperiments()=0;

	/*
	 * Updates the parameters according to the currentExperimentNumber, parameterValuesVariableName and parameterNamesVariableName
	 */
	virtual void UpdateParameters()=0;

	virtual void onExit()=0;
protected:
	std::string simuRunVariableName, parameterValuesVariableName, parameterNamesVariableName;
};

#endif /* IParameterSweeper_H_ */
