//CPP:ATLAS-TDAQ/Vector_TcpReceiverBuffer.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/TcpReceiverBuffer.cpp
#if !defined Vector_TcpReceiverBuffer_h
#define Vector_TcpReceiverBuffer_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "TcpReceiverBuffer.h"
#include "VectorialModel.h"

class Vector_TcpReceiverBuffer: public VectorialModel<TcpReceiverBuffer> {
public:
	Vector_TcpReceiverBuffer(const char *n): VectorialModel<TcpReceiverBuffer>(n) {};
	~Vector_TcpReceiverBuffer() { }

	int getParameterCount() { return 0; };
};
#endif
