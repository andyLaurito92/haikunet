//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packetgen_v2.cpp
#if !defined packetgen_v2_h
#define packetgen_v2_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "stdevstool.h"
#include "sinks/logtoscilabtool.h"
#include "math.h"


class packetgen_v2: public Simulator { 
// Declare the state,
// output variables
// and parameters

aMap myFields; 
aMapIter mapIt;
protocol myprotocol;

aProtocolList myprotoStack;
aProtocolListIter myprotoStackit;

STDEVS *mySTDEVS;
Packet *p;
aLoggerToScilab *myLoggerToScilab;

double sigma;
double counter;
char *FlowID;
double Period;
double Psize;

double BW;
double AvgPktSize;
double MinPktSize;
double MaxPktSize;
char *FixSizeOnAvg;
double PSize;





















public:
	packetgen_v2(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
