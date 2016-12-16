//CPP:ATLAS-TDAQ/Vector_L2TIterator.cpp
//CPP:ATLAS-TDAQ/L2TIterator.cpp
#if !defined Vector_L2TIterator_h
#define Vector_L2TIterator_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "L2TIterator.h"
#include "VectorialModel.h"

class Vector_L2TIterator: public VectorialModel<L2TIterator> {
public:
	Vector_L2TIterator(const char *n): VectorialModel<L2TIterator>(n) {};
	~Vector_L2TIterator() { }

	int getParameterCount() { return 3; };
};
#endif
