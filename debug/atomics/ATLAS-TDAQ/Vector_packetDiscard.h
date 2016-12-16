//CPP:ATLAS-TDAQ/Vector_packetDiscard.cpp
//CPP:ATLAS-TDAQ/VectorialModel.cpp
//CPP:ATLAS-TDAQ/packetDiscard.cpp
#if !defined Vector_packetDiscard_h
#define Vector_packetDiscard_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "engine.h"
#include "qss/qsstools.h"
#include "VectorialEvent.h"
#include "packetDiscard.h"
#include "VectorialModel.h"

class Vector_packetDiscard: public VectorialModel<packetDiscard> {
public:
	Vector_packetDiscard(const char *n): VectorialModel<packetDiscard>(n) {};
	~Vector_packetDiscard() { }

	int getParameterCount() { return 2; };
};
#endif
