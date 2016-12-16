//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packetred.cpp
#if !defined packetred_h
#define packetred_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "stdevstool.h"
#include "packettool.h"
#include "sinks/logtoscilabtool.h"


class packetred: public Simulator { 
// Declare the state,
// output variables
// and parameters

#define inf 1E10

aMap myFields; 
aMapIter mapIt;
protocol myprotocol;
STDEVS *mySTDEVS;

aProtocolList myprotoStack;
aProtocolListIter myprotoStackit;

aLoggerToScilab *myLoggerToScilab;

Packet *p;

double sigma;

int minTH ;
double PminTH ;
int maxTH ;
double PmaxTH ;

double dropped[10];
int measuredQueueState;
double drop_prob;
int phase;


#define WAITINGFORPACKET 0
#define T_EMITTINGPACKET 1
#define T_NOTIFYINGDROP  2




public:
	packetred(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
