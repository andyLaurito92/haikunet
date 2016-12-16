//CPP:ATLAS-TDAQ//ProtocolHeader2IndexVector.cpp
#if !defined ProtocolHeader2IndexVector_h
#define ProtocolHeader2IndexVector_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "VectorialEvent.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"

// Abstract class
class ProtocolHeader2IndexVector: public Simulator {
	// State variables
	double sigma;
	int index;
	std::queue<std::shared_ptr<VectorialEvent>> outputVectorialEvents;

	// Helpers

public:
	ProtocolHeader2IndexVector(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();

	// retrieve packet from event (could be a Vectorial or a normal event)
	virtual std::shared_ptr<TdaqPacket> getPacketFromIncomingEvent(Event x)=0;

	// returns the protocol ID to be used (ej: TCP or IP)
	virtual int getProtocolId()=0;

	// returns the protocol header ID to be used (ej: could be DST, SRC for IP or DST_PORT, SRC_PORT for TCP)
	virtual int getProtocolHeaderId()=0;

};
#endif
