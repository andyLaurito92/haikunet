//CPP:ATLAS-TDAQ/multipleSimulationCommands.cpp
//CPP:sinks/logtoscilabtool.cpp
//CPP:ATLAS-TDAQ/OpenTsdb/OpenTsdbLogger.cpp
#if !defined multipleSimulationCommands_h
#define multipleSimulationCommands_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <chrono>

#include "ATLAS-TDAQ/SimulationExperimentTracker.h"


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
