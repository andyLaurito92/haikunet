#include "logtoscilabtool.h"

int aLoggerToScilab::TotalFlushingTime_ms = 0;
int aLoggerToScilab::TotalWrittingTime_us = 0;
int aLoggerToScilab::TotalMetricPoints = 0;
int aLoggerToScilab::TotalLoggedVariables = 0;
int aLoggerToScilab::TotalLoggedModels = 0;

aLoggerToScilab::aLoggerToScilab(int level, int mID /* currently not used */, std::string mName, bool useMultipleSimulationRuns) { // Constructor
	printLog(LOG_LEVEL_DEBUG, "Creating aLoggerToScilab(useMultipleSimulationRuns=%s) for model %s \n", useMultipleSimulationRuns?"YES":"NO", mName.data());

	this->useMultipleSimulationRuns = useMultipleSimulationRuns;
	this->logLevel = level;
	if (SCILAB_LOG_LEVEL < this->logLevel) { return; }

	modelName = mName;
	fileName = new char[1024];
	tmpnam(fileName); // generates a random file name

	count = 0;
	char name[24] = { 29, 14, 22, 25, 13, 14, 31, 28, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40 };

	if (getOs() == WINDOWS)
		FOutHandler = open(fileName, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);
	else
		FOutHandler = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);

	printLog(1,"LogToScilab: Using file %s for model %s\n", fileName, modelName.data());
	//printLog(10,"LogToScilab: mID is (d:%d) (i:%i) (g:%f) \n", mID, mID, mID);
	write(FOutHandler, name, 24);
	close(FOutHandler);

	this->modelWrittingTime_us = 0;
	this->modelMetricPoints = 0;
}

void aLoggerToScilab::initSignals(std::map<std::string,int> variableNamesAndPriorities){
	printLog(LOG_LEVEL_IMPORTANT,"map #variables=%u \n", variableNamesAndPriorities.size());
	for(auto var : variableNamesAndPriorities){
		printLog(LOG_LEVEL_IMPORTANT,"\t %s=%u \n", var.first.data(), var.second);
	}
}

void aLoggerToScilab::initSignals(std::vector<const char*> variableNames) {
	printLog(LOG_LEVEL_IMPORTANT,"vector #variables=%u \n", variableNames.size());
	if (SCILAB_LOG_LEVEL < this->logLevel) { return; }
	signalsNum = variableNames.size(); // Numero de se�ales que quiero loguear
	aLoggerToScilab::TotalLoggedVariables += signalsNum + 1;
	aLoggerToScilab::TotalLoggedModels ++;

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

    const char* fvar;
	for (int i = 1; i <= variableNames.size(); i++) {
		fvar = variableNames[i-1];
		printLog(LOG_LEVEL_DEBUG,"LogToScilab: init varname #%i = %s \n", i, fvar);

		signalInfo mySignalInfo = { i, 0, fvar }; // Inicializo a 0
		signalsNames[i] = mySignalInfo;
	};

	// add time variable
	signalInfo timeSignalInfo = { 0, 0, "t" };
	signalsNames[0] = timeSignalInfo;
	close(FOutHandler);
}

void aLoggerToScilab::logSignal(double t, double signalValue, std::string signalName) { // Logger
	if (SCILAB_LOG_LEVEL < this->logLevel) { return; }
	auto initWrittingTime = std::chrono::system_clock::now();

	signalInfo* info;

	printLog(LOG_LEVEL_DEBUG,"LogToScilabMM: [%s] received var %s with value=%f and timestamp %f \n", modelName.data(), signalName.data(), signalValue, t);

	this->openFile();
	// only log once per timestamp
	if(t > this->lastLogguedT){ // if timestamp changed, log all previous vars
		this->count++;

		// write t
		write(FOutHandler, (char*) &this->lastLogguedT, sizeof(double));

		// log last stored values
		mit = signalsNames.begin(); mit++; // skip t variable
		for (; mit != signalsNames.end(); mit++) {
			info = &(*mit).second;

			printLog(LOG_LEVEL_DEBUG,"LogToScilabMM: logging varname #%i = %s with lastState=%f and timestamp %f \n", info->order, info->name, info->lastState, this->lastLogguedT);
			write(FOutHandler, (char*) &info->lastState, sizeof(double));
		};
	}

	// Updates the signal
	for (mit = signalsNames.begin(); mit != signalsNames.end(); mit++) {
		info = &(*mit).second;
		if (strcmp(info->name, signalName.data()) == 0){
			info->lastState = signalValue;
		}
	}

	this->lastLogguedT = t; // update t
	close(FOutHandler);

	// log writting times
	int writtingTime_us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - initWrittingTime).count();
	aLoggerToScilab::TotalWrittingTime_us += writtingTime_us;
	this->modelWrittingTime_us += writtingTime_us;

	this->modelMetricPoints++;
	aLoggerToScilab::TotalMetricPoints++;
}

