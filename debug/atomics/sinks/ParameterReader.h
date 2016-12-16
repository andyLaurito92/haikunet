//CPP:sinks/ParameterReader.cpp
//CPP:sinks/SimulationExperimentTracker.cpp
//CPP:../engine/pdevslib.cpp
#ifndef PARAMETER_READER_H
#define PARAMETER_READER_H

// pre defined logging levels
#define LOG_LEVEL_IMPORTANT 100
#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_ALWAYS 0
#define LOG_LEVEL_PRIORITY 1000
#define LOG_LEVEL_FULL_LOGGING 10000
#define LOG_LEVEL_DEBUG 9999999
#define LOG_LEVEL_INIT 10

// LOG_LEVEL=0 no logging; LOG_LEVEL=1 only important logs; ... LOG_LEVEL=9999 log everything
#define LOG_LEVEL LOG_LEVEL_IMPORTANT

// DEBUG LEVEL variables
#define DEBUG_LEVEL_VARIABLE_NAME "debugLevel"
#define DEBUG_LEVEL_DEFAULT 1
#define LOG_LEVEL_VARIABLE_NAME "logLevel"
#define LOG_LEVEL_DEFAULT LOG_LEVEL_INIT

// Cmdline option to specify the backend to be used for reading parameters
#define READING_BACKEND_OPTION_NAME "parameter_reading_backend"
#define SCILAB_READING_BACKEND_OPTION_VALUE "Scilab"
#define CMD_LINE_READING_BACKEND_OPTION_VALUE "CmdLine"
#define DEFAULT_READING_BACKEND_OPTION_VALUE "Default"

// Cmdline option to specify the backend to be used for loggin variables values
#define LOGGING_BACKEND_OPTION_NAME "variable_logging_backend"
#define SCILAB_LOGGING_BACKEND_OPTION_VALUE "Scilab"
#define OPENTSDB_LOGGING_BACKEND_OPTION_VALUE "OpenTSDB"
#define CONFIGURATION_LOGGING_BACKEND_OPTION_VALUE "ConfigurationLogger"
#define DEFAULT_LOGGING_BACKEND_OPTION_VALUE "Default"
#define OPENTSDB_HOST_OPTION_NAME "openTSDB-host"
#define OPENTSDB_DEFAULT_HOST "localhost"
#define OPENTSDB_PORT_OPTION_NAME "openTSDB-port"
#define OPENTSDB_DEFAULT_PORT 4242

#include "../engine/pdevslib.h"
#include <boost/algorithm/string/predicate.hpp>
#include "sinks/SimulationExperimentTracker.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "sinks/Loggers/ScilabLogger.h"
#include "simulator.h"

#include "hybrid/DistributionParameters/IDistributionParameter.h"
#include "hybrid/DistributionParameters/ExponentialDistributionParameter.h" // included also in 'CPP:' section of SimulationExperimentTracker
#include "hybrid/DistributionParameters/ConstantDistributionParameter.h"    // included also in 'CPP:' section of SimulationExperimentTracker
#include "hybrid/DistributionParameters/ParetoDistributionParameter.h"      // included also in 'CPP:' section of SimulationExperimentTracker
#include "hybrid/DistributionParameters/SplitDistributionParameter.h"      // included also in 'CPP:' section of SimulationExperimentTracker
#include "hybrid/DistributionParameters/NormalDistributionParameter.h"      // included also in 'CPP:' section of SimulationExperimentTracker
#include "hybrid/DistributionParameters/FelixPeriodPerThreadDistributionParameter.h"      // included also in 'CPP:' section of SimulationExperimentTracker
//class ConstantDistributionParameter;

#include <map>

#ifdef OPENTSDB
#include "sinks/OpenTsdb/OpenTsdbLogger.h"
#endif

class OpenTsdbLogger; // forward declaration of the class, because OpenTsdbLogger.h includes globals.h

/**
 * Print to the log file if level is higher
 */
void printLog(int level, const char *fmt,...);
void vprintLogLevel(const char *fmt, const char* fileName,  va_list va);

