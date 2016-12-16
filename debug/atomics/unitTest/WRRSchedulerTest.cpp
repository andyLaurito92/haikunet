#include <stdlib.h>
#include <iostream>

#include "PhaseI/WRRScheduler.h" // test target

#include "MockClasses.h"

#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/included/unit_test.hpp>
BOOST_AUTO_TEST_SUITE(WRRSchedulerTests); // add suite with test name

class WRRScheduler_wrapper : public WRRScheduler { // just exposes the protected members
public:
	WRRScheduler_wrapper(const char *n): WRRScheduler(n) {};

	int getWRRQueueIndex() {  return this->WRRQueueIndex; }
	std::vector<int>& getQueueSizes() { return this->queueSizes_bytes;}
	std::vector<double>& getQueueWeights() { return this->queueWeights;}

};


BOOST_AUTO_TEST_CASE(init_shouldInitializeVariables )
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;

	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1,2,3,4, 5, 6, 7, 8});
	model->init(0.0,"8", "weights");

	// starts waiting for ever
	BOOST_REQUIRE_EQUAL(model->ta(0), std::numeric_limits<double>::infinity());

	// queueSize
	BOOST_REQUIRE_EQUAL(model->getQueueSizes().size(), 8);
	auto it =std::find_if (model->getQueueSizes().begin(), model->getQueueSizes().end(), [](int queueSize){ return queueSize != 0; } );
	BOOST_REQUIRE_MESSAGE(it == model->getQueueSizes().end(), "not all queueSizes are initialized in 0"); // all queues to 0

	// queueWeights
	BOOST_REQUIRE_EQUAL(model->getQueueWeights().size(), 8);
	BOOST_REQUIRE_EQUAL(model->getQueueWeights()[0], 1);
	BOOST_REQUIRE_EQUAL(model->getQueueWeights()[1], 2);
	BOOST_REQUIRE_EQUAL(model->getQueueWeights()[2], 3);
	BOOST_REQUIRE_EQUAL(model->getQueueWeights()[6], 7);
	BOOST_REQUIRE_EQUAL(model->getQueueWeights()[7], 8);
}

BOOST_AUTO_TEST_CASE(init_shouldThrowIfWeightsDontMatchNumberOfQueues )
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;

	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1,2,3,4, 5, 6});

	BOOST_REQUIRE_THROW(model->init(0.0,"8", "weights");, std::runtime_error);
}

BOOST_AUTO_TEST_CASE(dext_port0_shouldRecordQueueSizes )
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});
	model->init(0.0,"8", "weights");

	double size = 400;
	model->dext(createEventWithVectorial(&size, 0, 3), 0);


	auto it =std::find_if (model->getQueueSizes().begin(), model->getQueueSizes().end(), [](int queueSize){ return queueSize != 0; } );
	BOOST_REQUIRE_MESSAGE(it != model->getQueueSizes().end(), "queueSizes was not stored");
	BOOST_REQUIRE_MESSAGE(it == model->getQueueSizes().begin() + 3, "queueSizes was stored in the wrong position");
	BOOST_REQUIRE_MESSAGE(*it == 400, "queueSize was stored with the wrong value");
	BOOST_REQUIRE_EQUAL(model->ta(0), std::numeric_limits<double>::infinity());

	size = 900;
	model->dext(createEventWithVectorial(&size, 0, 1), 0);

	it =std::find_if (model->getQueueSizes().begin(), model->getQueueSizes().end(), [](int queueSize){ return queueSize != 0; } );
	BOOST_REQUIRE_MESSAGE(it != model->getQueueSizes().end(), "queueSizes was not stored");
	BOOST_REQUIRE_MESSAGE(it == model->getQueueSizes().begin() + 1, "queueSizes was stored in the wrong position");
	BOOST_REQUIRE_MESSAGE(*it == 900, "queueSize was stored with the wrong value");
	BOOST_REQUIRE_EQUAL(model->ta(0), std::numeric_limits<double>::infinity());
}

