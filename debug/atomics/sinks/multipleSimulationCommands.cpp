#include "multipleSimulationCommands.h"
/// tmp
#include "root_coupling.h"
///

void multipleSimulationCommands::runAtInitOfSimulation() {
	if(!this->isScilabConfigured()){
		return;
	}

	char command[1024];
	if(initSimulationCommandName)
	{
		printLog(1, "\n[initSimulationCommandName] executing '%s' command ----------\n", initSimulationCommandName);
		sprintf(command,"exec('%s', 0)",initSimulationCommandName);
		executeVoidScilabJob(command,true);
	}
}

void multipleSimulationCommands::runAfterEachSimulation() {
	if(!this->isScilabConfigured()){
		return;
	}

	char command[1024];
	if(eachSimulationCommandName)
	{
		printLog(1, "\n[runAfterEachSimulation] executing '%s' command ----------\n", eachSimulationCommandName);
		sprintf(command,"exec('%s', 0)",eachSimulationCommandName);
		executeVoidScilabJob(command,true);
	}
}

void multipleSimulationCommands::runAfterAllSimulations() {
	if(!this->isScilabConfigured()){
		return;
	}

	char command[1024];
	if(lastSimulationCommandName){
		printLog(1, "\n[runAfterAllSimulations] executing '%s' command----------\n", lastSimulationCommandName);
		sprintf(command,"exec('%s', 0)",lastSimulationCommandName);
		executeVoidScilabJob(command,true);
	}
}

void multipleSimulationCommands::init(double t, ...) {
//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//  %Name% is the parameter name
//	  %Type% is the parameter type

	char* fvar = va_arg(parameters,char*);
	initSimulationCommandName = fvar;
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: initSimulationCommandName : %s \n", this->getName(), initSimulationCommandName);

	fvar = va_arg(parameters,char*);
	eachSimulationCommandName = fvar;
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: eachSimulationCommandName : %s \n", this->getName(), eachSimulationCommandName);

	fvar = va_arg(parameters,char*);
	lastSimulationCommandName = fvar;
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: lastSimulationCommandName : %s \n", this->getName(), lastSimulationCommandName);

	if (SimulationExperimentTracker::isFirstRun()){
		runAtInitOfSimulation();
	}

	// Finished initialization of all models, as this model must be the last one to be initialized
	auto initTime = std::chrono::duration_cast<std::chrono::milliseconds>(SimulationExperimentTracker::getElapsedWallClockTimeSinceStart());
	printLog(LOG_LEVEL_IMPORTANT, "---------------------- Finished initializing all models, now STARTING SIMULATION. Initialization TOTAL time: %i (ms) ----------------------\n", initTime.count());

	SimulationExperimentTracker::setWallClockSimulationStartTime();

	return;
}

double multipleSimulationCommands::ta(double t) {
	// Check this model is set to be executed last
	auto papa = (RootCoupling*) this->father;
	if(papa->D[papa->Dsize-1] != this){
		printLog(LOG_LEVEL_ERROR, "multipleSimulationCommands: '%s' model MUST be set with the least priority and now the least priority model is '%s'. \n", this->getName(), papa->D[papa->Dsize-1]->getName()  );
		throw std::runtime_error("multipleSimulationCommands model is not be set with the least priority. See log for details.");
	}
	return std::numeric_limits<double>::infinity();
}

void multipleSimulationCommands::dint(double t) {
	return;
}
void multipleSimulationCommands::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition
}
Event multipleSimulationCommands::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)

	return Event();

}
void multipleSimulationCommands::exit() {
	// Print Logging times
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Logging times for Scilab: \n ");
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total logged models :  %i  \n ", ScilabLogger::TotalLoggedModels);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total logged variables :  %i  \n ", ScilabLogger::TotalLoggedVariables);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total flushing time:  %i (ms) \n ", ScilabLogger::TotalFlushingTime_ms);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total writing time:  %i (us) \n ", ScilabLogger::TotalWrittingTime_us);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total metric points (not including T):  %i \n ", ScilabLogger::TotalMetricPoints);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: ---------------- ");

#ifdef OPENTSDB
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Logging times for OpenTsdb: \n ");
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total logged models :  %i  \n ", OpenTsdbLogger::TotalLoggedModels);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total logged variables :  %i  \n ", OpenTsdbLogger::TotalLoggedVariables);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total flushing time :  %i (us) \n ", OpenTsdbLogger::TotalFlushingTime_us);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total writing time :  %i (us) \n ", OpenTsdbLogger::TotalWrittingTime_us);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: Total metric points :  %i \n ", OpenTsdbLogger::TotalMetricPoints);
	printLog(LOG_LEVEL_IMPORTANT,"[multipleSimulationCommands]: ---------------- ");
#endif

	// Finished execution of all models (as this model must be the last one)
	auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(SimulationExperimentTracker::getElapsedWallClockTimeSinceStart());
	auto now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char timestr[100];
	std::strftime(timestr, sizeof(timestr), "%Y%m%d_%H%M%S", std::localtime(&now_c));
	printLog(LOG_LEVEL_IMPORTANT, "[FINISH] Finished Simulation Run %i. TOTAL execution time: %i (ms) . Wall-clock time: %s\n", SimulationExperimentTracker::getCurrentSimuRun(), elapsedTime.count(), timestr);

	this->runAfterEachSimulation();

	if (SimulationExperimentTracker::isLastRun()) {
		this->runAfterAllSimulations();
	}
}

/**
 * TODO: warning: this assumes Scilab is not the defautl option. It assumes scilab is configured only if the options are explicitly set to use scilab
 */
bool multipleSimulationCommands::isScilabConfigured(){
	std::string parameterReadingBackend = getReadingBackend();
	std::string variablesLoggingBackend = getLoggingBackend();

	return boost::iequals(parameterReadingBackend, SCILAB_READING_BACKEND_OPTION_VALUE) || boost::iequals(variablesLoggingBackend, SCILAB_LOGGING_BACKEND_OPTION_VALUE);
}