void aLoggerToScilab::flush() { // Flush
	auto initFlushingTime = std::chrono::system_clock::now();

	if (SCILAB_LOG_LEVEL < this->logLevel) {
		return;
	}

	char command[1024];
	char multipleSimulationsIndex[1024];
	int result;
	long pos = 32;
	int i = 0;
	const char *signame;
	char* time_var = "t";

	printLog(LOG_LEVEL_DEBUG,"\n\n-----------  SIMULATION FINISED (logging variables for model %s) ----------------------\n	", this->modelName.data());

	// log last stored values
	this->logSignal(INF,INF, signalsNames[0].name);
	this->modelMetricPoints--;
	aLoggerToScilab::TotalMetricPoints--;

	// PrintLog
	for (mit = signalsNames.begin(); mit != signalsNames.end(); mit++) {
		printLog(LOG_LEVEL_DEBUG,"LogToScilab: flushing for model %s. varname #%i = %s (with lastState=%f) \n", this->modelName.data(), (*mit).second.order, (*mit).second.name, (*mit).second.lastState);
	};

	// read values from file
	this->openFile();
	result = lseek(FOutHandler, pos, SEEK_SET);
	result = write(FOutHandler, &count, sizeof(int));
	close(FOutHandler);

	// Load file and set variables in Scilab
	sprintf(command, "load %s ", fileName);
	executeVoidScilabJob(command, true);

	// TEMP
	sprintf(command, "tempdevs1= [ 102 105 108 101 40 39 99 108 111 115 101 39 44 102 105 108 101 40 41 41 32]; execstr(char(tempdevs1)); clear tempdevs1;");
	executeVoidScilabJob(command, true);

	// To preserve the temporal matrix on Scilab (for debugging)
	// sprintf(command,"tempdevs_%s=tempdevs;",modelName.data());
	// executeVoidScilabJob(command,true);

	printLog(10,"[DEBUG]: CURRENT RUN number of logs: %u . \n", this->count);

	// handle multiple simulation runs
	sprintf(multipleSimulationsIndex,"");
	int rows = -1, cols = -1;

	printLog(LOG_LEVEL_DEBUG,"[DEBUG]: getCurrentSimuRun = %u   \n", SimulationExperimentTracker::getCurrentSimuRun());
	if (this->useMultipleSimulationRuns && SimulationExperimentTracker::getCurrentSimuRun() >= 1 ) {
		printLog(LOG_LEVEL_DEBUG,"[DEBUG]: USING MULTIPLE SIMULATIONS!!! Simu N = %u  \n", SimulationExperimentTracker::getCurrentSimuRun());
		double *parametersMeta = new double[2];

		// get size of variable matrix with previous simulations
		sprintf(command, "size(%s_%s)", modelName.data(), signalsNames[0].name);
		getScilabVector(command, &cols, parametersMeta);
		rows = parametersMeta[0];
		cols = parametersMeta[1];

		printLog(LOG_LEVEL_DEBUG,"[DEBUG]: variable %s has rows:%u and cols:%u . \n", signalsNames[0].name, rows, cols);

		// resize matrix(previous simulations) or array (current simulation) so that they match number of columns
		if (cols > this->count) { // resize array
			printLog(LOG_LEVEL_DEBUG,"[DEBUG]: resizing   tempdevs \n");
			sprintf(command, "tempdevs(:, %u:%u) = %%nan;", this->count+1, cols);
			executeVoidScilabJob(command, true);
		} else if (cols < this->count) { // resize matrix
			// must resize all variables, so it is done afterwards before setting value
		}

		// set indexer
		sprintf(multipleSimulationsIndex, "(%u,:)", SimulationExperimentTracker::getCurrentSimuRun()+1);
	}

	printLog(LOG_LEVEL_DEBUG,"[DEBUG]: multipleSimulationsIndex = %s \n", multipleSimulationsIndex);

	// Set the value to each variable
	for (mit = signalsNames.begin(); mit != signalsNames.end(); mit++) {
		i = (*mit).second.order+1;
		signame = (*mit).second.name;

		if (this->useMultipleSimulationRuns && cols < this->count && SimulationExperimentTracker::getCurrentSimuRun() >= 1) { // resize variable to match with matrix number of columns
			printLog(LOG_LEVEL_DEBUG,"[DEBUG]: resizing %s_%s \n ",modelName.data(), signame);
			sprintf(command, "%s_%s(:, %u:%u)=%%nan;", modelName.data(), signame, cols+1, this->count);
			executeVoidScilabJob(command, true);
		}

		// set variable value
		sprintf(command, "%s_%s%s=tempdevs(%i,:);", modelName.data(), signame, multipleSimulationsIndex, i);
		executeVoidScilabJob(command, true);
	};

	//sprintf(command, "clear tempdevs;");
	//executeVoidScilabJob(command, true);

	//sprintf(buf1,"%s0,0)",buf1);		printLog(10,"LogToScilab: %s \n",buf1);
	//executeVoidScilabJob(buf1,true); // HANGS UP the Scilab Plot !!! Do it manually for now.

	//sprintf(buf2,"plot_%s= \" %s \" ",modelName.data(),buf1);		printLog(10,"LogToScilab: %s \n",buf2);
	//executeVoidScilabJob(buf2,true); // HANGS UP

	// Comment the following line to preserve the temporary file (for debugging)
	//deleteFile(fileName);

	// Log flushing times
	 int flushingTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - initFlushingTime).count();
	 aLoggerToScilab::TotalFlushingTime_ms += flushingTime_ms;

	 printLog(LOG_LEVEL_PRIORITY,"[LogToScilab]: model %s flushing time: %i (ms) \n ",modelName.data(), flushingTime_ms);
	 printLog(LOG_LEVEL_PRIORITY,"[LogToScilab]: model %s writing time: %i (us) \n ",modelName.data(), this->modelWrittingTime_us);
	 printLog(LOG_LEVEL_PRIORITY,"[LogToScilab]: model %s logged metric points (not including T) : %i  \n ",modelName.data(), this->modelMetricPoints);
}