// A cache of the value, so as not to read it every time (every time a parameter is requested)
extern std::string global_private_readingBackend;
extern std::string global_private_loggingBackend;
std::string getReadingBackend();
std::string getLoggingBackend();

/**
 * Creas a logger based on the parameters
 */
std::shared_ptr<IPowerDEVSLogger> createDefaultLogger(std::string baseName);
std::shared_ptr<OpenTsdbLogger> createOpenTsdbLogger(int logLevel, Simulator* model);

/*** For reading Distribution parameters  **/
enum DISTRIBUTION_PARAMETER { CONSTANT, EXPONENTIAL, PARETO, SPLIT, NORMAL, FELIX_GBT};
std::shared_ptr<IDistributionParameter> readDistributionParameter(std::string paramName);

bool readScopedVariable(/*const*/ Simulator* model, std::string variableName, int& value);
int readDebugLevel(/*const*/ Simulator* model);
int readLogLevel(/*const*/ Simulator* model, std::string variableName);

template<typename T>
T readDefaultParameterValue(std::string paramName);


// NOTE: this is a trick to be able to do templates function overload (for numeric and non numeric types of T)
namespace detail{
	/**
	 * Main function that read the parameters from command line.
	 * Return false if reading parameter fails. Does not throw exceptions
	 */
	template<typename T>
	bool inner_getCmdLineParam(std::string paramName, T& value){
		try{
			po::variables_map vm;
			po::options_description generic("model Options");

			generic.add_options() (paramName.data(), po::value<T>());
			store(po::command_line_parser(CmdLineModelParams).options(generic)
					.style(po::command_line_style::allow_long | po::command_line_style::long_allow_adjacent | po::command_line_style::long_allow_next
							| po::command_line_style::allow_long_disguise) // to enable options to start with a '-' instead of '--'
							.allow_unregistered()    // to allow generic parameters (which can be read by models)
							.run()
							, vm);

			// Check parsing worked
			if(!vm.count(paramName)){
				return false;
			}

			value = vm[paramName].as<T>();
			return true;
		} catch(std::exception& e) {
			printLog(LOG_LEVEL_ERROR, "inner_getCmdLineParam: Parsing '%s' thow exception NOT FOUND\n", paramName.data());
			return false;
		}

		return false;
	}

	/**
	 * Main function that read the parameters from command line.
	 * If reading the parameter fails it throws an exception
	 */
	template<typename T>
	T inner_getCmdLineParam(std::string paramName){
		T value;

		if(detail::inner_getCmdLineParam(paramName, value)){
			return value;
		}

		// reading parameter failed. Throw exception
		printLog(LOG_LEVEL_ERROR, "Error Parsing parameters: parameter '%s' NOT FOUND\n", paramName.data());
		throw std::runtime_error(std::string("Error Parsing parameters: parameter '") + paramName + std::string("' NOT FOUND"));
	}

	// small utility to know if it is a numeric type
	template<bool> struct bool2type { };

	template<class T>
	struct S {
		/**
		 * Read parameter from command line. Non numeric types.
		 * Throws exception if parameter not present
		 */
		static T getCmdLineParam(std::string paramName, bool2type<false>){
//			printLog(LOG_LEVEL_FULL_LOGGING, "getCmdLineParam (primary template). Parameter: %s --\n", paramName.data());

			return detail::inner_getCmdLineParam<T>(paramName);
		}

		/**
		 * Read parameter from command line. Non numeric types.
		 * Returns false if parameter not present
		 */
		static bool getCmdLineParam(std::string paramName, T& value, bool2type<false>){
			//printLog(LOG_LEVEL_FULL_LOGGING, "getCmdLineParam (primary template) no exception. Parameter: %s--\n", paramName.data());

			return detail::inner_getCmdLineParam<T>(paramName, value);
		}

		/**
		 * Read parameter from command line. For numeric types first try to convert it directly.
		 * Throws exception if parameter not present
		 */
		static T getCmdLineParam(std::string paramName, bool2type<true>){
//			printLog(LOG_LEVEL_FULL_LOGGING, "getCmdLineParam (numeric specialization). Parameter: %s \n", paramName.data());

			// first try to convert the parameter to the value directly
			try
			{
				return boost::lexical_cast<T>(paramName.data());
			}
			catch(const boost::bad_lexical_cast &)
			{
				return detail::inner_getCmdLineParam<T>(paramName);
			}
		}

