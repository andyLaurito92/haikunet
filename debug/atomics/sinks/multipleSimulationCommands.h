//CPP:sinks/multipleSimulationCommands.cpp
//CPP:sinks/logtoscilabtool.cpp
//#ifdef OPENTSDB CPP:CPP:sinks/OpenTsdb/OpenTsdbLogger.cpp
//#ifdef OPENTSDB CPP:sinks/OpenTsdb/TelnetOpenTsdbClient.cpp
#ifndef multipleSimulationCommands_h
#define multipleSimulationCommands_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <chrono>

#include "SimulationExperimentTracker.h"


class multipleSimulationCommands: public Simulator {
// Declare the state,
// output variables
// and parameters

	// Parameters
	char* eachSimulationCommandName;
	char* lastSimulationCommandName;
	char* initSimulationCommandName;

	//	double sigma; // always INF


public:
	multipleSimulationCommands(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
private:
	bool isScilabConfigured();
	void runAfterEachSimulation();
	void runAfterAllSimulations();
	void runAtInitOfSimulation();


};
#endif
