//CPP:ATLAS-TDAQ/ScalarIpDst2IndexVector.cpp
//CPP:ATLAS-TDAQ/ProtocolHeader2IndexVector.cpp
#if !defined ScalarIpDst2IndexVector_h
#define ScalarIpDst2IndexVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"
#include "ProtocolHeader2IndexVector.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"


class ScalarIpDst2IndexVector: public ProtocolHeader2IndexVector {
	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers

public:
	ScalarIpDst2IndexVector(const char *n): ProtocolHeader2IndexVector(n) {};

	std::shared_ptr<TdaqPacket> getPacketFromIncomingEvent(Event x);
	int getProtocolId();
	int getProtocolHeaderId();
};
#endif
