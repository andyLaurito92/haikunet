//CPP:ATLAS-TDAQ/Vector_EventBuilder.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/EventBuilder.cpp
#if !defined Vector_EventBuilder_h
#define Vector_EventBuilder_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "EventBuilder.h"
#include "VectorialModel.h"

class Vector_EventBuilder: public VectorialModel<EventBuilder> {
public:
	Vector_EventBuilder(const char *n): VectorialModel<EventBuilder>(n) {};
	~Vector_EventBuilder() { }

	int getParameterCount() { return 1; };
};

#endif
