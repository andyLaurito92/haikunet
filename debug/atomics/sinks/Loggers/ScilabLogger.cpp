#include "ScilabLogger.h"

int ScilabLogger::TotalFlushingTime_ms = 0;
int ScilabLogger::TotalWrittingTime_us = 0;
int ScilabLogger::TotalMetricPoints = 0;
int ScilabLogger::TotalLoggedVariables = 0;
int ScilabLogger::TotalLoggedModels = 0;

ScilabLogger::ScilabLogger(Simulator* model, bool useMultipleSimulationRuns) : ScilabLogger(model->getFullName(), useMultipleSimulationRuns){

}

ScilabLogger::ScilabLogger(std::string modelName, bool useMultipleSimulationRuns) :
		modelWrittingTime_us(0), modelMetricPoints(0), useMultipleSimulationRuns(useMultipleSimulationRuns), logCount(0), signalsNum(-1), modelName(modelName) {
	printLog(LOG_LEVEL_DEBUG, "Creating ScilabLogger for model %s \n", modelName.data());

	// generates a file name to log data
	this->fileName = new char[1024];
	tmpnam(this->fileName); // generates a random file name
	printLog(1,"ScilabLogger: Using file %s for model %s\n", this->fileName, this->modelName.data());

	// open file in mode create
	if (getOs() == WINDOWS){
		FOutHandler = open(fileName, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);
	} else {
		FOutHandler = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	}

	if(FOutHandler == -1){
		printLog(LOG_LEVEL_ERROR,"Unable to open file %s for model %s\n", fileName, this->modelName.data());
	}

	// Write header and close
	char name[24] = { 29, 14, 22, 25, 13, 14, 31, 28, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40 };
	write(FOutHandler, name, 24);
	close(FOutHandler);
}

void ScilabLogger::initSignals(std::map<std::string,int> variableNamesAndPriorities){
	// TODO: we need a baseline logLevel to compare with. Maybe pass it in the constructor or in this method
	printLog(1,"ScilabLogger:initSignals(map<string, int>) this method is not implemented for ScilabLogger. Use the initSignals(vector) or the Configuration logger");
	throw std::runtime_error("ScilabLogger:initSignals(map<string, int>) this method is not implemented for ScilabLogger. Use the initSignals(vector) or the Configuration logger.");

	// filter variables which do not have enough importance
//	std::vector<std::string> variableNames;
//	int variableLogLevel;
//	for(auto var : variableNamesAndPriorities){
//		printLog(LOG_LEVEL_DEBUG,"\t %s=%u \n", var.first.data(), var.second);
//
//		if(variableLogLevel >= var.second){
//			variableNames.push_back(var.first);
//		}
//	}
//
//	this->initSignals(variableNames);
}

void ScilabLogger::initSignals(std::vector<std::string> variableNames) {
	signalsNum = variableNames.size(); // Numero de se�ales que quiero loguear
	ScilabLogger::TotalLoggedVariables += signalsNum + 1;
	ScilabLogger::TotalLoggedModels ++;

	// if there is nothing to log finish here
	if(signalsNum == 0) return;

	this->openFile();
	int type = 1;
	write(FOutHandler, &type, sizeof(int)); // FLOATING MATRX TYPE 1
	int cols = signalsNum + 1;
	write(FOutHandler, &cols, sizeof(int)); // N+1 INITIAL COLUMNS
	int rows = 2;
	write(FOutHandler, &rows, sizeof(int)); // 2 INITIAL ROWS
	short s = 0;
	for (int i = 1; i <= rows; i++) { // INICIALIZA A 0
		write(FOutHandler, &s, sizeof(short));
	};

    for (uint i = 1; i <= variableNames.size(); i++) {
    	std::string varName = variableNames[i-1];
		printLog(LOG_LEVEL_DEBUG,"ScilabLogger: init varname #%i = %s \n", i, varName.data());

//		signalInfo* mySignalInfo = new signalInfo();
//		mySignalInfo->order = i; mySignalInfo->lastState = 0; mySignalInfo->name = fvar.data();
		signalInfo mySignalInfo = { i, 0, varName.data() }; // Inicializo a 0
		this->signalNames[varName] = mySignalInfo;
		this->signalSortedNames.push_back(varName);
	};

	// add time variable
    signalInfo timeSignalInfo = { 0, std::numeric_limits<double>::max(), TIME_VARIABLE_NAME };
	this->signalNames[TIME_VARIABLE_NAME] = timeSignalInfo;
	this->signalSortedNames.push_back(TIME_VARIABLE_NAME);
	close(FOutHandler);
}

