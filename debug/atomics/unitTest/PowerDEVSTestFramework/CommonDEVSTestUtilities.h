#ifndef CommonDEVSTestUtilities_h
#define CommonDEVSTestUtilities_h

#include <stdlib.h>
#include <iostream>

#include "simulator.h"
#include "event.h"
#include "vector/VectorialEvent.h"


Event createEventWithVectorial(std::shared_ptr<void> value, int port, int index){
	auto vec = std::make_shared<VectorialEvent>(value, index);
	return Event(vec, port);
}

Event createEventWithVectorial(void* value, int port, int index){
	auto vec = std::make_shared<VectorialEvent>(value, index);
	return Event(vec, port);
}

void setCmdLineParameter(std::string paramName, int paramValue){
	CmdLineModelParams.push_back(std::string("-") + paramName + std::string("=") + std::to_string(paramValue)); // add it as a cmdline options would appear
}

void setCmdLineParameter(std::string paramName, double paramValue){
	CmdLineModelParams.push_back(std::string("-") + paramName + std::string("=") + std::to_string(paramValue)); // add it as a cmdline options would appear
}

void setCmdLineParameter(std::string paramName, std::vector<double> paramValues){
	// array cmdline parameters appear as repeated lines for each value
	for (auto value : paramValues) {
		setCmdLineParameter(paramName, value);
	}
}

#endif
