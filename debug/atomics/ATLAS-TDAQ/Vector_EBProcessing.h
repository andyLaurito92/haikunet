//CPP:ATLAS-TDAQ/Vector_EBProcessing.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/EBProcessing.cpp
#if !defined Vector_EBProcessing_h
#define Vector_EBProcessing_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "EBProcessing.h"
#include "VectorialModel.h"

class Vector_EBProcessing: public VectorialModel<EBProcessing> {
public:
	Vector_EBProcessing(const char *n): VectorialModel<EBProcessing>(n) {};
	~Vector_EBProcessing() { }

	int getParameterCount() { return 1; };
};

#endif
