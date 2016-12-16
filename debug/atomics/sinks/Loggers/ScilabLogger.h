#if !defined ScilabLogger_h
#define ScilabLogger_h

// pre defined Scilab  logging levels
#if !defined SCILAB_LOG_LEVEL
#define SCILAB_LOG_LEVEL_ALWAYS 10
#define SCILAB_LOG_LEVEL_IMPORTANT 100
#define SCILAB_LOG_LEVEL_PRIORITY 1000
#define SCILAB_LOG_LEVEL_DEBUG 9999999

// LOG_LEVEL=0 no logging; LOG_LEVEL=1 only important logs; ... LOG_LEVEL=9999 log everything
#define SCILAB_LOG_LEVEL SCILAB_LOG_LEVEL_ALWAYS
#endif

#include "simulator.h"
#include <stdlib.h>
#include "string.h"
#include "unistd.h"
#include "fcntl.h"
#include <vector>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <ctime>

#include "sink/ParameterReader.h"
#include "sink/SimulationExperimentTracker.h"
#include "IPowerDEVSLogger.h"

//#include "common/BaseSimulator.h" // TODO: move base simulator out of PhaseI

class ScilabLogger : public IPowerDEVSLogger {
	#define BACKUP_DIRECTORY "ScilabLogs"
	#define TIME_VARIABLE_NAME "t"
    #define SCILAB_VARIABLE_NAME_FORMAT "%s.%s" // modelName.variableName

	// Logging Metric (for debugging)
	int modelWrittingTime_us; // total ms which all scilab loggers took to write logs to disk
	int modelMetricPoints; // total ms which all scilab loggers took to write logs to disk


	public:

		// Logging Metric (for debugging)
		static int TotalFlushingTime_ms; // total ms which all scilab loggers took to flush
		static int TotalWrittingTime_us; // total ms which all scilab loggers took to write logs to disk
		static int TotalMetricPoints; // total ms which all scilab loggers took to write logs to disk
		static int TotalLoggedVariables; // total ms which all scilab loggers took to write logs to disk
		static int TotalLoggedModels; // total ms which all scilab loggers took to write logs to disk

		ScilabLogger(Simulator* model, bool useMultipleSimulationRuns);
		ScilabLogger(std::string baseLoggingName, bool useMultipleSimulationRuns);
		void initSignals(std::vector<std::string> variableNames);
		void initSignals(std::map<std::string,int> variableNamesAndPriorities);
		void logSignal(double, double, std::string);
		void flush(double t);
	
	private:
		struct signalInfo {
			uint order;
			double lastState;
			const char* name;

			bool operator==(const signalInfo& other) const {
					return strcmp(name, other.name) == 0;
			};
		} ;

		bool useMultipleSimulationRuns;
		int logCount;
		int signalsNum=0;
		int FOutHandler;
		std::string modelName;
		char* fileName;
		std::map<std::string, signalInfo> signalNames;
		std::vector<std::string> signalSortedNames; // we keep them twice to avoid iterating over the map permanently



		void openFile();
		void deleteFile(char* fileName);
};

#endif
