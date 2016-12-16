//CPP:vector/VectorBondedLinkVector.cpp
#if !defined VectorBondedLinkVector_h
#define VectorBondedLinkVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "tdaqPacketTool.h"


class VectorBondedLinkVector: public BaseSimulator {

	// Parameters
	int outputLinks;

	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers
	//TODO: these are MAC addresses from some ROSes  transformed into long
	std::vector<unsigned long> rosMacAddress = {268633365043616, 268633365047312, 268633365047024,268633365039584,268633365038832,268633365039216,268633365043104,268633365041457,268633365046196,268633365047749};

	//TODO: these are MAC addresses from some DCMs transformed into long
	std::vector<unsigned long> dcmMacAddress = {144345383690, 690716706065, 690716813293,690716703981,690716706793,165037182845,690716815049,165037037353,165037176801,165037175969,165037177849,690716815157};

	// HELPERS
	std::shared_ptr<IPowerDEVSLogger> logger;


public:
	VectorBondedLinkVector(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
private:
	unsigned int calculateHash(long destMacAddress, long srcMacAddres, long destIp, long srcIp, long packetTypeId);
};
#endif