BOOST_AUTO_TEST_CASE(dext_port0_shouldOverrideQueueSizes )
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});
	model->init(0.0,"8", "weights");

	double size = 400;
	model->dext(createEventWithVectorial(&size, 0, 3), 0);

	auto it =std::find_if (model->getQueueSizes().begin(), model->getQueueSizes().end(), [](int queueSize){ return queueSize != 0; } );
	BOOST_REQUIRE_MESSAGE(it != model->getQueueSizes().end(), "queueSizes was not stored");
	BOOST_REQUIRE_MESSAGE(it == model->getQueueSizes().begin() + 3, "queueSizes was stored in the wrong position");
	BOOST_REQUIRE_MESSAGE(*it == 400, "queueSize was stored with the wrong value");
	BOOST_REQUIRE_EQUAL(model->ta(0), std::numeric_limits<double>::infinity());

	size = 900;
	model->dext(createEventWithVectorial(&size, 0, 3), 0);

	it =std::find_if (model->getQueueSizes().begin(), model->getQueueSizes().end(), [](int queueSize){ return queueSize != 0; } );
	BOOST_REQUIRE_MESSAGE(it != model->getQueueSizes().end(), "queueSizes was not stored");
	BOOST_REQUIRE_MESSAGE(it == model->getQueueSizes().begin() + 3, "queueSizes was stored in the wrong position");
	BOOST_REQUIRE_MESSAGE(*it == 900, "queueSize was not overriden");
	BOOST_REQUIRE_EQUAL(model->ta(0), std::numeric_limits<double>::infinity());
}

BOOST_AUTO_TEST_CASE(dext_port1_shouldThrowIfARequestArrivesBeforePreviousWasSent )
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});
	model->init(0.0,"8", "weights");

	// set the size for queue 3
	double size = 400;
	model->dext(createEventWithVectorial(&size, 0, 3), 0);

	// request dequeue
	model->dext(Event(&size, 1), 0);

	BOOST_REQUIRE_THROW(model->dext(Event(&size, 1), 0);, std::runtime_error);
}

BOOST_AUTO_TEST_CASE(dext_port1_shouldProgramImmediateLambaIfQueuesNotEmpty )
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});
	model->init(0.0,"8", "weights");

	// set the size for a queue
	double size = 400;
	model->dext(createEventWithVectorial(&size, 0, 3), 0);

	// request dequeue
	model->dext(Event(&size, 1), 0);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0);
}

BOOST_AUTO_TEST_CASE(dext_port1_shouldSetIndexToFirstNonEmptyQueue )
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});
	model->init(0.0,"8", "weights");

	// set the size for queue 3
	double size = 400;
	model->dext(createEventWithVectorial(&size, 0, 3), 0);
	model->dext(createEventWithVectorial(&size, 0, 1), 0);

	// request dequeue
	model->dext(Event(&size, 1), 0);

	BOOST_REQUIRE_EQUAL(model->getWRRQueueIndex(), 1);
	BOOST_REQUIRE_EQUAL(model->ta(0), 0);
}

BOOST_AUTO_TEST_CASE(lambda_shouldSendDequeueRequestWithAdequateIndex)
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});
	model->init(0.0,"8", "weights");


	double size = 400;
	model->dext(createEventWithVectorial(&size, 0, 3), 0); // set the size for queue 3
	model->dext(createEventWithVectorial(&size, 0, 1), 0); // set the size for queue 3
	model->dext(Event(&size, 1), 0);	// request dequeue

	// validate state
	BOOST_REQUIRE_EQUAL(model->getWRRQueueIndex(), 1);
	BOOST_REQUIRE_EQUAL(model->ta(0), 0);

	Event output = model->lambda(0);	// request dequeue
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	auto vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, model->getWRRQueueIndex()); // dequeue to queue 1
}

BOOST_AUTO_TEST_CASE(dint_shouldSetTaToInfinite)
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});
	model->init(0.0,"8", "weights");


	double size = 400;
	model->dext(createEventWithVectorial(&size, 0, 1), 0); // set the size for queue 3
	model->dext(Event(&size, 1), 0);	// request dequeue
	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=0
	Event output = model->lambda(0);	// request dequeue

	model->dint(0);	// after the lambda
	BOOST_REQUIRE_EQUAL(model->ta(0), std::numeric_limits<double>::infinity()); // validate ta=inf
}

