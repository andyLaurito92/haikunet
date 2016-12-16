//CPP:ATLAS-TDAQ/VectorIndexMod2Vector.cpp
#if !defined VectorIndexMod2Vector_h
#define VectorIndexMod2Vector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "ATLAS-TDAQ/globals.h"
#include "sinks/ParameterReader.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include <queue>

class VectorIndexMod2Vector: public Simulator { 
	// Parameters
	int mod;

	// State variables
	double sigma;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

public:
	VectorIndexMod2Vector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
