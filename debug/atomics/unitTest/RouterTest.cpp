#include <stdlib.h>
#include <iostream>

#include "PhaseI/Router.h" // test target
//#include "PhaseI/NetworkData/GlobalFlowBasedRoutingTable.h"

#include "MockClasses.h"

#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/included/unit_test.hpp>
BOOST_AUTO_TEST_SUITE(RouterTests); // add suite with test name



BOOST_AUTO_TEST_CASE(functional_withASingleEntry_shouldForwardsThroughAssignedPort)
{
	// test subject
	auto model = new Router("Router");

	// set up parameters
	std::vector<std::pair<double, Event>> events;
	CmdLineModelParams.clear();

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"pepe"} );

	// prepare routing table
	int outport = 1;
	GlobalFlowBasedRoutingTable::emptyTable();
	GlobalFlowBasedRoutingTable::addEntry("TestFlow", "Router", outport);

	// send one input packet
	auto packet = std::make_shared<NetworkPacket>(0);
	std::shared_ptr<IProtocol> routeProtocol = std::make_shared<FlowProtocol>("TestFlow");
	packet->addProtocol(routeProtocol);

	events.push_back({1, Event(packet, 0)}); // request dequeue --> 1 should be served


	root->setupExternalEvents(events);

	// run simulation
	auto generatedEvents = root->run(10);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), 1);

	//verify
	int eventIndex = 0;
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].first, 1); // t=1
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].second.port, outport); // queue 1
//	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<NetworkPacket>(generatedEvents[eventIndex].second.valuePtr)), packet); // queue 1
}

BOOST_AUTO_TEST_CASE(functional_withDifferentFlowIds_shouldForwardThroughCorrespondingFlowPort)
{
	// test subject
	auto model = new Router("Router");

	// set up parameters
	std::vector<std::pair<double, Event>> events;
	CmdLineModelParams.clear();

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"pepe"} );

	// prepare routing table
	GlobalFlowBasedRoutingTable::emptyTable();
	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router", 1);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow2", "Router", 5);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow2", "Router2", 9);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow3", "Router2", 3);

	// send 2 input packets
	auto packet = std::make_shared<NetworkPacket>(0);
	std::shared_ptr<IProtocol> routeProtocol = std::make_shared<FlowProtocol>("TestFlow1");
	packet->addProtocol(routeProtocol);

	events.push_back({1, Event(packet, 0)});

	// another packet with flow2
	packet = std::make_shared<NetworkPacket>(1);
	routeProtocol = std::make_shared<FlowProtocol>("TestFlow2");
	packet->addProtocol(routeProtocol);

	events.push_back({2, Event(packet, 0)});

	// run simulation
	root->setupExternalEvents(events);
	auto generatedEvents = root->run(10);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), 2);

	//verify
	int eventIndex = 0;
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].first, 1); // t=1
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].second.port, 1); // queue 1
	eventIndex++;
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].first, 2); // t=1
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].second.port, 5); // queue 1
//	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<NetworkPacket>(generatedEvents[eventIndex].second.valuePtr)), packet); // queue 1
}

BOOST_AUTO_TEST_CASE(functional_withNoDefinedFlows_shouldRaiseException)
{
	// test subject
	auto model = new Router("Router");

	// set up parameters
	std::vector<std::pair<double, Event>> events;
	CmdLineModelParams.clear();

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"pepe"} );

	// prepare routing table
	GlobalFlowBasedRoutingTable::emptyTable();

	// send 2 input packets
	auto packet = std::make_shared<NetworkPacket>(0);
	std::shared_ptr<IProtocol> routeProtocol = std::make_shared<FlowProtocol>("TestFlow1");
	packet->addProtocol(routeProtocol);

	events.push_back({1, Event(packet, 0)});

	// run simulation
	root->setupExternalEvents(events);
	BOOST_REQUIRE_THROW( root->run(10), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(functional_withDifferentFlowIdsDisordered_shouldForwardThroughCorrespondingFlowPort)
{
	// test subject
	auto model = new Router("Router");

	// set up parameters
	std::vector<std::pair<double, Event>> events;
	CmdLineModelParams.clear();

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"pepe"} );

	// prepare routing table
	GlobalFlowBasedRoutingTable::emptyTable();

	GlobalFlowBasedRoutingTable::addEntry("TestFlow3", "Router2", 3);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router3", 99);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router", 5);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow6", "Router", 77);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow2", "Router", 9);

	// send 2 input packets
	auto packet = std::make_shared<NetworkPacket>(0);
	std::shared_ptr<IProtocol> routeProtocol = std::make_shared<FlowProtocol>("TestFlow1");
	packet->addProtocol(routeProtocol);

	events.push_back({1, Event(packet, 0)});

	// another packet with flow2
	packet = std::make_shared<NetworkPacket>(1);
	routeProtocol = std::make_shared<FlowProtocol>("TestFlow2");
	packet->addProtocol(routeProtocol);

	events.push_back({2, Event(packet, 0)});

	// run simulation
	root->setupExternalEvents(events);
	auto generatedEvents = root->run(10);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), 2);

	//verify
	int eventIndex = 0;
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].first, 1); // t=1
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].second.port, 5); // queue 1
	eventIndex++;
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].first, 2); // t=1
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex].second.port, 9); // queue 1
//	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<NetworkPacket>(generatedEvents[eventIndex].second.valuePtr)), packet); // queue 1
}

