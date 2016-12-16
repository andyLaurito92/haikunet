#include <stdlib.h>
#include <iostream>

#include "PhaseI/FelixServer.h" // test target

#include "MockClasses.h"
#include "hybrid/DistributionParameters/BaseDistributionParameter.h"

#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(FelixServerTests); // add suite with test name

class MockDistributionParameter : public BaseDistributionParameter {
#define PARAMETER_VALUE_NAME "NONE"

private:
	std::vector<double> values;
	int actualValue;

public:
	MockDistributionParameter(std::vector<double> values) : BaseDistributionParameter() {
		this->values = values;
		this->actualValue = 0;
		printLog(LOG_LEVEL_DEBUG, "Using a MockDistributionParameter \n");
	}

	double getNewRandomValue() {
		if( this->actualValue >= this->values.size() ) this->actualValue = 0;

		return this->values[this->actualValue++];
	}

	std::vector<std::string> getParameterNames(){
		return std::vector<std::string> { PARAMETER_VALUE_NAME } ;
	};

	std::string getName() {
		return std::string("MockDistributionParameter");
	}
};

BOOST_AUTO_TEST_CASE(functional_withTwoFlowsDefined_shouldGenerateFirstFlow1)
{

	std::shared_ptr<ConstantDistributionParameter> flowFlow0_1PeriodDistribution = std::make_shared<ConstantDistributionParameter>();
	std::shared_ptr<ConstantDistributionParameter> flowFlow0_1PacketSizeDistribution = std::make_shared<ConstantDistributionParameter>();

	std::shared_ptr<ConstantDistributionParameter> flowFlow1_1PeriodDistribution = std::make_shared<ConstantDistributionParameter>();
	std::shared_ptr<ConstantDistributionParameter> flowFlow1_1PacketSizeDistribution = std::make_shared<ConstantDistributionParameter>();


	auto flow0 = std::make_shared<Flow>("lar_felix_0_FlowID", 0 /*startTime*/,
			0 /*typeOfService*/, flowFlow0_1PeriodDistribution, flowFlow0_1PacketSizeDistribution);

	auto flow1 = std::make_shared<Flow>("lar_felix_1_FlowID", 5 /*startTime*/,
				0 /*typeOfService*/, flowFlow1_1PeriodDistribution, flowFlow1_1PacketSizeDistribution);

	FlowDefinitions::emptyFlowsDefinition();
	FlowDefinitions::registerFlowSourceNode(flow0, "lar_felix_0.GeneratorApplication");
	FlowDefinitions::registerFlowSourceNode(flow1, "lar_felix_0.GeneratorApplication");

	// test subject
	auto model = new FelixServer("lar_felix_0.GeneratorApplication");

	// set up parameters
	CmdLineModelParams.clear();

	setCmdLineParameter("flowFlow01PeriodDistribution_value", 1);
	setCmdLineParameter("flowFlow01PacketSizeDistribution_value", 1*1000);
	setCmdLineParameter("flowFlow11PeriodDistribution_value", 1);
	setCmdLineParameter("flowFlow11PacketSizeDistribution_value", 1*1000);

	flowFlow0_1PeriodDistribution->readParameters("flowFlow01PeriodDistribution");
	flowFlow0_1PacketSizeDistribution->readParameters("flowFlow01PacketSizeDistribution");
	flowFlow1_1PeriodDistribution->readParameters("flowFlow11PeriodDistribution");
	flowFlow1_1PacketSizeDistribution->readParameters("flowFlow11PacketSizeDistribution");

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"pepe"} );

	auto generatedEvents = root->run(10);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), 15);

	//verify
	std::vector<std::string> flows = {"lar_felix_0_FlowID", "lar_felix_1_FlowID"};
	std::vector<std::pair<int, Event>> secondFlowEvents;
	std::vector<std::pair<int, Event>> firstFlowEvents;

	for(std::pair<int, Event> eventGenerated : generatedEvents ){
		auto packet = std::static_pointer_cast<NetworkPacket>(eventGenerated.second.valuePtr);

		BOOST_REQUIRE_EQUAL(packet->getLength_bits(), 1*1000);

		std::shared_ptr<FlowProtocol> flowProtocol = std::static_pointer_cast<FlowProtocol>(packet->getProtocol(PROTOCOL_ID_FLOW));
		std::string flowId = flowProtocol->getFlowId();
		if( flowId.compare("lar_felix_1_FlowID") == 0 ){
			secondFlowEvents.push_back( (std::pair<int, Event>) eventGenerated);
		}else {
			firstFlowEvents.push_back(eventGenerated);
		}

		if(eventGenerated.first < 6){
			BOOST_REQUIRE_EQUAL(flowId, "lar_felix_0_FlowID"); // t=1
		}
	}

	BOOST_REQUIRE_EQUAL(secondFlowEvents.size(), 5);
	BOOST_REQUIRE_EQUAL(firstFlowEvents.size(), 10);

	int time = 6;
	for(std::pair<int, Event> eventGenerated : secondFlowEvents ){
		BOOST_REQUIRE_EQUAL(eventGenerated.first, time);
		time++;
	}
}

