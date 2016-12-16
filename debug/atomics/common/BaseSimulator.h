//CPP:common/BaseSimulator.cpp
#if !defined BaseSimulator_h
#define BaseSimulator_h


#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "coupling.h"

#include "sinks/ParameterReader.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "sinks/Loggers/ConfigurationLogger.h" // TODO: delete this include afterwards, when the default logger is set to configure


class BaseSimulator: public Simulator {
protected:
	// Parameters
	double sigma;
	
	// Helpers
	int logLevel = -1;
	int debugLevel = -1;
	std::shared_ptr<IPowerDEVSLogger> logger;

	/**
	 * Write a debug message to the pdevs_<ExperimentNumber>.log file.
	 * If level <  Config(debugLevel)
	 */
	void debugMsg(int level, const char *fmt,...);

	/**
	 * Implements safety check for casting the value in the shared_ptr in Event.valuePtr.	 *
	 */
	template<typename T>
	std::shared_ptr<T> castEventPointer(Event x){
		auto value = std::static_pointer_cast<T>(x.valuePtr);

		// Safety Checks. TODO: this checks are not enough, but we found no way to really validate the cast was succesfull: http://stackoverflow.com/questions/27157735/checking-cast-from-shared-ptrvoid
		if(value == NULL){ // wrong message format
			printLog(LOG_LEVEL_ERROR, "[%f] %s: [WARNING!] Error wrong message received: unable to cast to (%s). \n", this->e, this->getName(), typeid(T).name());
			throw std::runtime_error( std::string("Unable to cast event value in model ") + std::string(this->getName()));
		}

		return value;
	}
public:
	BaseSimulator(const char *n): Simulator(n), sigma(std::numeric_limits<double>::max()), logLevel(-1), debugLevel(-1)  {};
	void init(double);
	double ta(double t);
	void exit(double t);
};
#endif
