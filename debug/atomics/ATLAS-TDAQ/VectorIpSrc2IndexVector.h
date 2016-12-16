//CPP:ATLAS-TDAQ/VectorIpSrc2IndexVector.cpp
//CPP:ATLAS-TDAQ/ProtocolHeader2IndexVector.cpp
#if !defined VectorIpSrc2IndexVector_h
#define VectorIpSrc2IndexVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"
#include "ProtocolHeader2IndexVector.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"


class VectorIpSrc2IndexVector: public ProtocolHeader2IndexVector {
	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers

public:
	VectorIpSrc2IndexVector(const char *n): ProtocolHeader2IndexVector(n) {};

	std::shared_ptr<TdaqPacket> getPacketFromIncomingEvent(Event x);
	int getProtocolId();
	int getProtocolHeaderId();
};
#endif
