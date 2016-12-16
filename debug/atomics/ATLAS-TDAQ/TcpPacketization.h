//CPP:ATLAS-TDAQ/TcpPacketization.cpp
#if !defined TcpPacketization_h
#define TcpPacketization_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "hybrid/stdevstool.h"
#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "tdaqPacketTool.h"
#include <cmath>
#include <algorithm>



// TODO: this model is just a place holder, which currently sends a 1 packet for every fragment response
class TcpPacketization: public BaseSimulator {
	// Parameters
	double maxPacketSize;
	double fragmentSize;
	double fragmentSd;
        double minFragmentSize;

	// State Variables
	double sigma;
	std::vector<std::shared_ptr<FragmentInfo>> waitingResponseToBeSent;
	std::shared_ptr<TdaqPacket> requestPacket;

	std::shared_ptr<IPowerDEVSLogger> logger;

	// Helpers


	public:
		TcpPacketization(const char *n): BaseSimulator(n) {};
		void init(double, ...);
		void dint(double);
		void dext(Event , double );
		Event lambda(double);
	private:
		std::shared_ptr<TdaqPacket> getNextPacketToSend(double t);
};
#endif
