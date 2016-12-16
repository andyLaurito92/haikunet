//CPP:ATLAS-TDAQ/Vector_PacketGen.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/packetgen.cpp
#if !defined Vector_PacketGen_h
#define Vector_PacketGen_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "packetgen.h"
#include "VectorialModel.h"

class Vector_PacketGen: public VectorialModel<packetgen> {
public:
	Vector_PacketGen(const char *n): VectorialModel<packetgen>(n) {};
	~Vector_PacketGen() { }

	int getParameterCount() { return 4; };
};

#endif
