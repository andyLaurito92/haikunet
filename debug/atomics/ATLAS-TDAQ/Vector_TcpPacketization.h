//CPP:ATLAS-TDAQ/Vector_TcpPacketization.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/TcpPacketization.cpp
//CPP:ATLAS-TDAQ/ScalarSimulator.cpp
#if !defined Vector_TcpPacketization_h
#define Vector_TcpPacketization_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "TcpPacketization.h"
#include "ScalarSimulator.h"
#include "VectorialModel.h"

class Vector_TcpPacketization: public VectorialModel<TcpPacketization> {
public:
	Vector_TcpPacketization(const char *n): VectorialModel<TcpPacketization>(n) {};
	~Vector_TcpPacketization() { }

	int getParameterCount() { return 4; };
};
#endif