BOOST_AUTO_TEST_CASE(functional_WRRwithEqualWeights)
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});
	model->init(0.0,"8", "weights");
	double size = 400;

	// set some queue sizes
	model->dext(createEventWithVectorial(&size, 0, 3), 0); // set the size for queue 3
	model->dext(createEventWithVectorial(&size, 0, 1), 0); // set the size for queue 1

	// request dequeue t=0
	model->dext(Event(&size, 1), 0);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=inf

	Event output = model->lambda(model->ta(0));	// lambda
	model->dint(model->ta(0));	// after the lambda

	////// validate output 4  (to queue 1)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	auto vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 1); // dequeue to queue 1


	// request dequeue t=1
	model->dext(Event(&size, 1), 1);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=0

	output = model->lambda(1);	// lambda
	model->dint(1);	// after the lambda

	////// validate output 2  (to queue 3)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 3); // dequeue to queue 3

	// request dequeue t=2
	model->dext(Event(&size, 1), 2);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=0

	output = model->lambda(2);	// lambda
	model->dint(2);	// after the lambda

	////// validate output 3  (again to queue 1)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 1); // dequeue to queue 1

	// request dequeue t=3
	model->dext(Event(&size, 1), 3);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=0

	output = model->lambda(3);	// lambda
	model->dint(3);	// after the lambda

	////// validate output 3  (again to queue 3)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 3); // dequeue to queue 3
}

BOOST_AUTO_TEST_CASE(functional_WRRwithEqualWeightsAndChangingSizes)
{
	auto mainCoupling = new RootCoupling((char*)"top");

	auto model = new WRRScheduler_wrapper("WRRScheduler");
	model->father = mainCoupling;
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});
	model->init(0.0,"8", "weights");
	double size = 400;

	// set some queue sizes
	model->dext(createEventWithVectorial(&size, 0, 3), 0); // set the size for queue 3
	model->dext(createEventWithVectorial(&size, 0, 1), 0); // set the size for queue 1
	model->dext(createEventWithVectorial(&size, 0, 5), 0); // set the size for queue 5

	// request dequeue t=0
	model->dext(Event(&size, 1), 0);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=inf

	Event output = model->lambda(model->ta(0));	// lambda
	model->dint(model->ta(0));	// after the lambda

	////// validate output 4  (to queue 1)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	auto vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 1); // dequeue to queue 1


	// request dequeue t=1
	model->dext(Event(&size, 1), 1);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=0

	output = model->lambda(1);	// lambda
	model->dint(1);	// after the lambda

	////// validate output 2  (to queue 3)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 3); // dequeue to queue 3

	// request dequeue t=2
	model->dext(Event(&size, 1), 2);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=0

	output = model->lambda(2);	// lambda
	model->dint(2);	// after the lambda

	////// validate output 3  (queue 5)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 5); // dequeue to queue 5

	// request dequeue t=3
	model->dext(Event(&size, 1), 3);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=0

	output = model->lambda(3);	// lambda
	model->dint(3);	// after the lambda

	////// validate output 3  (again to queue 1)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 1); // dequeue to queue 1


	// request dequeue t=3
	size = 0;
	model->dext(createEventWithVectorial(&size, 0, 3), 0); // set the size for queue 3 = 0
	model->dext(createEventWithVectorial(&size, 0, 1), 0); // set the size for queue 1 = 0
	model->dext(Event(&size, 1), 3);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=0

	output = model->lambda(3);	// lambda
	model->dint(3);	// after the lambda

	////// validate output 3  (jumps empty queue 3, again to queue 5)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 5); // dequeue to queue 5

	// request dequeue t=3
	model->dext(Event(&size, 1), 3);

	BOOST_REQUIRE_EQUAL(model->ta(0), 0); // validate ta=0

	output = model->lambda(3);	// lambda
	model->dint(3);	// after the lambda

	////// validate output 3  (jumps empty queue 3, again to queue 5)
	BOOST_REQUIRE_EQUAL(output.port, 0); // out port
	BOOST_REQUIRE(output.valuePtr);      // non null value
	vecOutput = std::static_pointer_cast<VectorialEvent>(output.valuePtr);
	BOOST_REQUIRE_EQUAL(vecOutput->index, 5); // dequeue to queue 5
}

