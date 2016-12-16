#include "OpenTsdbLogger.h"

int OpenTsdbLogger::TotalFlushingTime_us = 0;
int OpenTsdbLogger::TotalWrittingTime_us = 0;
int OpenTsdbLogger::TotalMetricPoints = 0;
int OpenTsdbLogger::TotalLoggedVariables = 0;
int OpenTsdbLogger::TotalLoggedModels = 0;

IOpenTsdbClient* OpenTsdbLogger::OpenTsdbClientInstance = NULL;
std::thread* OpenTsdbLogger::ClientWorkerThread = NULL;

/*static*/ IOpenTsdbClient* OpenTsdbLogger::getOpenTsdbClientInstance(const std::string & hostName, unsigned short port){
	// TODO: show warning if connection to different host:port wants to be generated
	if (OpenTsdbLogger::OpenTsdbClientInstance == NULL){
		OpenTsdbLogger::OpenTsdbClientInstance = new TelnetOpenTsdbClient(hostName, port);

		// connect to openTSDB
		OpenTsdbLogger::ClientWorkerThread = new std::thread(boost::bind(&IOpenTsdbClient::run, OpenTsdbLogger::OpenTsdbClientInstance));
	}

	return OpenTsdbLogger::OpenTsdbClientInstance;
}

OpenTsdbLogger::OpenTsdbLogger(int level, const std::string modelName, const std::string & hostName, unsigned short port) {
	printLog(LOG_LEVEL_DEBUG, "Creating OpenTsdbLogger(host=%s, port=%i) for model %s\n", hostName.data(), port, modelName.data());

	this->logLevel = level;
	this->modelName = modelName;
	this->hostName = hostName;
	this->port = port;

	if (SCILAB_LOG_LEVEL < this->logLevel) { return; }

	// create the instance to start connections
	IOpenTsdbClient* instance = OpenTsdbLogger::getOpenTsdbClientInstance(this->hostName, this->port);
	if(instance == NULL){
		throw "OpenTsdbLogger client instances was not created properly";
	}

	// init metric variables
	this->modelWrittingTime_us = 0;
	this->modelMetricPoints = 0;
}

void OpenTsdbLogger::initSignals(int amount, ...) {
	if (SCILAB_LOG_LEVEL < this->logLevel) { return; }

	OpenTsdbLogger::TotalLoggedVariables += amount;
	OpenTsdbLogger::TotalLoggedModels ++;

	// TODO: would be good to evaluate to 'pre-assign' UIDs here. Read more at http://opentsdb.net/docs/build/html/user_guide/writing.html#write-performance
	// doing pre-assing would make initialization more time consuming, but might ease (not sure) the rest of the simulation.

}

void OpenTsdbLogger::logSignal(double t, double signalValue, std::string signalName) {
	if (SCILAB_LOG_LEVEL < this->logLevel) { return; }
	auto initWrittingTime = std::chrono::system_clock::now();

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] OpenTsdbLogger: about to log <%s, %f> \n", t, signalName.data(),  signalValue);

	// prepare signal as an OpenTsdb point
	IOpenTsdbClient::TagsType tags;
	tags["modelName"] = this->modelName;
	//tags["simuSetID"] = SimulationExperimentTracker::getSimuSetID();
	//tags["simuID"] = SimulationExperimentTracker::getSimuID();
	tags["experimentNumber"] = boost::lexical_cast<std::string>(SimulationExperimentTracker::getCurrentSimuRun());
	unsigned long timestamp_us = t * (1000000); // log in microseconds
	std::string metricName = OTSDB_VARIABLE_PREFIX + SimulationExperimentTracker::getSimuSetID() + "." + SimulationExperimentTracker::getSimuID() + "." + signalName;

	// send point to openTsdb
	OpenTsdbLogger::getOpenTsdbClientInstance(this->hostName, this->port)->add_point(metricName, timestamp_us, signalValue, &tags);

	// log writting times
	int writtingTime_us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - initWrittingTime).count();
	OpenTsdbLogger::TotalWrittingTime_us += writtingTime_us;
	this->modelWrittingTime_us += writtingTime_us;

	this->modelMetricPoints++;
	OpenTsdbLogger::TotalMetricPoints++;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] OpenTsdbLogger: successfully logged %s (total logs: %i)\n", t, signalName.data(),  OpenTsdbLogger::TotalMetricPoints );
}

void OpenTsdbLogger::flush() {
	if (SCILAB_LOG_LEVEL < this->logLevel) { return; }

	auto initFlushingTime = std::chrono::system_clock::now();

	if(!OpenTsdbLogger::getOpenTsdbClientInstance(this->hostName, this->port)->isClosed() && OpenTsdbLogger::ClientWorkerThread){
		// close the connection & wait for thread to finish
		printLog(LOG_LEVEL_IMPORTANT, "[OpenTsdbLogger] closing & waiting thread to finish\n");
		OpenTsdbLogger::getOpenTsdbClientInstance(this->hostName, this->port)->close();
		OpenTsdbLogger::ClientWorkerThread->join();
		printLog(LOG_LEVEL_IMPORTANT, "[OpenTsdbLogger] thread finished \n");

		// TODO: do a proper clean up of the variables and closing connection (a shared instance)
		// wait for the process to finish
		//delete OpenTsdbLogger::ClientWorkerThread;
		//	delete this->openTsdbClient;
	} else {
		//printLog(LOG_LEVEL_IMPORTANT, "[OpenTsdbLogger] WARNING: requested closed, but already closed !!\n");
	}

	// track flushing time
	int flushingTime_us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - initFlushingTime).count();
	OpenTsdbLogger::TotalFlushingTime_us += flushingTime_us;
}
