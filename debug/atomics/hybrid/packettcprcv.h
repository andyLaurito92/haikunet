//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packettcprcv.cpp
#if !defined packettcprcv_h
#define packettcprcv_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "stdevstool.h"
#include "math.h"
#include "sinks/logtoscilabtool.h"


class packettcprcv: public Simulator { 
// Declare the state,
// output variables
// and parameters

#define inf 1E10

STDEVS *mySTDEVS;

aMap myFields; 
aMapIter mapIt;
protocol myprotocol;
aLoggerToScilab *myLoggerToScilab; 

aProtocolList myprotoStack;
aProtocolListIter myprotoStackit;

aDoubleEndedPacketQueue SendBuffer;
Packet *p;
Packet *pout;

int DATA_SEQ_NUMBER;
double out_ack[10];

int SEQ_NEXT_Expected;
int SEQ_LAST_Valid;
int SEQ_DUP;

int SEQ_NXT; 
int ReceivedACKedFromPeerLayer; 
int SendACKtoPeerLayer;  
double DATA_TCP_SND_TIMESTAMP;

double sigma;

enum STATE {WAITxAPP,WAITxACK,SENDING,tr_REQxAPP};
STATE mystate;






public:
	packettcprcv(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
