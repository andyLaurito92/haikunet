//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packetserver.cpp
#if !defined packetserver_h
#define packetserver_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "sinks/logtoscilabtool.h"


class packetserver: public Simulator { 
// Declare the state,
// output variables
// and parameters

aMap myFields; 
aMapIter mapIt;
protocol myprotocol;

aProtocolList myprotoStack;
aProtocolListIter myprotoStackit;
aLoggerToScilab *myLoggerToScilab;

Packet *p;

double sigma;
double counter;
double ServiceTime;
double ForcedServiceTime;
char* LayerFunction;
double MinTransitDelay;
double bitsPerSec;

#define inf 1E10

enum STATE {REQUESTING,PROCESSING};
STATE mystate;







public:
	packetserver(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