void checkEventsGeneratedByFlow(std::vector<std::pair<double, Event>> flowEvents, int sizeOfFlowEvents, std::vector<double> times, std::vector<double> packetsSizes){
	BOOST_REQUIRE_EQUAL(flowEvents.size(), sizeOfFlowEvents);

	int index = 0;
	double time = 0.0;
	for(std::pair<double, Event> eventGenerated : flowEvents ){
		time += times[index];
		BOOST_REQUIRE_EQUAL(eventGenerated.first, time);
		auto packet = std::static_pointer_cast<NetworkPacket>(eventGenerated.second.valuePtr);
		BOOST_REQUIRE_EQUAL(packet->getLength_bits(), packetsSizes[index]);
		index++;
	}
}

BOOST_AUTO_TEST_CASE(functional_withThreeFlowsDefined_packetsAreGeneratedInOrder)
{
	std::vector<double> flow0argumentsPeriodDistribution = {0.0, 2.0, 2.0, 3.0, 2.0, 2.0};
	std::vector<double> flow0argumentsSizeDistribution = {1000.0, 1500.0, 500.0, 800.0, 300.0};
	std::shared_ptr<MockDistributionParameter> flow0PeriodDistribution = std::make_shared<MockDistributionParameter>(flow0argumentsPeriodDistribution);
	std::shared_ptr<MockDistributionParameter> flow0PacketSizeDistribution = std::make_shared<MockDistributionParameter>(flow0argumentsSizeDistribution);


	std::vector<double> flow1argumentsPeriodDistribution = {0.0, 1.0, 3.0, 1.0, 1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, 0.5};
	std::vector<double> flow1argumentsSizeDistribution = {800.0, 1000.0, 300.0, 150.0, 750.0, 900.0, 600.0, 873.0, 458.0, 473.0, 67.0};
	std::shared_ptr<MockDistributionParameter> flow1PeriodDistribution = std::make_shared<MockDistributionParameter>(flow1argumentsPeriodDistribution);
	std::shared_ptr<MockDistributionParameter> flow1PacketSizeDistribution = std::make_shared<MockDistributionParameter>(flow1argumentsSizeDistribution);

	std::vector<double> flow2argumentsPeriodDistribution = {0.0, 0.5, 8.0, 12.0};
	std::vector<double> flow2argumentsSizeDistribution = {500.0, 500.0, 500.0, 500.0};
	std::shared_ptr<MockDistributionParameter> flow2PeriodDistribution = std::make_shared<MockDistributionParameter>(flow2argumentsPeriodDistribution);
	std::shared_ptr<MockDistributionParameter> flow2PacketSizeDistribution = std::make_shared<MockDistributionParameter>(flow2argumentsSizeDistribution);

	auto flow0 = std::make_shared<Flow>("lar_felix_0_FlowID", 0 /*startTime*/,
			0 /*typeOfService*/, flow0PeriodDistribution, flow0PacketSizeDistribution);

	auto flow1 = std::make_shared<Flow>("lar_felix_1_FlowID", 0 /*startTime*/,
				0 /*typeOfService*/, flow1PeriodDistribution, flow1PacketSizeDistribution);

	auto flow2 = std::make_shared<Flow>("lar_felix_2_FlowID", 0 /*startTime*/,
					0 /*typeOfService*/, flow2PeriodDistribution, flow2PacketSizeDistribution);

	FlowDefinitions::emptyFlowsDefinition();
	FlowDefinitions::registerFlowSourceNode(flow0, "lar_felix_0.GeneratorApplication");
	FlowDefinitions::registerFlowSourceNode(flow1, "lar_felix_0.GeneratorApplication");
	FlowDefinitions::registerFlowSourceNode(flow2, "lar_felix_0.GeneratorApplication");

	// test subject
	auto model = new FelixServer("lar_felix_0.GeneratorApplication");

	// set up parameters
	CmdLineModelParams.clear();

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"pepe"} );

	auto generatedEvents = root->run(10);

	std::vector<std::string> flows = {"lar_felix_0_FlowID", "lar_felix_1_FlowID", "lar_felix_2_FlowID"};
	std::vector<std::pair<double, Event>> firstFlowEvents;
	std::vector<std::pair<double, Event>> secondFlowEvents;
	std::vector<std::pair<double, Event>> thirdFlowEvents;

	for(std::pair<double, Event> eventGenerated : generatedEvents ){
		auto packet = std::static_pointer_cast<NetworkPacket>(eventGenerated.second.valuePtr);

		std::shared_ptr<FlowProtocol> flowProtocol = std::static_pointer_cast<FlowProtocol>(packet->getProtocol(PROTOCOL_ID_FLOW));
		std::string flowId = flowProtocol->getFlowId();
		if( flowId.compare("lar_felix_0_FlowID") == 0 ){
			firstFlowEvents.push_back(eventGenerated);
		}else if(flowId.compare("lar_felix_1_FlowID") == 0){
			secondFlowEvents.push_back(eventGenerated);
		}else{
			thirdFlowEvents.push_back(eventGenerated);
		}
	}

	checkEventsGeneratedByFlow(firstFlowEvents, 5, {0.0, 2.0, 2.0, 3.0, 2.0}, {1000.0, 1500.0, 500.0, 800.0, 300.0});
	checkEventsGeneratedByFlow(secondFlowEvents, 11, {0.0, 1.0, 3.0, 1.0, 1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5}, {800.0, 1000.0, 300.0, 150.0, 750.0, 900.0, 600.0, 873.0, 458.0, 473.0, 67.0});
	checkEventsGeneratedByFlow(thirdFlowEvents, 3, {0.0, 0.5, 8.0}, {500.0, 500.0, 500.0});
}

BOOST_AUTO_TEST_SUITE_END() // end suite test declaration


