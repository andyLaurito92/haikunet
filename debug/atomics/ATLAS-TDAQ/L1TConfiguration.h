//CPP:ATLAS-TDAQ/L1TConfiguration.cpp
#if !defined L1TConfiguration_h
#define L1TConfiguration_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <vector>
#include <map>

#include "sinks/ParameterReader.h"

/***
 * This model exposes the configuration of ROBIds, ROSIds and their mapping using static methods (only one instance should exist)
 * Only the init function has behavior to read parameter and initialize the Ids.
 * If a model need to use these Ids in their init function, they need to have less priority than this model.
 */
class L1TConfiguration: public Simulator {
private:
	// parameters
	static int TotalROS;
	static int NChannelsPerRos;

	// Constants set at init
	static std::vector<int> RosIds;      // ROS IDs
	static std::vector<int> RobIds; // ROB (fragment) IDs
	static std::multimap<int, int> SourceToRobIds; // map associating the sources (ROSes) with the ROB IDs (Fragments) they provide.
	static std::multimap<int, int> RobIdToSource; // map associating the served ROB IDs with the source providing them.


public:
	L1TConfiguration(const char *n) : Simulator(n) { };
	~L1TConfiguration() { };
	virtual void init(double, ...);
	virtual double ta(double t);
	virtual void dint(double);
	virtual void dext(Event, double);
	virtual Event lambda(double);
	virtual void exit();

	static const std::vector<int>& getRobIds() { return L1TConfiguration::RobIds; }

	static const std::multimap<int, int>& getRobIdToSource() { return L1TConfiguration::RobIdToSource; }

	static const std::vector<int>& getRosIds() { return L1TConfiguration::RosIds; }

	static const std::multimap<int, int>& getSourceToRobIds() { return L1TConfiguration::SourceToRobIds;}

	static const int getTotalRos() { return L1TConfiguration::TotalROS; }

	static const int getFragmentsPerEvent() { return L1TConfiguration::getRobIds().size(); }

	static const int getChannelsPerRos() { return L1TConfiguration::NChannelsPerRos; }

	//static const int getRosMachinesPerRack() { return L1TConfiguration::rosMachinesPerRack; }

	// Given the ROS that is sending a message and the dcmId to where the message is directed, it returns which TcpSender index should handle it
	static const int getTcpSenderIndex(int dcmId, int rosId) { return L1TConfiguration::getTotalRos() * dcmId + rosId; }

	// Given the TcpSender index and the dcmId to where the message is directed, it returns which ROS index sent the message
	static const int getRosIndex(int dcmId, int tcpSenderIndex) { return tcpSenderIndex - (L1TConfiguration::getTotalRos() * dcmId); }

	// Given the ROS index, it returns which ROS ToR index the messages should be sent to
	//static const int getRosToRIndex(int rosId) { return rosId / L1TConfiguration::getRosMachinesPerRack(); }



};
#endif
