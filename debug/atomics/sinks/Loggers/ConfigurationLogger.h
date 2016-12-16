#if !defined ConfigurationLogger_h
#define ConfigurationLogger_h

#include "sinks/ParameterReader.h"
#include "SamplerLogger.h"

#define LOGGER_ID_VARIABLE_NAME "logger"

enum CONFIGURATION_LOGGERS { ALL, SAMPLER /*HISTOGRAM*/};

#define DEFAULT_LOGGER_ID CONFIGURATION_LOGGERS::ALL

class ConfigurationLogger : public IPowerDEVSLogger {
	private:
		Simulator* model;
		std::map<std::string, std::shared_ptr<IPowerDEVSLogger>> variableLogger;

	public:

		ConfigurationLogger(Simulator* model);
		void initSignals(std::vector<std::string> variableNames);
		void initSignals(std::map<std::string,int> variableNamesAndPriorities);
		void logSignal(double, double, std::string);
		void flush(double t);
	
	private:
		std::shared_ptr<IPowerDEVSLogger> getLogger(int loggerId, std::string varName);

};

#endif