void aLoggerToScilab::openFile() {
	if (getOs() == WINDOWS)
		FOutHandler = open(fileName, O_WRONLY  | O_BINARY, 0666);
	else
		FOutHandler = open(fileName, O_WRONLY , 0666);

	if(FOutHandler == -1){
		printLog(LOG_LEVEL_ERROR,"Unable to open file %s for model %s\n", fileName, modelName.data());
	}

	lseek(FOutHandler, 0, SEEK_END);
}

void aLoggerToScilab::deleteFile(char* fileName) {
	struct stat info;

	//printLog(10,"LogToScilab: deleting file %s \n",fileName);

	stat(fileName, &info);
	if (S_ISREG(info.st_mode) == 1) { // existe el archivo de datos, es necesario borrarlo.
		if (chmod(fileName, S_IRWXU) != 0)
			printLog(10,"LogToScilab: CHMOD Error: %s\n", strerror(errno));
		else {
			stat(fileName, &info);
			//printLog(10,"LogToScilab: CHMOD permissions are: %08x\n", info.st_mode);
		}
		if (remove(fileName) != 0)
			printLog(10,"LogToScilab: REMOVE %s Error: %s\n", fileName, strerror(errno)); //Da error cuando el File tiene parametro de Solo Lectura
		else
			printLog(10,"LogToScilab: REMOVE %s Successful!\n", fileName);
	}
}

