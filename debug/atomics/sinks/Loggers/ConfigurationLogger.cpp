#include "ConfigurationLogger.h"

ConfigurationLogger::ConfigurationLogger(Simulator* model) : model(model) {
	printLog(LOG_LEVEL_DEBUG, "Created ConfigurationLogger \n");
}

std::shared_ptr<IPowerDEVSLogger> ConfigurationLogger::getLogger(int loggerId, std::string varName){
	CONFIGURATION_LOGGERS loggerType = static_cast<CONFIGURATION_LOGGERS>(loggerId);

	std::shared_ptr<IPowerDEVSLogger> logger;
	std::string fullName = this->model->getFullName() + "." + varName; // the logger will use modelName.varName as baseName for logging this variable
	switch (loggerType) {
	case ALL: {
		logger = createDefaultLogger(fullName);
		break;
	}
	case SAMPLER: {
		auto baseLogger = createDefaultLogger(fullName);
		logger = std::shared_ptr<IPowerDEVSLogger>(new SamplerLogger(this->model, baseLogger));
		break;
	}
	default: {
		printLog(LOG_LEVEL_ERROR, "ConfigurationLogger::getLogger: unknown logger code: %i  \n", loggerType);
		throw std::runtime_error("ConfigurationLogger::getLogger: unknown logger '" + std::to_string(loggerType) + "' code. See log for details");
		break;
	}
	}

	return logger;
}

void ConfigurationLogger::initSignals(std::map<std::string,int> variableNamesAndPriorities){
	// filter variables which do not have enough importance
	std::vector<std::string> variableNames;
	int configuredLogLevel;
	for(auto var : variableNamesAndPriorities){
		// read log level from config for this model variable
		configuredLogLevel = readLogLevel(this->model, var.first);

//		std::cout << "Variable '" << this->model->getFullName() << "::" << var.first <<"': Configured level: " << configuredLogLevel << " >?   setPriority:" << var.second << "\n";
		if(configuredLogLevel > var.second){ // Config > VarPriority
			variableNames.push_back(var.first.data());
		}
	}

	this->initSignals(variableNames);
}

void ConfigurationLogger::initSignals(std::vector<std::string> variableNames) {
	// Read from configuration the appropriate logger to use for each variable
	for(auto varName : variableNames){
		// read logger from config
		int loggerId = DEFAULT_LOGGER_ID;
		readScopedVariable<int>(this->model, std::string(varName) + "." +  LOGGER_ID_VARIABLE_NAME, loggerId);

		printLog(LOG_LEVEL_FULL_LOGGING, "Using logger %u for variable %s.%s \n", loggerId, this->model->getFullName().data(), varName.data());
		//std::cout << "Using logger " << loggerId << " for variable " << this->model->getFullName() << "." << varName << "\n";

		// set the logger for this variable
		auto logger = this->getLogger(loggerId, varName);
		logger->initSignals({varName}); // initialize variable with name 'value' as the logger already initialize as modelName.varName
		this->variableLogger[varName] = logger;
	}
}

void ConfigurationLogger::logSignal(double t, double signalValue, std::string signalName) {
	if(!this->variableLogger.count(signalName)) return; // signal has no logger associated (not enough priority)

	// forward to corresponding logger
	this->variableLogger[signalName]->logSignal(t, signalValue, signalName);
}

void ConfigurationLogger::flush(double t) { // Flush
	// forward to all loggers
	for(auto it = this->variableLogger.begin(); it != this->variableLogger.end(); it++) {
		it->second->flush(t);
	}
}