void ScilabLogger::logSignal(double t, double signalValue, std::string signalName) { // Logger
	auto initWrittingTime = std::chrono::system_clock::now();

	if(!this->signalNames.count(signalName)){
		printLog(LOG_LEVEL_FULL_LOGGING ,"ScilabLogger for model '%s': received var '%s' which was not initialized. ignoring.. \n", this->modelName.data(), signalName.data());
		return;
	}
	signalInfo info = this->signalNames[signalName];

	printLog(LOG_LEVEL_DEBUG,"ScilabLogger for model '%s': received var '%s' with value=%f and timestamp %f \n", this->modelName.data(), info.name, signalValue, t);


	// log all  vars only log once per timestamp
	if(t > this->signalNames[TIME_VARIABLE_NAME].lastState){ // if timestamp changed, log all previous vars
		this->logCount++;

		// log last stored values
		this->openFile();
		for(auto varName : this->signalSortedNames){
			// get var info
			info = this->signalNames[varName];

			printLog(LOG_LEVEL_DEBUG,"ScilabLogger for model '%s': logging var #%i = '%s' with lastState=%f and timestamp %f \n", this->modelName.data(), info.order, info.name, info.lastState, this->signalNames[TIME_VARIABLE_NAME].lastState);

			// write previous var value to the file
			write(FOutHandler, (char*) &info.lastState, sizeof(double));
		}
		close(FOutHandler);
	}

	// Updates this signal
	this->signalNames[signalName].lastState = signalValue;
	this->signalNames[TIME_VARIABLE_NAME].lastState = t; // update t

	// log writting times
	int writtingTime_us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - initWrittingTime).count();
	ScilabLogger::TotalWrittingTime_us += writtingTime_us;
	this->modelWrittingTime_us += writtingTime_us;

	this->modelMetricPoints++;
	ScilabLogger::TotalMetricPoints++;
}

void ScilabLogger::flush(double t) { // Flush
	auto initFlushingTime = std::chrono::system_clock::now();

	// if there is nothing to log finish here
	if(signalsNum <= 0) {
		deleteFile(fileName);
		return;
	}

	char command[1024];
	char multipleSimulationsIndex[1024];

	printLog(LOG_LEVEL_FULL_LOGGING,"\n\n-----------  SIMULATION FINISED (logging %u variables for model %s) ----------------------\n ", this->signalsNum, this->modelName.data());

	// log last stored values
	this->logSignal(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), signalNames[TIME_VARIABLE_NAME].name);
	this->modelMetricPoints--;
	ScilabLogger::TotalMetricPoints--;

	// PrintLog
