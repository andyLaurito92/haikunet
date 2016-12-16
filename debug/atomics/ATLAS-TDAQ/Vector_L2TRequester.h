//CPP:ATLAS-TDAQ/Vector_L2TRequester.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/L2TRequester.cpp
#if !defined Vector_L2TRequester_h
#define Vector_L2TRequester_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "L2TRequester.h"
#include "VectorialModel.h"

class Vector_L2TRequester: public VectorialModel<L2TRequester> {
public:
	Vector_L2TRequester(const char *n): VectorialModel<L2TRequester>(n) {};
	~Vector_L2TRequester() { }

	int getParameterCount() { return 2; };
};

#endif
