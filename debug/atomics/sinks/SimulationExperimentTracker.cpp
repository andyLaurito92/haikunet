#include "SimulationExperimentTracker.h"


std::string SimulationExperimentTracker::simuSetID = "";
std::string SimulationExperimentTracker::simuID = new char[80];
int SimulationExperimentTracker::currentSimuRun = -1;
int SimulationExperimentTracker::totalNumberOfRuns = -1;
std::chrono::system_clock::time_point SimulationExperimentTracker::wallClockExecutionStartTime = std::chrono::system_clock::now();
std::chrono::system_clock::time_point SimulationExperimentTracker::wallClockSimulationStartTime = std::chrono::system_clock::now();


void SimulationExperimentTracker::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);

	// Update the start time of the simulation, as this should be the first model to get initialized
	SimulationExperimentTracker::wallClockExecutionStartTime = std::chrono::system_clock::now();

	// Set Simu ID using the current timestamp
	char simuIdstr[1024];
	sprintf(simuIdstr, "%li", time(NULL));
	SimulationExperimentTracker::simuID = std::string(simuIdstr);

	// read parameters
	char* fvar = va_arg(parameters,char*);
	SimulationExperimentTracker::simuSetID = std::string(fvar);

	fvar = va_arg(parameters,char*);
	simuRunVariableName =  fvar;

	fvar = va_arg(parameters,char*);
	parameterValuesVariableName = fvar;

	fvar = va_arg(parameters,char*);
	parameterNamesVariableName = fvar;

	// Create ParameterSweeper according to the configuration
	std::string parameterReadingBackend = getReadingBackend();

	printLog(LOG_LEVEL_FULL_LOGGING, "[INIT] %s_init: reading backend = %s  \n", this->getName(), parameterReadingBackend.data());
	if(boost::iequals(parameterReadingBackend, SCILAB_READING_BACKEND_OPTION_VALUE)){ // Scilab parameter sweeper configured
		parameterSweeper = std::make_shared<ScilabParameterSweeper>(simuRunVariableName, parameterValuesVariableName, parameterNamesVariableName);
	} else { // by default use CmdLine
		parameterSweeper = std::make_shared<CmdLineParameterSweeper>(simuRunVariableName, parameterValuesVariableName, parameterNamesVariableName);
	}

	// Update current simulation run based on the cmdLine parameter
	SimulationExperimentTracker::currentSimuRun = parameterSweeper->getCurrentExperimentNumber();
	SimulationExperimentTracker::totalNumberOfRuns = parameterSweeper->getTotalNumberOfExperiments();

	// Simulation Run number has been set, now print parameters:
	printLog(LOG_LEVEL_IMPORTANT, "[INIT] %s_init: simulation ID : %s \n", this->getName(), SimulationExperimentTracker::getSimuID().data());
	printLog(LOG_LEVEL_IMPORTANT, "[INIT] %s_init: VariableName for simu SetID : %s \n", this->getName(), SimulationExperimentTracker::getSimuSetID().data());
	printLog(LOG_LEVEL_IMPORTANT, "[INIT] %s: Variable Name for Simulation Run : %s \n", this->getName(), simuRunVariableName);
	printLog(LOG_LEVEL_IMPORTANT, "[INIT] %s_init: VariableName for parameters values : %s \n", this->getName(), parameterValuesVariableName);
	printLog(LOG_LEVEL_IMPORTANT, "[INIT] %s_init: VariableName for parameters names : %s \n", this->getName(), parameterNamesVariableName);

	// For CmdLine parameters
	if(!boost::iequals(parameterReadingBackend, SCILAB_READING_BACKEND_OPTION_VALUE)){
		// read the simuSetID from config file (not possible from Scilab)
		SimulationExperimentTracker::simuSetID = getCmdLineParam<std::string>(SimulationExperimentTracker::simuSetID);
		printLog(LOG_LEVEL_IMPORTANT, "[INIT] %s_init: simuSetID : %s \n", this->getName(), SimulationExperimentTracker::getSimuSetID().data());

		// print parameters them to the log file
		// TODO: record them in OpenTSDB if configured
		std::ostringstream ostr;
		std::copy(CmdLineModelParams.begin(), CmdLineModelParams.end(), std::ostream_iterator<std::string>(ostr, " "));
		printLog(LOG_LEVEL_IMPORTANT, "[INIT] %s_init: Commnad line parameters: %s \n", this->getName(), ostr.str().data() );
	}

	// Validate Run Number
	if(SimulationExperimentTracker::currentSimuRun >= SimulationExperimentTracker::totalNumberOfRuns){
		printLog(LOG_LEVEL_ERROR, "SimulationExperimentTracker: Invalid Simulation Experiment Number. Attempting to execute Experiment %i (0-based) when only %i parameters are defined. \n", SimulationExperimentTracker::currentSimuRun, SimulationExperimentTracker::totalNumberOfRuns);
		throw std::runtime_error("SimulationExperimentTracker: Invalid Simulation Experiment Number. Check log for details. \n");
	}

	// Update parameters
	parameterSweeper->UpdateParameters();

	// print initialization wall clock time time
	auto now_c = std::chrono::system_clock::to_time_t(SimulationExperimentTracker::wallClockExecutionStartTime);
	char timestr[100];
	std::strftime(timestr, sizeof(timestr), "%Y%m%d_%H%M%S", std::localtime(&now_c));
	printLog(LOG_LEVEL_IMPORTANT, "---------------------- STARTING simulation Experiment number %u (0-based) of a total of %u Experiments %s. Wall-clock time: %s  --------------------- \n",
				SimulationExperimentTracker::getCurrentSimuRun(), SimulationExperimentTracker::getTotalNumberOfRuns(), SimulationExperimentTracker::isLastRun()?"(last Experiment)":"" , timestr);


	return;
}

double SimulationExperimentTracker::ta(double t) { return 1E10;  } //INF

void SimulationExperimentTracker::dint(double t) { return; }

void SimulationExperimentTracker::dext(Event x, double t) { }

Event SimulationExperimentTracker::lambda(double t) { return Event(); }

void SimulationExperimentTracker::exit() {
	auto initTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - SimulationExperimentTracker::getWallClockSimulationStartTime());
	printLog(LOG_LEVEL_IMPORTANT, "---------------------- Finished Simulation, now EXITING all models. Simulation time (not including init): %i (ms) ----------------------\n", initTime.count());

	parameterSweeper->onExit();

	//TODO: this is to fix include compilation problems:
	// for some reason the templating is not getting the readDefaultParameterValue<double> method for the BaseDistributionParameter class. This line forces it to create the method for <double>.. others might be needed later..

	readDefaultParameterValue<double>("");
}


