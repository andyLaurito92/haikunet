#include "ParameterReader.h"

std::string global_private_readingBackend = "";
std::string global_private_loggingBackend = "";

std::shared_ptr<IDistributionParameter> readDistributionParameter(std::string paramName){
	int distributionCode = readDefaultParameterValue<int>(paramName);

	// Check which assignment algorithm was configured
	DISTRIBUTION_PARAMETER ditributionType = static_cast<DISTRIBUTION_PARAMETER>(distributionCode);

	std::shared_ptr<IDistributionParameter> distribution;
	switch (ditributionType) {
	case CONSTANT: {
		distribution = std::make_shared<ConstantDistributionParameter>();
		break;
	}
	case EXPONENTIAL: {
		distribution = std::make_shared<ExponentialDistributionParameter>();
		break;
	}
	case PARETO: {
		distribution = std::make_shared<ParetoDistributionParameter>();
		break;
	}
	case SPLIT: {
		distribution = std::make_shared<SplitDistributionParameter>();
		break;
	}
	case NORMAL: {
		distribution = std::make_shared<NormalDistributionParameter>();
		break;
	}
	case FELIX_GBT: {
			distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
			break;
	}
	default: {
		printLog("readDistributionParameter: unknown distribution code: %i  \n", ditributionType);
		throw std::runtime_error("readDistributionParameter: unknown distribution code. See log for details");
		break;
	}
	}

	// parse the distribution parameters
	distribution->readParameters(paramName);

	return distribution;
}

int readDebugLevel(/*const*/ Simulator* model){
	int level = DEBUG_LEVEL_DEFAULT; // if not in config, default value
	readScopedVariable<int>(model, DEBUG_LEVEL_VARIABLE_NAME, level); // read from config

	return level;
}

/* read the log level for a variable in different scopes:
 * - First, it trys with full name: EJ: coupled.atomic.variable.logLevel=1
 * - If it fails, it uses the scope search for without variable name. EJ: coupled.atomic.logLevel --> atomic.logLevel --> logLevel
 */
int readLogLevel(/*const*/ Simulator* model, std::string variableName){ // read from config
	int level = LOG_LEVEL_DEFAULT; // if not in config, default value;
	try { // complete hierarchical model scope with var name (EJ: coupled.atomic.logLevel=1)
		level = readDefaultParameterValue<int>(model->getFullName()+ "." +  variableName + "." LOG_LEVEL_VARIABLE_NAME);
	} catch (...) {
		readScopedVariable<int>(model, LOG_LEVEL_VARIABLE_NAME, level); // read scoped (without variable name) from config
	}

	return level;
}


std::string getReadingBackend(){
	// read variable only once, lazy loading
	if(boost::iequals(global_private_readingBackend, "")){
		if(!getCmdLineParam<std::string>(READING_BACKEND_OPTION_NAME, global_private_readingBackend)){
			global_private_readingBackend = DEFAULT_READING_BACKEND_OPTION_VALUE;
		}
	}

	return global_private_readingBackend;
}

std::string getLoggingBackend(){
	// read variable only once, lazy loading
	if(boost::iequals(global_private_loggingBackend, "")){
		if(!getCmdLineParam<std::string>(LOGGING_BACKEND_OPTION_NAME, global_private_loggingBackend)){
			global_private_loggingBackend = DEFAULT_LOGGING_BACKEND_OPTION_VALUE;
		}
		//		printLog(LOG_LEVEL_IMPORTANT,"logging backend = %s\n", global_private_loggingBackend.data());
	}

	return global_private_loggingBackend;
}

std::shared_ptr<IPowerDEVSLogger> createDefaultLogger(std::string baseName){
	std::string loggingBackend = getLoggingBackend();
	if(boost::iequals(loggingBackend, DEFAULT_LOGGING_BACKEND_OPTION_VALUE)){// the default option, if no backend was specified
		return std::make_shared<ScilabLogger>(baseName, false);
	} else if(boost::iequals(loggingBackend, SCILAB_LOGGING_BACKEND_OPTION_VALUE)){ // Scilab
		return std::make_shared<ScilabLogger>(baseName, false);
	}
#ifdef OPENTSDB
	else if (boost::iequals(loggingBackend, OPENTSDB_LOGGING_BACKEND_OPTION_VALUE)){ // OpenTSDB
		return createOpenTsdbLogger(1 /* TODO: fix Otsdb implementation */, baseName);
	}
#endif
	else { // ERROR
		printLog(LOG_LEVEL_ERROR, "%s is not a recognized value for parameter %s.", loggingBackend.data(), LOGGING_BACKEND_OPTION_NAME);
		throw std::runtime_error(loggingBackend + std::string(" is not a recognized value for parameter ") + std::string(LOGGING_BACKEND_OPTION_NAME));
	}
}

std::shared_ptr<OpenTsdbLogger> createOpenTsdbLogger(int logLevel, std::string modelName){
#ifdef OPENTSDB
	int port = OPENTSDB_DEFAULT_PORT;
	std::string host = OPENTSDB_DEFAULT_HOST;

	// try to read host & port from config
	getCmdLineParam<std::string>(OPENTSDB_HOST_OPTION_NAME, host);
	getCmdLineParam<int>(OPENTSDB_HOST_OPTION_NAME, port);


	return std::make_shared<OpenTsdbLogger>(logLevel, modelName, host, port);
#endif
	throw std::runtime_error("OpenTsdb is not enabled in the compilation. Use the -DOPENTSDB flag in compilation to enable it");
}

/**
 * Uses a log level and takes into account various simulations runs (for the filename)
 */
void printLog(int level, const char *fmt,...){
	if (LOG_LEVEL >= level) { // TODO: update it for readDebugLevel(this) > level
		char fileName[1024];
		sprintf(fileName,"%s/pdevs_run%i.log",getExecutablePath(), SimulationExperimentTracker::getCurrentSimuRun());

		va_list va;
		va_start(va,fmt);
		vprintLogLevel(fmt, fileName, va);
		va_end(va);
	}
}

void vprintLogLevel(const char *fmt, const char* fileName,  va_list va) {
	static std::vector<std::string> files;

	if (std::find(files.begin(), files.end(), fileName) == files.end())
	{
		files.push_back(fileName);
		fclose(fopen(fileName,"w"));
	}

	FILE *fd=fopen(fileName,"a");
	vfprintf(fd,fmt,va);
	fclose(fd);
}
