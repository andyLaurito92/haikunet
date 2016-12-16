//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/PacketRetransmission.cpp
#if !defined packetRetransmission_h
#define packetRetransmission_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <vector> // For std::vector
#include <algorithm>    // std::find_if

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "hybrid/packettool.h"
#include "tdaqPacketTool.h"

class PacketRetransmission: public Simulator {
	struct UnACKPacket {
		TdaqPacket* packet;
	    double retransmissionTime;
	    int pendingFragments; // fragments not ACKed for this packet. TODO: // TODO: A complete remodel with ACKs from the DCM side is pending
	};

	// Parametes
	double retransmissionTimeOut;

	std::shared_ptr<IPowerDEVSLogger> logger;

	double sigma;
	std::vector<UnACKPacket> unACKPackets;

	enum STATE { WAITING };
	STATE myState;
	int retransmissions;

public:
	PacketRetransmission(const char *n) : Simulator(n) { }	;
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event, double);
	Event lambda(double);
	void exit();
private:
	TdaqPacket* getNextPacketToRetransmit();
	double getNextRetransmissionSigma(double t);

};
#endif
