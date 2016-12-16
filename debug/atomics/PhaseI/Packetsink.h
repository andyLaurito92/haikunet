//CPP:PhaseI/Packetsink.cpp
//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/stdevstool.cpp
//CPP:PhaseI/NetworkData/NetworkPacket.cpp
#if !defined Packetsink_h
#define Packetsink_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/ParameterReader.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "hybrid/stdevstool.h"

#include "NetworkData/NetworkPacket.h"
#include "common/BaseSimulator.h"

#include <deque>


class Packetsink: public BaseSimulator {
	int packetCount = 0;
	double lastT=-1;

	public:
		Packetsink(const char *n): BaseSimulator(n) {};
		void init(double, ...);
		void dint(double);
		void dext(Event , double );
		Event lambda(double);
	private:

};
#endif
