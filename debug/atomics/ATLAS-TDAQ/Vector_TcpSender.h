//CPP:ATLAS-TDAQ/Vector_TcpSender.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/TcpSender.cpp
#if !defined Vector_TcpSender_h
#define Vector_TcpSender_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "TcpSender.h"
#include "VectorialModel.h"

class Vector_TcpSender: public VectorialModel<TcpSender> {
public:
	Vector_TcpSender(const char *n): VectorialModel<TcpSender>(n) {};
	~Vector_TcpSender() { }

	int getParameterCount() { return 4; };
};
#endif
