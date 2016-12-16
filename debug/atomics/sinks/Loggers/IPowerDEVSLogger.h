/*
 * IPowerDEVSLogger.h
 *
 *  Created on: Jun 3, 2015
 *      Author: mbonaven
 */

#ifndef I_POWERDEVS_LOGGER_H_
#define I_POWERDEVS_LOGGER_H_

#include <initializer_list>
#include <vector>

/**
 * Abstract class for PowerDEVS loggers. Known implementations: OpenTsdbLogger and aLoggerToScilab
 */
class IPowerDEVSLogger {
public:
	virtual void initSignals(std::vector<std::string> variableNames)=0;
	virtual void initSignals(std::map<std::string,int> variableNamesAndPriorities)=0;
	virtual void logSignal(double, double, std::string) =0;
	virtual void flush(double t) =0;

	virtual ~IPowerDEVSLogger() { }
};

#endif /* IPowerDEVSLogger_H_ */
