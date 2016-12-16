//CPP:ATLAS-TDAQ/Vector_PacketQueue.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/PacketQueue.cpp
#if !defined Vector_PacketQueue_h
#define Vector_PacketQueue_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "PacketQueue.h"
#include "VectorialModel.h"

class Vector_PacketQueue: public VectorialModel<PacketQueue> {
public:
	Vector_PacketQueue(const char *n): VectorialModel<PacketQueue>(n) {};
	~Vector_PacketQueue() { }

	int getParameterCount() { return 2; };

	void dext(Event x, double t) {
		VectorialModel<PacketQueue>::dext(x,t); // forward to parent
	}
};

#endif
