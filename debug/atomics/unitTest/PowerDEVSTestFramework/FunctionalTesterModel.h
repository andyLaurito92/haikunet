#ifndef FunctionalTesterModel_h
#define FunctionalTesterModel_h

#include <stdlib.h>
#include <iostream>

#include "simulator.h"
#include "event.h"
#include "root_coupling.h"


class FunctionalTesterModel: public Simulator {
private:
	// State variables
	double sigma;
	std::vector<std::pair<double, Event>> eventsToGenerate; //events to generate
	std::vector<std::pair<double, Event>> eventsReceived; //events to generate

public:
	FunctionalTesterModel(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit(double t);

	void setEventsToGenerate(std::vector<std::pair<double, Event>> events);
	std::vector<std::pair<double, Event>> getEventsReceived();
};

#endif
