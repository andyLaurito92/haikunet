//CPP:ATLAS-TDAQ/Vector_DcmApp.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/DcmApp.cpp
#if !defined Vector_DcmApp_h
#define Vector_DcmApp_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "engine.h"

#include "DcmApp.h"
#include "VectorialModel.h"

class Vector_DcmApp: public VectorialModel<DcmApp> {
public:
	Vector_DcmApp(const char *n): VectorialModel<DcmApp>(n) {};
	~Vector_DcmApp() { }

	int getParameterCount() { return 2; };
};
#endif
