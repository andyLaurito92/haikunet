//CPP:sinks/logtoscilabtool.cpp
#if !defined logtoscilabtool_h
#define logtoscilabtool_h

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

#include "ParameterReader.h"
#include "SimulationExperimentTracker.h"
#include "Loggers/IPowerDEVSLogger.h"

class aLoggerToScilab : public IPowerDEVSLogger {
	#define INF 1E10
	#define BACKUP_DIRECTORY "ScilabLogs"

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

		/// useMultipleSimulationRuns: 	if true, use a row for each simulation run
	    aLoggerToScilab(int level, int modelId,std::string modelName) : aLoggerToScilab(level, modelId,modelName, true) {};
		aLoggerToScilab(int level, int modelId,std::string modelName, bool useMultipleSimulationRuns);
		void initSignals(std::vector<const char*> variableNames);
		void initSignals(std::map<std::string,int> variableNamesAndPriorities);
		void logSignal(double, double, std::string);
		void flush();

	
	private:
		bool useMultipleSimulationRuns;
		int logLevel;
		int count;
		int signalsNum;
		int FOutHandler;
		std::string modelName;
		char* fileName;
		int modelID;
		double lastLogguedT = INF;


		void deleteFile(char* fileName);
	
		struct signalInfo {
			int order;
			double lastState;
			const char* name;

			bool operator==(const signalInfo& other) const {
					return strcmp(name, other.name) == 0;
			};
		} ;
		std::map<int,signalInfo> signalsNames;
		std::map<int,signalInfo>::iterator mit;
//		std::map<char*,signalInfo>::reverse_iterator mrit;

		void openFile();

};

#endif
