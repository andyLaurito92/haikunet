//CPP:TcpAck.cpp
#if !defined TcpAck_h
#define TcpAcks_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "tdaqPacketTool.h"

class TcpAck: public BaseSimulator {

	// State Variables
	int totalACKsSent;
	enum STATE { WAITING, SENDING_ACK };STATE myState;
	std::queue<std::shared_ptr<TdaqPacket>> packetsToAck;

	// Helpers
	

public:
	TcpAck(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
};
#endif
