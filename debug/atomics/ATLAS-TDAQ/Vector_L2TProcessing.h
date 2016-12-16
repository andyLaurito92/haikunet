//CPP:ATLAS-TDAQ/Vector_L2TProcessing.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/L2TProcessing.cpp
//CPP:hybrid/stdevstool.cpp
//CPP:ATLAS-TDAQ/tdaqPacketTool.cpp
#if !defined Vector_L2TProcessing_h
#define Vector_L2TProcessing_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "L2TProcessing.h"
#include "VectorialModel.h"

class Vector_L2TProcessing: public VectorialModel<L2TProcessing> {
public:
	Vector_L2TProcessing(const char *n): VectorialModel<L2TProcessing>(n) {};
	~Vector_L2TProcessing() { }

	int getParameterCount() { return 4; };
};

#endif
