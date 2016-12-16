#include "FunctionalTestRootSimulator.h"
#include "sink/SimulationExperimentTracker.h"

FunctionalTestRootSimulator::FunctionalTestRootSimulator (Simulator* testSubjectModel, std::vector<std::string> initParams) : RootSimulator((char*)"rootFunctionalTestSimulator"), testSubjectModel(testSubjectModel) {
	assert(initParams.size() < 10); // 10 parameters should be enough

	// create top model
	mainCoupling = new RootCoupling((char*)"rootFunctionalTestSimulator");
	Simulator **D0 = new Simulator* [2];
	Connection **IC0 = new Connection* [1000*2];
	//D0[0] is  tester
	this->testerModel = new FunctionalTesterModel("functionalTesterModel");
	D0[0] = this->testerModel ;
	((Simulator*)D0[0])->father = mainCoupling;
	D0[0]->init(0.0);

	//D0[1] is  testSubject
	D0[1] = testSubjectModel;
	((Simulator*)D0[1])->father = mainCoupling;
	D0[1]->init(0.0, initParams[0].c_str(), initParams[1].c_str(), initParams[2].c_str(), initParams[3].c_str(), initParams[4].c_str(), initParams[5].c_str(), initParams[6].c_str(), initParams[7].c_str(), initParams[8].c_str(), initParams[9].c_str());

	// tester outPorts --> testSubject inPorts
	for(int i = 0; i < 1000; i++){ // setting up 1000 ports should be more than enough
		IC0[i] = new Connection();
		IC0[i]->setup(0,i,1,i);
	}

	// testSubject outPorts --> tester inPorts
	for(int i = 0; i < 1000; i++){ // setting up 1000 ports should be more than enough
		IC0[i+1000] = new Connection();
		IC0[i+1000]->setup(1,i,0,i);
	}

	mainCoupling->setup(D0,2,IC0,1000*2);
	((RootCoupling*)mainCoupling)->rootSim = this;
	BOOST_TEST_CHECKPOINT("Created root simulator for functional test of '" << this->testSubjectModel->getFullName() << "' atomic model \n");
}

void FunctionalTestRootSimulator::setupExternalEvents(std::vector<std::pair<double, Event>> events){
	this->testerModel->setEventsToGenerate(events);
}

std::vector<std::pair<double, Event>> FunctionalTestRootSimulator::run(double finalTime){
	this->setInitialTime(0);
	this->setBreakCount(10000 /* normal simulation is 1000000 */);
	this->init();
	this->setFinalTime(finalTime);
	setFinalTime(finalTime); // set the global tf
	BOOST_TEST_MESSAGE("Starting functional simulation for '" << this->testSubjectModel->getFullName() << "' atomic model ....\n");
	RootSimulator::run();
	BOOST_TEST_MESSAGE("Functional simulation completed running. \n");

	if(this->getExitStatus() !=0 ){
		BOOST_REQUIRE_MESSAGE(this->getExitStatus(), 0);
	}

	return this->testerModel->getEventsReceived();
}
