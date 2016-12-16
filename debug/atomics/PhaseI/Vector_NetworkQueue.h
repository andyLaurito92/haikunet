//CPP:PhaseI/Vector_NetworkQueue.cpp
 //CPP:vector/VectorialModel.cpp
 //CPP:vector/ScalarSimulator.cpp
 //CPP:PhaseI/NetworkQueue.cpp
#if !defined Vector_NetworkQueue_h
#define Vector_NetworkQueue_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "NetworkQueue.h"
#include "vector/VectorialModel.h"

class Vector_NetworkQueue: public VectorialModel<NetworkQueue> {
public:
	Vector_NetworkQueue(const char *n): VectorialModel<NetworkQueue>(n) {};
	~Vector_NetworkQueue() { }

	int getParameterCount() { return 2; };
};
#endif
