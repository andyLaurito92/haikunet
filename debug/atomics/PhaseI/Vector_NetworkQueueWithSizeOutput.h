//CPP:PhaseI/Vector_NetworkQueueWithSizeOutput.cpp
//CPP:vector/VectorialModel.cpp
//CPP:vector/ScalarSimulator.cpp
//CPP:PhaseI/NetworkQueueWithSizeOutput.cpp
#if !defined Vector_NetworkQueueWithSizeOutput_h
#define Vector_NetworkQueueWithSizeOutput_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "NetworkQueueWithSizeOutput.h"
#include "vector/VectorialModel.h"

class Vector_NetworkQueueWithSizeOutput: public VectorialModel<NetworkQueueWithSizeOutput> {
public:
	Vector_NetworkQueueWithSizeOutput(const char *n): VectorialModel<NetworkQueueWithSizeOutput>(n) {};
	~Vector_NetworkQueueWithSizeOutput() { }

	int getParameterCount() { return 2; };
};
#endif
