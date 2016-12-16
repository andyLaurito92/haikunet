//CPP:ATLAS-TDAQ/Vector_ScilabSampler.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/ScilabSampler.cpp
#if !defined Vector_ScilabSampler_h
#define Vector_ScilabSampler_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "ScilabSampler.h"
#include "VectorialModel.h"

class Vector_ScilabSampler: public VectorialModel<ScilabSampler> {
public:
	Vector_ScilabSampler(const char *n): VectorialModel<ScilabSampler>(n) {};
	~Vector_ScilabSampler() { }

	int getParameterCount() { return 1; };
};
#endif
