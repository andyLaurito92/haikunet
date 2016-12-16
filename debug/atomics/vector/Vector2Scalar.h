//CPP:vector/Vector2Scalar.cpp
#if !defined Vector2Scalar_h
#define Vector2Scalar_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include <queue>

#include "common/BaseSimulator.h"
#include "VectorialEvent.h"
#include "sinks/ParameterReader.h"

/* Receives vectorial events and outputs them as a normal scalar (Event) event.
 * Same as vec2scalar, but uses shared_ptr not to loose the pointer reference (required when using the shared_ptr constructor of Event)
 *
 * */
class Vector2Scalar: public BaseSimulator {
	// state variables
	int index;
	std::queue<std::shared_ptr<void>> outputEvents;

public:
	Vector2Scalar(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
