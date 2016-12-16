//CPP:ATLAS-TDAQ/Vector_DcmLoadSampler.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/DcmLoadSampler.cpp
//CPP:ATLAS-TDAQ/ScalarSimulator.cpp
#if !defined Vector_DcmLoadSampler_h
#define Vector_DcmLoadSampler_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "VectorialModel.h"
#include "DcmLoadSampler.h"
#include "ScalarSimulator.h"

class Vector_DcmLoadSampler: public VectorialModel<DcmLoadSampler> {
public:
	Vector_DcmLoadSampler(const char *n): VectorialModel<DcmLoadSampler>(n) {};
	~Vector_DcmLoadSampler() { }

	int getParameterCount() { return 1; };
};

#endif
