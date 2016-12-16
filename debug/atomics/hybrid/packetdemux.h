//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packetdemux.cpp
#if !defined packetdemux_h
#define packetdemux_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/logtoscilabtool.h"
#include "packettool.h"


class packetdemux: public Simulator { 
// Declare the state,
// output variables
// and parameters

double sigma; 

//parameters
std::string Labels[10];
int n;

#define INF 1e20

aMap myFields; 
aMapIter mapIt;

protocol myprotocol;

aProtocolList myprotoStack;
aProtocolListIter myprotoStackit;

Packet *p;

aLoggerToScilab *myLoggerToScilab;

std::string FlowID;
int Flen;

#define INF 1e20












public:
	packetdemux(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