BOOST_AUTO_TEST_CASE(functional_WRRwithEqualWeights2)
{
	// test subject
	auto model = new WRRScheduler_wrapper("WRRScheduler");

	// set up parameters
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 1, 1, 1, 1, 1, 1});

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"8", "weights"});

	double size = 500;
	root->setupExternalEvents({
		{0, createEventWithVectorial(&size, 0, 1)}, // size for queue 1
		{0, createEventWithVectorial(&size, 0, 4)}, // size for queue 4
		{0, createEventWithVectorial(&size, 0, 5)}, // size for queue 5
		{1, Event(&size, 1)}, // dequeue request t=1
		{2, Event(&size, 1)}, // dequeue request t=2
		{3, Event(&size, 1)}, // dequeue request t=3
		{4, Event(&size, 1)} // dequeue request t=4
	});

	// run simulation
	auto generatedEvents = root->run(5);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), 4);
	BOOST_REQUIRE_EQUAL(generatedEvents[0].first, 1); // t=1
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[0].second.valuePtr))->index, 1); // queue 1

	BOOST_REQUIRE_EQUAL(generatedEvents[1].first, 2); // t=2
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[1].second.valuePtr))->index, 4); // queue 4

	BOOST_REQUIRE_EQUAL(generatedEvents[2].first, 3); // t=3
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[2].second.valuePtr))->index, 5); // queue 5

	BOOST_REQUIRE_EQUAL(generatedEvents[2].first, 3); // t=4
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[3].second.valuePtr))->index, 1); // queue 1
}

BOOST_AUTO_TEST_CASE(functional_WRRwithWeights)
{
	// test subject
	auto model = new WRRScheduler_wrapper("WRRScheduler");

	// set up parameters
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 3, 2, 1, 1, 1, 1, 5}); //weights for queues 2,3&7

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"8", "weights"});

	// prepare input events
	double size = 500;
	std::vector<std::pair<double, Event>> events;

	// size in all the queues
	for(int i = 0; i < 8; i++) {
		events.push_back({0, createEventWithVectorial(&size, 0, i)});
	}

	int totalRequests = 25;
	for(int i = 1; i <= totalRequests; i++) { // totalRequests dequeue requests
		events.push_back({i, Event(&size, 1)}); // t=i
	}

	root->setupExternalEvents(events);

	// run simulation
	auto generatedEvents = root->run(totalRequests + 5);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), totalRequests);

	//verify times
	for(int i = 0; i < totalRequests; i++) { // totalRequests dequeue requests
		BOOST_REQUIRE_EQUAL(generatedEvents[i].first, i+1); // t=i
	}

	int eventIndex = 0;
	// weights: {1, 3, 2, 1, 1, 1, 1, 5}
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 0); // queue 0
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 1); // queue 1
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 1); // queue 1 (2nd time)
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 1); // queue 1 (3rd time)
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 2); // queue 2
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 2); // queue 2
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 3); // queue 3
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 4); // queue 4
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 5); // queue 5
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 6); // queue 6
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 7); // queue 7 (1 time)
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 7); // queue 7 (2nd time)
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 7); // queue 7 (3 time)
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 7); // queue 7 (4 time)
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 7); // queue 7 (5 time)
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 0); // queue 0
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 1); // queue 1
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 1); // queue 1
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 1); // queue 1
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 2); // queue 2
}