		/**
		 * Read parameter from command line. For numeric types first try to convert it directly.
		 * Return false if parameter not present
		 */
		static bool getCmdLineParam(std::string paramName,T& value, bool2type<true>){
			printLog(LOG_LEVEL_FULL_LOGGING, "getCmdLineParam (numeric specialization) - no exceptions. Parameter: %s \n", paramName.data());

			// first try to convert the parameter to the value directly
			try
			{
				value = boost::lexical_cast<T>(paramName.data());
				return true;
			}
			catch(const boost::bad_lexical_cast &)
			{
				return detail::inner_getCmdLineParam<T>(paramName, value);
			}
		}
	};

	/**
	 * Specialization for vector<string>. If parameter is not present, return a vector with the parameter name.
	 */
	template<>
	struct S <std::vector<std::string>> {
		static std::vector<std::string> getCmdLineParam(std::string paramName, bool2type<false>){
			printLog(LOG_LEVEL_FULL_LOGGING, "getCmdLineParam vector<std::string>. Parameter: %s \n", paramName.data());

			std::vector<std::string> value;
			if(detail::inner_getCmdLineParam<std::vector<std::string>>(paramName, value)){
				return value;
			} else {
				printLog(LOG_LEVEL_DEBUG, "Parameter not found. Returning a vector with the parameter as the only element \n");
				std::vector<std::string> vec;
				vec.push_back(paramName);
				return vec;
			}
		}
	};

	/**
	 * Specialization for vector<double>. If parameter is not present, try to cast the parameterName to double and return a vector with casted value.
	 */
	template<>
	struct S <std::vector<double>> {
		static std::vector<double> getCmdLineParam(std::string paramName, bool2type<false>){
			printLog(LOG_LEVEL_FULL_LOGGING, "getCmdLineParam vector<double>. Parameter: %s \n", paramName.data());

			double value;
			// first try to convert the parameter to the value directly
			try
			{
				value = boost::lexical_cast<double>(paramName.data());
				printLog(LOG_LEVEL_DEBUG, "Parameter not found. Returning a vector with the parameter as the only element \n");
				std::vector<double> vec;
				vec.push_back(value);
				return vec;
			}
			catch(const boost::bad_lexical_cast &)
			{
				return detail::inner_getCmdLineParam<std::vector<double>>(paramName);
			}
		}
	};
}

/**
 * Read parameter from command line. Throws exception if not found
 * For numeric types it first tries to convert it directly to a value. If conversion fails, it reads the parameter.
 */
template<typename T>
T getCmdLineParam(std::string paramName) {
//	printLog(LOG_LEVEL_IMPORTANT,"reading %s from cmdline. \n", paramName.data());
	// call the functions in the detail namespace. Using numeric_limits to check if T is numeric
	return detail::S<T>::getCmdLineParam(paramName, detail::bool2type<std::numeric_limits<T>::is_specialized>());
}

/**
 * Read parameter from command line. Returns false if not found.
 * For numeric types it first tries to convert it directly to a value. If conversion fails, it reads the parameter.
 */
template<typename T>
bool getCmdLineParam(std::string paramName, T& value) {
//	printLog(LOG_LEVEL_IMPORTANT,"reading %s from cmdline. \n", paramName.data());
	// call the functions in the detail namespace. Using numeric_limits to check if T is numeric
	return detail::S<T>::getCmdLineParam(paramName, value, detail::bool2type<std::numeric_limits<T>::is_specialized>());
}

/**
 * read scalar parameters from Scilab
 */
template<typename T>
T readDefaultScilabParameterValue(std::string& paramName, T*){
	//printLog(LOG_LEVEL_IMPORTANT,"reading %s from Scilab. \n", paramName);
	return getScilabVar(paramName.data(), true);
}

/**
 * read vector parameters from Scilab. Only works for std::vector<double>
 */
