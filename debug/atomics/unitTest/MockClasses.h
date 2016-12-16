#ifndef MOCK_CLASSES
#define MOCK_CLASSES

#include <stdlib.h>
#include <iostream>

#include "simulator.h"
#include "event.h"
#include "root_coupling.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"

// TODO: evaluate using https://github.com/eranpeer/FakeIt to create mocks easily

class MockSimulator : public Simulator{
public:
	MockSimulator(const char *n) : Simulator(n) {
		this->father = new RootCoupling((char*)"Mock top model");
	};
	void init(double, ...) { }
	double ta(double t) { return 99;}
	void dint(double) { }
	void dext(Event, double) { }
	Event lambda(double) { return Event(); }
	void exit(double t) { }
};

class MockLogger : public IPowerDEVSLogger{
public:
	std::vector<std::string> variableNames;
	bool flushed = false;
	std::map<std::string, std::vector<std::pair<double, double>>> loggedValues;

	void initSignals(std::vector<std::string> variableNames){
		this->variableNames = variableNames;
	}

	void initSignals(std::map<std::string,int> variableNamesAndPriorities){}

	void logSignal(double t, double value, std::string name) {
		this->loggedValues[name].push_back({t,value});
	}

	void flush(double t) { this->flushed = true;}

};

#endif
