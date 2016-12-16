//CPP:ATLAS-TDAQ/Vector_TcpAck.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/TcpAck.cpp
#if !defined Vector_TcpAck_h
#define Vector_TcpAck_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "TcpAck.h"
#include "VectorialModel.h"

class Vector_TcpAck: public VectorialModel<TcpAck> {
public:
	Vector_TcpAck(const char *n): VectorialModel<TcpAck>(n) {};
	~Vector_TcpAck() { }

	int getParameterCount() { return 0; };
};

#endif
