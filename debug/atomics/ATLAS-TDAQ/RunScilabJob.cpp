#include "ATLAS-TDAQ/RunScilabJob.h"

void RunScilabJob::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type
	atInit=va_arg(parameters,char*);
	atDExt=va_arg(parameters,char*);
	atExit=va_arg(parameters,char*);

	sigma=1e10;
	if (strlen(atInit)!=0 && this->isScilabConfigured())
		executeVoidScilabJob(atInit,true);
}
double RunScilabJob::ta(double t) {
	//This function returns a double.
	return sigma;
}
void RunScilabJob::dint(double t) {

}
void RunScilabJob::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
	if (strlen(atDExt)!=0 && this->isScilabConfigured())
		executeVoidScilabJob(atDExt,true);
}
Event RunScilabJob::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)
	return Event();
}
void RunScilabJob::exit() {
	//Code executed at the end of the simulation.
	if (strlen(atExit)!=0 && this->isScilabConfigured())
		executeVoidScilabJob(atExit,true);
}

/**
 * TODO: warning: this assumes Scilab is not the defautl option. It assumes scilab is configured only if the options are explicitly set to use scilab
 */
bool RunScilabJob::isScilabConfigured(){
	std::string readingBackend = getReadingBackend();
	std::string loggingBackend = getLoggingBackend();

	return boost::iequals(readingBackend, SCILAB_READING_BACKEND_OPTION_VALUE) || boost::iequals(loggingBackend, SCILAB_LOGGING_BACKEND_OPTION_VALUE);
}