BOOST_AUTO_TEST_CASE(functional_withMultipleOutPortsForFlow_shouldDoRoundRobinBetweenPorts)
{
	// test subject
	auto model = new Router("Router");

	// set up parameters
	std::vector<std::pair<double, Event>> events;
	CmdLineModelParams.clear();

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"pepe"} );

	// prepare routing table
	GlobalFlowBasedRoutingTable::emptyTable();

	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router", 0);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router", 1);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router3", 5);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow2", "Router", 77);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router", 2);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow2", "Router", 9);

	// send 2 input packets
	int numberOfPackets = 9;

	for(int i=0; i < numberOfPackets; i++){
		auto packet = std::make_shared<NetworkPacket>(i);
		std::shared_ptr<IProtocol> routeProtocol = std::make_shared<FlowProtocol>("TestFlow1");
		packet->addProtocol(routeProtocol);

		events.push_back({i, Event(packet, 0)});
	}
	// run simulation
	root->setupExternalEvents(events);
	auto generatedEvents = root->run(10);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), numberOfPackets);

	//verify
	std::vector<int> eventsPerPort(3);
	eventsPerPort[0] = 0;
	eventsPerPort[1] = 0;
	eventsPerPort[2] = 0;

	for(int i=0; i < numberOfPackets; i++){
		eventsPerPort[generatedEvents[i].second.port]++;
		BOOST_REQUIRE_EQUAL(generatedEvents[i].first, i); // t=i
	}

	BOOST_REQUIRE_EQUAL(eventsPerPort[0], 3); // number of out events in port 0
	BOOST_REQUIRE_EQUAL(eventsPerPort[1], 3); // number of out events in port 1
	BOOST_REQUIRE_EQUAL(eventsPerPort[2], 3); // number of out events in port 2
}

BOOST_AUTO_TEST_CASE(functional_withMultipleOutPortsForMultipleFlows_shouldDoRoundRobinBetweenPortsPerFlow)
{
	// test subject
	auto model = new Router("Router");

	// set up parameters
	std::vector<std::pair<double, Event>> events;
	CmdLineModelParams.clear();

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"pepe"} );

	// prepare routing table
	GlobalFlowBasedRoutingTable::emptyTable();

	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router", 0);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router", 1);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow1", "Router", 3);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow2", "Router", 4);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow2", "Router", 2);
	GlobalFlowBasedRoutingTable::addEntry("TestFlow2", "Router", 5);

	// send 2 input packets
	int numberOfPackets = 9;

	// flow 1
	for(int i=0; i < numberOfPackets; i++){
		auto packet = std::make_shared<NetworkPacket>(i);
		std::shared_ptr<IProtocol> routeProtocol = std::make_shared<FlowProtocol>("TestFlow1");
		packet->addProtocol(routeProtocol);

		events.push_back({i, Event(packet, 0)});
	}

	// flow 2
	for(int i=0; i < numberOfPackets; i++){
		auto packet = std::make_shared<NetworkPacket>(i+0.1);
		std::shared_ptr<IProtocol> routeProtocol = std::make_shared<FlowProtocol>("TestFlow2");
		packet->addProtocol(routeProtocol);

		events.push_back({i+0.1, Event(packet, 0)});
	}
	// run simulation
	root->setupExternalEvents(events);
	auto generatedEvents = root->run(10);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), numberOfPackets*2);

	//verify
	std::vector<int> eventsPerPort(3);
	eventsPerPort[0] = 0; // Flow 1
	eventsPerPort[1] = 0;
	eventsPerPort[2] = 0;
	eventsPerPort[3] = 0; // Flow 2
	eventsPerPort[4] = 0;
	eventsPerPort[5] = 0;
	for(int i=0, t=0; i < numberOfPackets*2; i++, t++){
		eventsPerPort[generatedEvents[i].second.port]++;
		BOOST_REQUIRE_EQUAL(generatedEvents[i].first, t); // t=i
		i++;
		eventsPerPort[generatedEvents[i].second.port]++;
		BOOST_REQUIRE_EQUAL(generatedEvents[i].first, t + 0.1); // t=i

	}

	BOOST_REQUIRE_EQUAL(eventsPerPort[0], 3); // number of out events in port 0
	BOOST_REQUIRE_EQUAL(eventsPerPort[1], 3); // number of out events in port 1
	BOOST_REQUIRE_EQUAL(eventsPerPort[2], 3); // number of out events in port 2
}



BOOST_AUTO_TEST_SUITE_END() // end suite test declaration