template<typename T>
std::vector<T> readDefaultScilabParameterValue(std::string& paramName, std::vector<T> *){
//	printLog(LOG_LEVEL_IMPORTANT,"reading a vector from scilab = %s \n", paramName);
	char command[1024];
	double *vectorMeta = new double [2];
	int rows, cols;

	// get size of vector
	sprintf(command,"size(%s)",paramName.data());
	getScilabVector(command, &cols, vectorMeta );
	rows = vectorMeta[0];
	cols = vectorMeta[1];

	// validate it is not a matrix
	if(rows != 1) {
		// check that variable exists-
		double f;
		sprintf(command,"exists('%s')", paramName.data());
		executeScilabJob(command,false);
		getAns(&f,1,1);
		if (f==0) {
			printLog("Variable %s does not exists! \n", paramName.data());
			throw std::runtime_error("Scilab Variable does not exists!");
		} else {
			printLog(LOG_LEVEL_ERROR,"reading matrices from scilab not supported. You must Implement getScialbMatrix in globals.h");
			throw std::runtime_error("reading matrices from scilab not supported. You must Implement getScialbMatrix in globals.h");
		}
	}

	// reserve memory
	double *vector = new double[cols];
//	sprintf(command,"%s",paramName.data());
	getScilabVector(paramName.data(), &cols, vector );

	// create a std::vector
	std::vector<double> ret(vector, vector + cols);

	// release memory
	delete vector;

	return ret;
}

/**
 * This is the default parameter reader. May read from Scilab or CmdLine depending its implementations.
 * Individual models can use directly the Scilab or CmdLine functions. For example, to read string parameters only command line is supported, or to read from Scilab without using cache.
 */
template<typename T>
T readDefaultParameterValue(std::string paramName){
	std::string parameterReadingBackend = getReadingBackend();

	// read according to the configured backend
	if(boost::iequals(parameterReadingBackend, DEFAULT_READING_BACKEND_OPTION_VALUE)){ // default
		return getCmdLineParam<T>(paramName); // NOTE: if the default option is changed to 'scilab' consider updating the RunScilabJob model to take that into account
	}else if(boost::iequals(parameterReadingBackend, SCILAB_READING_BACKEND_OPTION_VALUE)){ // Scilab
		return readDefaultScilabParameterValue(paramName, static_cast<T*>(0));
	} else if (boost::iequals(parameterReadingBackend, CMD_LINE_READING_BACKEND_OPTION_VALUE)){ // CmdLine
		return getCmdLineParam<T>(paramName);
	} else { // Error
		printLog(LOG_LEVEL_ERROR, "%s is not a recognized value for parameter %s.", parameterReadingBackend.data(), READING_BACKEND_OPTION_NAME);
		throw std::runtime_error(parameterReadingBackend + std::string(" is not a recognized value for parameter ") + std::string(READING_BACKEND_OPTION_NAME));
	}
}

/*! Tries to read a variable from configuration in different scopes in the following order
 * - First the complete hierarchical name (EJ: coupled.atomic.var=1)
 * - If fails, for the model name (EJ: atomic.var=1) (works for all models with the same name, for example inside coupled models)
 * - If fails, from global scope (EJ: var=1)
 * TODO: reading a non-existing variable from scilab throws exception, which is now handled with a try/catch (no nice and very slow).
 * TODO: there is now way to know if a variable is defined, so this might take a long time of reading if there are too many models
 * */
template<typename T>
bool readScopedVariable(/*const*/ Simulator* model, std::string variableName, T& value){
	try { // complete hierarchical model scope (EJ: coupled.atomic.var=1)
		value = readDefaultParameterValue<T>(model->getFullName()+ "." +  variableName);
		return true;
	} catch (...) {
		try { // model scope (EJ: atomic.var=1), would set all models with the same name (inside coupled).
			char paramName[150];
			sprintf(paramName,"%s.%s", model->getName(), variableName.data());
			value = readDefaultParameterValue<T>(paramName);
			return true;
		} catch (...) {
			try { // global scope (EJ: var=1), used for debugLevel, logLevel
				value = readDefaultParameterValue<T>(variableName);
				return true;
			} catch(...){
				return false;
			}
		}
	}

	return false;
}

#endif