//	for (auto mit = signalNames.begin(); mit != signalNames.end(); mit++) {
//		printLog(LOG_LEVEL_DEBUG,"ScilabLogger: flushing for model %s. varname #%i = %s (with lastState=%f) \n", modelName, (*mit).second.order, (*mit).second.name, (*mit).second.lastState);
//	};

	// read values from file (TODO: check possible errors)
	this->openFile();
	lseek(FOutHandler, 32, SEEK_SET);
	write(FOutHandler, &logCount, sizeof(int));
	close(FOutHandler);

	// Load file and set variables in Scilab
	sprintf(command, "load %s ", fileName);
	executeVoidScilabJob(command, true);

	// TEMP //TODO: review if it is really necessary
	sprintf(command, "tempdevs1= [ 102 105 108 101 40 39 99 108 111 115 101 39 44 102 105 108 101 40 41 41 32]; execstr(char(tempdevs1)); clear tempdevs1;");
	executeVoidScilabJob(command, true);

	// To preserve the temporal matrix on Scilab (for debugging)
	// sprintf(command,"tempdevs_%s=tempdevs;",this->modelName.data());
	// executeVoidScilabJob(command,true);

	printLog(LOG_LEVEL_DEBUG,"[DEBUG]: CURRENT RUN number of logs: %u . \n", this->logCount);

	// handle multiple simulation runs
	multipleSimulationsIndex[0] = 0; //multipleSimulationsIndex="";
	//int rows = -1;
	int cols = -1;

	printLog(LOG_LEVEL_DEBUG,"[DEBUG]: getCurrentSimuRun = %u   \n", SimulationExperimentTracker::getCurrentSimuRun());
	if (this->useMultipleSimulationRuns && SimulationExperimentTracker::getCurrentSimuRun() >= 1 ) {
		printLog(LOG_LEVEL_DEBUG,"[DEBUG]: USING MULTIPLE SIMULATIONS!!! Simu N = %u  \n", SimulationExperimentTracker::getCurrentSimuRun());
		double *parametersMeta = new double[2];

		// get size of variable matrix with previous simulations
		sprintf(command, "size(" SCILAB_VARIABLE_NAME_FORMAT ")", this->modelName.data(), signalNames[TIME_VARIABLE_NAME].name);
		getScilabVector(command, &cols, parametersMeta);
		//rows = parametersMeta[0];
		cols = parametersMeta[1];

		//printLog(LOG_LEVEL_DEBUG,"[DEBUG]: variable %s has rows:%u and cols:%u . \n", signalNames[TIME_VARIABLE_NAME].name, rows, cols);

		// resize matrix(previous simulations) or array (current simulation) so that they match number of columns
		if (cols > this->logCount) { // resize array
			//printLog(LOG_LEVEL_DEBUG,"[DEBUG]: resizing   tempdevs \n");
			sprintf(command, "tempdevs(:, %u:%u) = %%nan;", this->logCount+1, cols);
			executeVoidScilabJob(command, true);
		} else if (cols < this->logCount) { // resize matrix
			// must resize all variables, so it is done afterwards before setting value
		}

		// set indexer
		sprintf(multipleSimulationsIndex, "(%u,:)", SimulationExperimentTracker::getCurrentSimuRun()+1);
	}

	printLog(LOG_LEVEL_DEBUG,"[DEBUG]: multipleSimulationsIndex = %s \n", multipleSimulationsIndex);

	// Set the value to each variable
	int i = 1;
	for(auto varName : this->signalSortedNames){
		if (this->useMultipleSimulationRuns && cols < this->logCount && SimulationExperimentTracker::getCurrentSimuRun() >= 1) { // resize variable to match with matrix number of columns
			sprintf(command,  SCILAB_VARIABLE_NAME_FORMAT "(:, %u:%u)=%%nan;", this->modelName.data(), varName.data(), cols+1, this->logCount);
			executeVoidScilabJob(command, true);
		}

		// set variable value
		sprintf(command, SCILAB_VARIABLE_NAME_FORMAT "%s=tempdevs(%i,:);", this->modelName.data(), varName.data(), multipleSimulationsIndex, i);
		executeVoidScilabJob(command, true);
//		printLog(1,"[ScilabLogger- DEBUG]: executed %s \n ",command);
		i++;
	}

	// Comment the following line to preserve the temporary file (for debugging)
	//deleteFile(fileName);

	// Log flushing times
	 int flushingTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - initFlushingTime).count();
	 ScilabLogger::TotalFlushingTime_ms += flushingTime_ms;

	 printLog(LOG_LEVEL_PRIORITY,"[ScilabLogger]: model %s flushing time: %i (ms) \n ", this->modelName.data(), flushingTime_ms);
	 printLog(LOG_LEVEL_PRIORITY,"[ScilabLogger]: model %s writing time: %i (us) \n ", this->modelName.data(), this->modelWrittingTime_us);
	 printLog(LOG_LEVEL_PRIORITY,"[ScilabLogger]: model %s logged metric points (not including T) : %i  \n ", this->modelName.data(), this->modelMetricPoints);
}

void ScilabLogger::openFile() {
	if (getOs() == WINDOWS)
		FOutHandler = open(fileName, O_WRONLY  | O_BINARY, 0666);
	else
		FOutHandler = open(fileName, O_WRONLY , 0666);

	if(FOutHandler == -1){
		printLog(LOG_LEVEL_ERROR,"Unable to open file %s for model %s\n", fileName, this->modelName.data());
	}

	lseek(FOutHandler, 0, SEEK_END);
}

void ScilabLogger::deleteFile(char* fileName) {
	struct stat info;

	//printLog(10,"ScilabLogger: deleting file %s \n",fileName);

	stat(fileName, &info);
	if (S_ISREG(info.st_mode) == 1) { // existe el archivo de datos, es necesario borrarlo.
		if (chmod(fileName, S_IRWXU) != 0)
			printLog(1,"ScilabLogger: CHMOD Error: %s\n", strerror(errno));
		else {
			stat(fileName, &info);
			//printLog(10,"ScilabLogger: CHMOD permissions are: %08x\n", info.st_mode);
		}

		if (remove(fileName) != 0)
			printLog(1,"ScilabLogger: REMOVE %s Error: %s\n", fileName, strerror(errno)); //Da error cuando el File tiene parametro de Solo Lectura
		else
			printLog(LOG_LEVEL_DEBUG,"ScilabLogger: REMOVE %s Successful!\n", fileName);
	}
}

