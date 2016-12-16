//CPP:ATLAS-TDAQ/Vector_RosApp.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/RosApp.cpp
#if !defined Vector_RosApp_h
#define Vector_RosApp_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "RosApp.h"
#include "VectorialModel.h"

class Vector_RosApp: public VectorialModel<RosApp> {
public:
	Vector_RosApp(const char *n): VectorialModel<RosApp>(n) {};
	~Vector_RosApp() { }

	int getParameterCount() { return 2; };
};
#endif