BOOST_AUTO_TEST_CASE(functional_WRRwithSomeCeroWeights)
{
	// TODO: this test fails because the model is not prepared to receive all 0 weights. It should not affect the simulations, as all-0s makes no sense.
	// TODO: add an expection in the model to avoid this situation.

	// test subject
	auto model = new WRRScheduler_wrapper("WRRScheduler");

	// set up parameters
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {0, 1, 2, 3, 0, 4, 1, 0}); // 0 weights for queues 0,4&7

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"8", "weights"});

	// prepare input events
	double size = 500;
	std::vector<std::pair<double, Event>> events;

	// size in all the queues
	for(int i = 0; i < 8; i++) {
		events.push_back({0, createEventWithVectorial(&size, 0, i)});
	}

	int totalRequests = 45;
	for(int i = 1; i <= totalRequests; i++) { // totalRequests dequeue requests
		events.push_back({i, Event(&size, 1)}); // t=i
	}

	root->setupExternalEvents(events);

	// run simulation
	auto generatedEvents = root->run(totalRequests + 5);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), totalRequests);

	//verify times
	for(int i = 0; i < totalRequests; i++) { // totalRequests dequeue requests
		BOOST_REQUIRE_EQUAL(generatedEvents[i].first, i+1); // t=i
	}


	// weights: {0, 1, 2, 3, 0, 4, 1, 0}
	for(int eventIndex = 0; eventIndex < 3; ){ // test 3 rounds of robin
		//	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 0); // queue 0
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 1); // queue 1
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 2); // queue 2 (1)
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 2); // queue 2 (2)
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 3); // queue 3 (1)
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 3); // queue 3 (2)
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 3); // queue 3 (3)
		//	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 4); // queue 4
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 5); // queue 5 (1)
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 5); // queue 5 (2)
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 5); // queue 5 (3)
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 5); // queue 5 (4)
		BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 6); // queue 6
		//	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 7); // queue 7
	}
}

BOOST_AUTO_TEST_CASE(functional_WRRwithWeightsAndAllEmptyQueues_shouldWaitForFirstQueueToNotifySize)
{
	// test subject
	auto model = new WRRScheduler_wrapper("WRRScheduler");

	// set up parameters
	CmdLineModelParams.clear();
	setCmdLineParameter("weights", {1, 1, 2, 1, 1, 2, 1, 1});

	FunctionalTestRootSimulator* root = new FunctionalTestRootSimulator(model, {"8", "weights"});

	// prepare input events
	double fullQueue = 500;
	double emptyQueue = 0;
	std::vector<std::pair<double, Event>> events;

	// set size in queue 1 and 3
	events.push_back({0, createEventWithVectorial(&fullQueue, 0, 1)});
	events.push_back({0, createEventWithVectorial(&fullQueue, 0, 3)});

	events.push_back({1, Event(&fullQueue, 1)}); // request dequeue --> 1 should be served
	events.push_back({1+0.1, createEventWithVectorial(&emptyQueue, 0, 1)}); // queue 1 should notify 0 size

	events.push_back({2, Event(&fullQueue, 1)}); // request dequeue --> 3 should be served
	events.push_back({2+0.1, createEventWithVectorial(&emptyQueue, 0, 3)}); // queue 3 should notify 0 size

	events.push_back({3, Event(&fullQueue, 1)}); // request dequeue --> all empty, should wait for next queue to have packets

	events.push_back({4, createEventWithVectorial(&fullQueue, 0, 3)}); // queue 3 notifies some size --> should request dequeue 3

	events.push_back({5, Event(&fullQueue, 1)}); // request dequeue --> request queue 3 again

	events.push_back({6, createEventWithVectorial(&fullQueue, 0, 1)}); // queue 1 notifies some size --> nothing should happen

	events.push_back({7, Event(&fullQueue, 1)}); // request dequeue --> request queue 1
	events.push_back({8, Event(&fullQueue, 1)}); // request dequeue --> request queue 3

	root->setupExternalEvents(events);

	// run simulation
	auto generatedEvents = root->run(10);

	// verify
	BOOST_REQUIRE_EQUAL(generatedEvents.size(), 6);

	//verify times
	int eventIndex = 0;
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex++].first, 1);
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex++].first, 2);
	//	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex++].first, 3); // all queues empty, should not send request
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex++].first, 4);
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex++].first, 5);
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex++].first, 7);
	BOOST_REQUIRE_EQUAL(generatedEvents[eventIndex++].first, 8);


	// verify queue indexes
	eventIndex = 0;
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 1); // queue 1
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 3); // queue 3
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 3); // queue 3
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 3); // queue 3
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 1); // queue 1
	BOOST_REQUIRE_EQUAL((std::static_pointer_cast<VectorialEvent>(generatedEvents[eventIndex++].second.valuePtr))->index, 3); // queue 3

}


BOOST_AUTO_TEST_SUITE_END() // end suite test declaration


