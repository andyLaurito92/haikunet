#include "L1TConfiguration.h"

// Static references
int L1TConfiguration::TotalROS;
int L1TConfiguration::NChannelsPerRos;
std::vector<int> L1TConfiguration::RosIds;      // ROS IDs
std::vector<int> L1TConfiguration::RobIds; // ROB (fragment) IDs
std::multimap<int, int> L1TConfiguration::SourceToRobIds; // map associating the sources (ROSes) with the ROB IDs (Fragments) they provide.
std::multimap<int, int> L1TConfiguration::RobIdToSource; // map associating the served ROB IDs with the source providing them.


void L1TConfiguration::init(double t, ...) {
	va_list parameters;
	va_start(parameters,t);

	char* fvar;

	fvar = va_arg(parameters,char*);
	L1TConfiguration::TotalROS = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: L1TConfiguration::TotalROS =  %i \n", t, this->getName(), L1TConfiguration::TotalROS);

	fvar = va_arg(parameters,char*);
	L1TConfiguration::NChannelsPerRos = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: L1TConfiguration::NChannelsPerRos =  %i \n", t, this->getName(), L1TConfiguration::NChannelsPerRos);

	// Initialize ROB & Fragment ids. TODO: Now just consecutive numbers
	for(int rosId=0; rosId < L1TConfiguration::TotalROS; rosId++) {
	       L1TConfiguration::RosIds.push_back(rosId);
	       for(int j=0; j < L1TConfiguration::NChannelsPerRos; j++) {
	    	   int robId = rosId * 100000 + j; // fragment Ids look like <ROBId>00000<FrID>
	    	   L1TConfiguration::SourceToRobIds.insert({rosId, robId});
	    	   L1TConfiguration::RobIdToSource.insert({robId, rosId});
	    	   L1TConfiguration::RobIds.push_back(robId);
	    	   //printLog(99, "[%f] %s_init: ROS=%i ROB=%i  \n", t, this->getName(), rosId, robId);
	       }
	}

	va_end(parameters);
	return;
}

double L1TConfiguration::ta(double t) {
	return INF;
}

void L1TConfiguration::dint(double t) {

}
void L1TConfiguration::dext(Event x, double t) {

}
Event L1TConfiguration::lambda(double t) {
	return Event();
}

void L1TConfiguration::exit() {

}
