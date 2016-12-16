//CPP:sinks/SimulationExperimentTracker.cpp
//CPP:common/BaseSimulator.cpp
//CPP:sinks/ParameterSweeper/CmdLineParameterSweeper.cpp
//CPP:sinks/ParameterSweeper/ScilabParameterSweeper.cpp
//CPP:sinks/ParameterReader.cpp
//CPP:sinks/logtoscilabtool.cpp
//CPP:sinks/Loggers/ScilabLogger.cpp
//CPP:sinks/Loggers/ConfigurationLogger.cpp
//CPP:sinks/Loggers/SamplerLogger.cpp
//CPP:hybrid/stdevstool.cpp
//CPP:hybrid/DistributionParameters/BaseDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/ConstantDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/ExponentialDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/ParetoDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/SplitDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/NormalDistributionParameter.cpp
//CPP:hybrid/DistributionParameters/FelixPeriodPerThreadDistributionParameter.cpp
//CPP:vector/VectorialModel.cpp
//CPP:vector/ScalarSimulator.cpp
#if !defined SimulationExperimentTracker_h
#define SimulationExperimentTracker_h


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
#include <memory>
#include <iostream>
#include <fstream>

//#include "queueingTheory/globals.h"
#include "sinks/ParameterSweeper/IParameterSweeper.h"
#include "sinks/ParameterSweeper/CmdLineParameterSweeper.h"
#include "sinks/ParameterSweeper/ScilabParameterSweeper.h"

// to be removed:
#include <iostream>

class SimulationExperimentTracker: public Simulator {

    // Constants accessible by all models
	static std::string simuSetID; // set ID for all simulations
	static std::string simuID; // set ID for one simulation run (one iteration)
	static int currentSimuRun;  // current number of simulation
	static int totalNumberOfRuns; // total number of simulations to be executed
	static std::chrono::system_clock::time_point wallClockExecutionStartTime; // start time of the current execution
	static std::chrono::system_clock::time_point wallClockSimulationStartTime; // start time of the simulation (not taking into account Initialization time)

	// Parameters
	char* simuRunVariableName;
	char* parameterValuesVariableName;
	char* parameterNamesVariableName;
	char* initSimulationCommandName;
	char* updateParamsCommandName;

public:
	SimulationExperimentTracker(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();

	/*!
	 * retrieve set ID for all simulations
	 */
	static std::string getSimuSetID() { return SimulationExperimentTracker::simuSetID;};

	/*!
	 * retrieve set ID for current simulation run. Returns a string with a timestamp
	 */
	static std::string getSimuID() { return SimulationExperimentTracker::simuID;};

	/*!
	 * retrieve current simulation number
	 */
	static int getCurrentSimuRun() { return SimulationExperimentTracker::currentSimuRun;};

	/*!
	 * retrieve total number of simulations to be executed
	 */
	static int getTotalNumberOfRuns() { return SimulationExperimentTracker::totalNumberOfRuns;};

	/*!
	 * retrieve start time of the current execution
	 */
	static std::chrono::system_clock::time_point getWallClockStartTime() { return SimulationExperimentTracker::wallClockExecutionStartTime;};

	/*!
	 * retrieve start time of the current simulation (not taking into account Initialization time).
	 */
	static std::chrono::system_clock::time_point getWallClockSimulationStartTime() { return SimulationExperimentTracker::wallClockSimulationStartTime;};

	/*!
	 * set start time of the current simulation (not taking into account Initialization time). WARNING: Should be used only by the last model being initialized
	 */
	static void setWallClockSimulationStartTime() { SimulationExperimentTracker::wallClockSimulationStartTime = std::chrono::system_clock::now();};

	static std::chrono::system_clock::duration getElapsedWallClockTimeSinceStart() { return std::chrono::system_clock::now() - getWallClockStartTime(); };

	static bool isLastRun() { return SimulationExperimentTracker::getCurrentSimuRun() + 1 >= SimulationExperimentTracker::getTotalNumberOfRuns(); };
	static bool isFirstRun() { return SimulationExperimentTracker::getCurrentSimuRun() <= 0; };

private:
	std::shared_ptr<IParameterSweeper> parameterSweeper;
};


#endif
