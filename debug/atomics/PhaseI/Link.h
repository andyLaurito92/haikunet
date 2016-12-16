//CPP:PhaseI/Link.cpp
//CPP:PhaseI/NetworkData/NetworkPacket.cpp
#if !defined Link_h
#define Link_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/ParameterReader.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "hybrid/stdevstool.h"

#include "NetworkData/NetworkPacket.h"
#include "common/BaseSimulator.h"

#include <deque>


class Link: public BaseSimulator {
	// Parameters
	double capacity_bit_s_r; // reverse of the capacity to avoid divisions
	double delay_s;


	enum STATE { FIRST_REQUEST, WAITING_NEXT_JOB, PROCESSING};
	STATE mystate;

	std::shared_ptr<NetworkPacket> incomingPacket;

	public:
		Link(const char *n): BaseSimulator(n) {};
		void init(double, ...);
		void dint(double);
		void dext(Event , double );
		Event lambda(double);
	private:

};
#endif
