//CPP:ATLAS-TDAQ/PacketSender.cpp
#if !defined PacketSender_h
#define PacketSender_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"

/*!
  - Inport0: Accepts signals to restart sending new requests.
  - Outport0: Sends requests

Behaviour:
1) Starts sending requests (Outport0) with requestDelay in between
2) Once cantRequest requests has been sent, the models sets to IDLE
3) when a request to restart sending arrived (Inport0), start again in 1)
 */
class PacketSender: public Simulator {
protected:
	virtual Packet* createPacketToSend(double t){ return NULL;};

	// parameters
		int cantRequests; // Number of requests needed to send each time it is restarted
		double requestDelay; // delay to be used between each request is sent

		// state variables
		double sigma;
		int sentRequests;
		int totalEventsRequested;
		bool isBaseClass;

		enum STATE { REQUESTING, WAITING};
		STATE mystate;

		std::shared_ptr<IPowerDEVSLogger> logger;

public:
	PacketSender(const char *n, bool isBaseClass): Simulator(n) { this->isBaseClass = isBaseClass; };
	PacketSender(const char *n): PacketSender(n, true) {};
	~PacketSender(){};
	virtual void init(double, ...);
	virtual void init(double, va_list* parameters); // to be used by inheriting classes to read parameters
	virtual double ta(double t);
	virtual void dint(double);
	virtual void dext(Event , double );
	virtual Event lambda(double);
	virtual void exit();


};
#endif
