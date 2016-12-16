#ifndef FunctionalTestRootSimulator_h
#define FunctionalTestRootSimulator_h

#include <stdlib.h>
#include <iostream>

#include "simulator.h"
#include "event.h"
#include "root_coupling.h"
#include "FunctionalTesterModel.h"


class FunctionalTestRootSimulator: public RootSimulator {
	FunctionalTesterModel* testerModel;
	Simulator* testSubjectModel;
public:
	FunctionalTestRootSimulator (Simulator* testSubjectModel, std::vector<std::string> initParams) ;

	void setupExternalEvents(std::vector<std::pair<double, Event>> events);
	std::vector<std::pair<double, Event>> run(double finalTime);
};

#endif
