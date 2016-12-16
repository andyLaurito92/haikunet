//CPP:ATLAS-TDAQ/Vector_EBRequester.cpp
//CPP:ATLAS-TDAQ/EBRequester.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
#if !defined Vector_EBRequester_h
#define Vector_EBRequester_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "EBRequester.h"
#include "VectorialModel.h"

class Vector_EBRequester: public VectorialModel<EBRequester> {
public:
	Vector_EBRequester(const char *n): VectorialModel<EBRequester>(n) {};
	~Vector_EBRequester() { }

	int getParameterCount() { return 0; };
};


#endif
