//CPP:hybrid/ScilabSimulationsTracker.cpp
#if !defined ScilabSimulationsTracker_h
#define ScilabSimulationsTracker_h


#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <ctime>
#include <time.h>

// to be removed:
#include <iostream>

class ScilabSimulationsTracker: public Simulator {
// Declare the state,
// output variables
// and parameters

	static char* simuSetID; // set ID for all simulations
	static char* simuID; // set ID for one simulation run (one iteration)
	static int currentSimuRun;  // current number of simulation
	static int totalNumberOfRuns; // total number of simulations to be executed
	static std::chrono::system_clock::time_point wallClockStartTime; // total number of simulations to be executed

	// Parameters
	char* simuCounterVariableName;
	char* parameterValuesVariableName;
	char* parameterNamesVariableName;
	char* initSimulationCommandName;
	char* updateParamsCommandName;

//	double sigma; // always INF
	char* parameterNames[20];



public:
	ScilabSimulationsTracker(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();

	/*!
	 * retrieve set ID for all simulations
	 */
	static char* getSimuSetID() { return ScilabSimulationsTracker::simuSetID;};

	/*!
	 * retrieve set ID for current simulation run. Returns a string with a timestamp
	 */
	static char* getSimuID() { return ScilabSimulationsTracker::simuID;};

	/*!
	 * retrieve current simulation number
	 */
	static int getCurrentSimuRun() { return ScilabSimulationsTracker::currentSimuRun;};

	/*!
	 * retrieve total number of simulations to be executed
	 */
	static int getTotalNumberOfRuns() { return ScilabSimulationsTracker::totalNumberOfRuns;};

	/*!
	 * retrieve start time of the current simulation
	 */
	static std::chrono::system_clock::time_point getWallClockStartTime() { return ScilabSimulationsTracker::wallClockStartTime;};

	static std::chrono::system_clock::duration getElapsedWallClockTimeSinceStart() { return std::chrono::system_clock::now() - getWallClockStartTime(); };

	static bool isLastRun() { return ScilabSimulationsTracker::getCurrentSimuRun() + 1 >= ScilabSimulationsTracker::getTotalNumberOfRuns(); };
	static bool isFirstRun() { return ScilabSimulationsTracker::getCurrentSimuRun() <= 0; };


private:
	void updateParameters();
	void runAtInitOfSimulation();

};


#endif
