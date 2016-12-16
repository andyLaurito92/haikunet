//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/packetqueue.cpp
//NO-CPP:hybrid/ScilabSimulationsTracker.cpp
#if !defined packetqueue_h
#define packetqueue_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "sinks/logtoscilabtool.h"


class packetqueue: public Simulator { 
// Declare the state,
// output variables
// and parameters

#define inf 1E10

aMap myFields; 
aMapIter mapIt;
protocol myprotocol;

aProtocolList myprotoStack;
aProtocolListIter myprotoStackit;

Packet *p;
Packet *pout;

aPacketQueue myPacketQueue;
aLoggerToScilab *myLoggerToScilab;

double sigma;
int MaxCapacity_bytes;
double ForcedPeriod;
bool pendingRequest;
bool pendingLengthNotify;
double sizeout[10];

enum STATE {WAITING,t_NOTIFYING,t_READYTOEMIT};
STATE mystate;
enum SERVERSTATE {SERVERWAITING,SERVERBUSY};
SERVERSTATE serverstate;



public:
	packetqueue(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
