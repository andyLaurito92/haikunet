//CPP:sinks/OpenTsdb/OpenTsdbLogger.cpp
//CPP:sinks/OpenTsdb/TelnetOpenTsdbClient.cpp
//CPP:sinks/SimulationExperimentTracker.cpp

#ifndef OPENTSDB_OpenTsdbLogger_h
#define OPENTSDB_OpenTsdbLogger_h

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
#include <thread>

//#include "sinks/globals.h"
//#include "sinks/OpenTsdb/HttpOpenTsdbClient.h"
#include "sinks/OpenTsdb/TelnetOpenTsdbClient.h"
#include "sinks/OpenTsdb/IOpenTsdbClient.h"
#include "sinks/IPowerDEVSLogger.h"
#include "sinks/SimulationExperimentTracker.h"


/***
 * TODO:
 *     - show warning if connection to different host:port wants to be generated
 *     - do a proper clean up of the variables and closing connection
 */
class OpenTsdbLogger : public IPowerDEVSLogger {
	#define OTSDB_VARIABLE_PREFIX "pwd."

private:
	static std::thread* ClientWorkerThread;
	// Lazy initialization for a single client instance
	static IOpenTsdbClient* OpenTsdbClientInstance;
	static IOpenTsdbClient* getOpenTsdbClientInstance(const std::string & hostName, unsigned short port);

public:
		// Logging Metric (for debugging)
		static int TotalFlushingTime_us; // total ms which all scilab loggers took to flush
		static int TotalWrittingTime_us; // total ms which all scilab loggers took to write logs to disk
		static int TotalMetricPoints; // total ms which all scilab loggers took to write logs to disk
		static int TotalLoggedVariables; // total ms which all scilab loggers took to write logs to disk
		static int TotalLoggedModels; // total ms which all scilab loggers took to write logs to disk

		OpenTsdbLogger(int level, const std::string modelName, const std::string & host, unsigned short port);
		void initSignals(int,...);
		void logSignal(double, double, std::string);
		void flush();


private:
		// Parameters (set in constructor)
		int logLevel;
		std::string modelName;
		std::string hostName;
		unsigned short port;

		// Logging Metric (for debugging)
		int modelWrittingTime_us; // total ms which all scilab loggers took to write logs to disk
		int modelMetricPoints; // total ms which all scilab loggers took to write logs to disk

};

#endif
