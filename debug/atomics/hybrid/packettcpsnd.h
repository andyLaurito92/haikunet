//CPP:sinks/logtoscilabtool.cpp
//CPP:hybrid/ScilabSimulationsTracker.cpp       
//CPP:hybrid/packettcpsnd.cpp
#if !defined packettcpsnd_h
#define packettcpsnd_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "packettool.h"
#include "stdevstool.h"
#include "math.h"
#include "sinks/logtoscilabtool.h"


class packettcpsnd: public Simulator { 
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

int MSS;
bool WND_firsttime_SSTHRESH;
bool BUF_Fully_SNT; 
int BUF_SIZE; 
bool WND_Fully_SNT; 
int WND_NXT_SND;
int SEQ_NEXT_Expected_ACK; 
int SEQ_LAST_Valid_ACK; 
int WND_DUP_ACK; 
int WND_CWND; 
int WND_CWND_MAX;
int WND_SSTHRESH;
bool firstSS;
double WND_CWND_precise;
int SEQ_CYCLE_ACK; 
int SEQ_NXT; 
int ReceivedACKedFromPeerLayer; 
int SendACKtoPeerLayer;  
double INTERPACKET_SND_TIME; 
double INTER_REQ_TIME; 
double RTT_alfa;
double T_MEASURED_RTT;
double T_RTT_DEVIATION; 
double T_RTO;
double T_RTT;
int DUP_ACK_LIMIT;
double T_FORCED_RTO;
double OriginalACKedSEQ_TimeStamp;

double NchangeFactor;
double NchangeFactorInit;
double NchangeFactorEnd;

double sigma;

enum STATE {WAITxAPP,WAITxACK,SENDING,tr_REQxAPP};
STATE mystate;









public:
	packettcpsnd(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
};
#endif
