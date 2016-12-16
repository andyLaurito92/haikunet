#include "packetgen_v2.h"
void packetgen_v2::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//  %Name% is the parameter name
//	%Type% is the parameter type

//////////////////////////////// STDEVS
mySTDEVS = new STDEVS(_STDEVS_DEFAULT_SEED);

printLog ("STDEVS Init Poisson %u \n", mySTDEVS->poisson(500));
printLog ("STDEVS Init Bernoulli %g \n", mySTDEVS->bernoulli(0.5));

FlowID = va_arg(parameters,char*);
printLog("[%g] PacketGen: FlowID: %s \n",t,FlowID);

char* fvar;

fvar = va_arg(parameters,char*);
BW = (double)getScilabVar(fvar);
printLog("[%g] PacketGen: BW: %g bits/seg \n",t,BW);

fvar = va_arg(parameters,char*);
AvgPktSize = (double)getScilabVar(fvar);
printLog("[%g] PacketGen: Average Packet Size: %g bits \n",t,AvgPktSize);

fvar = va_arg(parameters,char*);
MinPktSize = (double)getScilabVar(fvar);
printLog("[%g] PacketGen: Min Packet Size: %g bits \n",t,MinPktSize);

fvar = va_arg(parameters,char*);
MaxPktSize = (double)getScilabVar(fvar);
printLog("[%g] PacketGen: Max Packet Size: %g bits \n",t,MaxPktSize);

FixSizeOnAvg = va_arg(parameters,char*);
printLog("[%g] PacketGen: Fix Size on %g bits: %s \n",t,AvgPktSize,FixSizeOnAvg);

myprotocol.protoID=0;
myFields[IP_SRC] = 70.1;
myFields[IP_DST] = 80.2;
myFields[SEQ] = 90.3;
myprotocol.headerFields=myFields;
// myprotocol.METADATA[OVERHEAD]=mySTDEVS->poisson(500);
myprotoStack.push_back(myprotocol);

PSize=AvgPktSize;
sigma=PSize/BW;
counter=0;

myLoggerToScilab = new aLoggerToScilab(this->myself,this->getName());
myLoggerToScilab->initSignals(2,"pktlength","intergen");

return;
}
double packetgen_v2::ta(double t) {
//This function returns a double.
return sigma;
}
void packetgen_v2::dint(double t) {
//for ( it=mymap.begin() ; it != mymap.end(); it++ )
//      printLog("[%g] PacketGen: key = %u val = %u \n",t,(*it).first,(*it).second);

int lowerrange = ceil(AvgPktSize-MinPktSize) ;
int upperrange = ceil(MaxPktSize-AvgPktSize) ;

if (strcmp(FixSizeOnAvg,"Yes")==0) {
	PSize = AvgPktSize; // Fixed Period
} else {
	PSize = ceil(mySTDEVS->exponential(AvgPktSize));
	if (PSize<MinPktSize) {PSize=MinPktSize;}
	if (PSize>MaxPktSize) {PSize=MaxPktSize;}
}

sigma=PSize/BW;

myLoggerToScilab->logSignal(t,sigma,"intergen");

// FOR TESTING PURPOSES ONLY:
//if (t>200) {sigma=mySTDEVS->exponential(Period*2.05);}; // FORCES A STEP IN THE INTERARRIVAL TIMES

return;
}
void packetgen_v2::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition
}
Event packetgen_v2::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)

// ADDs new protocols to the packet to track them along a full network path
//for (int i=1; i<=1; i++) {
//  	  myprotoStack.push_back(myprotocol); // 1 2 3 4 5
//	  myprotocol.protoID++;
//	  myFields[SRC]++;
//	  myFields[DST]++;
//	  myFields[SEQ]++;
//	  myprotocol.headerFields=myFields;
//	  };

///////////////
  printLog("[%g] PacketGen: myprotoStack contains: \n",t);
  for (myprotoStackit=myprotoStack.begin(); myprotoStackit!=myprotoStack.end(); myprotoStackit++) {
  printLog("[%g] PacketGen: ID %u \n",t,(*myprotoStackit).protoID);
    myFields = (*myprotoStackit).headerFields;	
		 for ( mapIt=myFields.begin() ; mapIt != myFields.end(); mapIt++ )
  			printLog("[%g] PacketGen: field value %g \n",t,(*mapIt).second);
  }

printLog("[%g] PacketGen: emitting protocol ID %u \n",t, myprotocol.protoID);

counter++;
p = new Packet;
p->ID = counter;
p->FlowID = this->FlowID;

//printLog("executing experimental assingments... \n");
//int experiment_int=45;
//char experiment_char='W';
//double experiment_double=5.456;
//p->experimentalList.push_back (&experiment_int);
//p->experimentalList.push_back (&experiment_char);
//p->experimentalList.push_back (&experiment_double);
//printLog("... int %u \n",*(int*)p->experimentalList.front());
//p->experimentalList.pop_front();
//printLog("... int %c \n",*(char*)p->experimentalList.front());
//p->experimentalList.pop_front();
//printLog("... int %g \n",*(double*)p->experimentalList.front());
//p->experimentalList.pop_front();
//printLog("... done ! \n");

p->ComplexityFactor=1;
p->birthTime=t-sigma; 	// The emmiting event represents the arrival of the last bit to the destination block
						// Thus, the packet was born at the begginning of the wait perios (i.e., sigma)

// myprotoStack.back().METADATA[OVERHEAD] = 10*8;
// myprotoStack.back().METADATA[PAYLOADSIZE] = (int)mySTDEVS->exponential(490*8)%(1000*8);

myprotoStack.back().METADATA[PAYLOADSIZE] = Psize;
printLog("[%g] PacketGen: created packet #%i with tiemstamp %g \n",t, p->ID, p->birthTime);

p->ProtocolList = myprotoStack;
printLog("[%g] PacketGen: packet list size %u \n",t, (int)p->ProtocolList.size());

p->calcLength();
double templength=(double)p->getLength_bits();

printLog("[%g] PacketGen: Packet length %g \n",t, templength);
myLoggerToScilab->logSignal(t,templength,"pktlength");

return Event(p,0);
}
void packetgen_v2::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flushToScilab();
}
