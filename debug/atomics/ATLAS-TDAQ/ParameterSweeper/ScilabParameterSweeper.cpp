/*
 * ScilabParameterSweeper.cpp
 *
 *  Created on: Jul 7, 2015
 *      Author: mbonaven
 */

#include "ScilabParameterSweeper.h"

int ScilabParameterSweeper::getCurrentExperimentNumber(){
	printLog(LOG_LEVEL_FULL_LOGGING, "Reading ExperimentNumber from Scilab \n");

	// update Scilab working directory to match the simulation execution file
	char command[1024];
	char* path = getExecutablePath();
	sprintf(command,"cd '%s'; printf('working directory is now %s \n') ",path, path);
	executeVoidScilabJob(command,true);

	printLog(LOG_LEVEL_FULL_LOGGING, "[INIT] executed: %s \n", command);

	// get run number
	int experimentNumber;
	try {
		experimentNumber = (double) getScilabVar(this->simuRunVariableName.data());
	} catch (...) {
		printLog(LOG_LEVEL_FULL_LOGGING, "[INIT] ScilabParameterSweeper: (WARNING) Scilab variable %s was not declared. Variable forced to 0  \n", this->simuRunVariableName.data());
		putScilabVar(this->simuRunVariableName.data(), 0);
		experimentNumber = (double) getScilabVar(this->simuRunVariableName.data());
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[INIT] ScilabParameterSweeper: Retrieved ExperimentNumber=%i  \n", experimentNumber);
	return experimentNumber;
}

int ScilabParameterSweeper::getTotalNumberOfExperiments(){
	char command[1024];
	double *parametersMeta = new double [2];
	int rows=1, cols=2;

	// get amount of parameters
	sprintf(command,"size(%s);", parameterValuesVariableName.data());
	printLog(LOG_LEVEL_FULL_LOGGING, "command=%s \n", command);
	getScilabVector(command, &cols, parametersMeta );

	printLog(LOG_LEVEL_FULL_LOGGING, "Params done \n");
	rows = parametersMeta[0];
	cols = parametersMeta[1];
	return cols;
}


void ScilabParameterSweeper::UpdateParameters(){
	// TODO: Retrieve parameter names from scilab. Problem now is how to retrieve string matrices.
	char* parameterNames[20];
	//parameterNames[0] = "l1EventRate";
	//parameterNames[0] = "nRequestCredits";
	//parameterNames[0] = "l2Requests";
	//parameterNames[0] = "nDCM";
	parameterNames[0] = "hltsvAlgorithm";

	// Second parameter
	//parameterNames[1] = "l1EventRate";


	char command[1024];
	double *parametersMeta = new double [2];
	int rows=1, cols=2;

	// get amount of parameters
	sprintf(command,"size(%s);", parameterValuesVariableName.data());
	printLog(LOG_LEVEL_FULL_LOGGING, "command=%s \n", command);
	getScilabVector(command, &cols, parametersMeta );

	printLog(LOG_LEVEL_FULL_LOGGING, "Params done \n");
	rows = parametersMeta[0];
	cols = parametersMeta[1];
	printLog(LOG_LEVEL_IMPORTANT, "[INIT] ScilabParameterSweeper: There are %g parameters defined with %g values each. \n",  parametersMeta[0], parametersMeta[1]);

	// Get Parameter values matrix from scilab
	double **simParameters = new double* [rows];
	for (int i = 0; i < rows; i++)
		simParameters[i] = new double[cols];

	getScilabMatrix(parameterValuesVariableName.data(), &rows, &cols, simParameters);

	printLog(LOG_LEVEL_IMPORTANT, "[INIT] ScilabParameterSweeper: CURRENT simulation will use the following parameters: \n");
	for (int i = 0; i < rows; i++) {
		// Update parameter i
		sprintf(command,"%s = %g;",parameterNames[i], simParameters[i][SimulationExperimentTracker::getCurrentSimuRun()]);
		executeVoidScilabJob(command,true);
		printLog(LOG_LEVEL_IMPORTANT, "          %s: %g:\n", parameterNames[i], simParameters[i][SimulationExperimentTracker::getCurrentSimuRun()]);
	}
}

void ScilabParameterSweeper::onExit(){
	char command[1024];
	if(!SimulationExperimentTracker::isLastRun()){
		// Increment Simu Run in Scilab (don't update currentSimuRun until next simulation run in the init function)
		sprintf(command,"%s = %s + 1;", this->simuRunVariableName.data(), this->simuRunVariableName.data());
	} else	{ // reset counter (leave it ready for next set of runs)
		sprintf(command,"%s = 0;", this->simuRunVariableName.data());
	}

	printLog(LOG_LEVEL_IMPORTANT, "[ScilabParameterSweeper] executing '%s' command ----------\n", command);
	executeVoidScilabJob(command,true);
}

