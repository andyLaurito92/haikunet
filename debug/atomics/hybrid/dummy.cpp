#include "dummy.h"
void dummy::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//  %Name% is the parameter name
//	%Type% is the parameter type

//////////////////////////////// STDEVS
//    mySTDEVS = new STDEVS(_STDEVS_DEFAULT_SEED);
      mySTDEVS = new STDEVS(_STDEVS_UNIQUE_SEED);
//      mySTDEVS = new STDEVS(12345678);


printLog ("STDEVS Init Poisson %u \n", mySTDEVS->poisson(500));
printLog ("STDEVS Init Bernoulli %g \n", mySTDEVS->bernoulli(0.5));

FlowID = va_arg(parameters,char*);
printLog("[%g] PacketGen: FlowID: %s \n",t,FlowID);

char* fvar;

fvar = va_arg(parameters,char*);
Period = (double)getScilabVar(fvar);
printLog("[%g] PacketGen: Period: %g \n",t,Period);

fvar = va_arg(parameters,char*);
Psize = (double)getScilabVar(fvar);
printLog("[%g] PacketGen: Packet Size by Parameter: %g bits \n",t,Psize);

fvar = va_arg(parameters,char*);
StartTime = (double)getScilabVar(fvar);
printLog("[%g] PacketGen: StartTime: %g secs. \n",t,StartTime);

myprotocol.protoID=0;

myFields[IP_SRC] = 70.1;
myFields[IP_DST] = 80.2;
myFields[SEQ] = 90.3;
myprotocol.headerFields=myFields;
// myprotocol.METADATA[OVERHEAD]=mySTDEVS->poisson(500);
myprotoStack.push_back(myprotocol);

sigma=StartTime;

counter=0;

myLoggerToScilab = new aLoggerToScilab(this->myself,this->getName());
myLoggerToScilab->initSignals(2,"pktlength","intergen");

return;
}
double dummy::ta(double t) {
//This function returns a double.
return sigma;
}
void dummy::dint(double t) {
//for ( it=mymap.begin() ; it != mymap.end(); it++ )
//      printLog("[%g] PacketGen: key = %u val = %u \n",t,(*it).first,(*it).second);

// sigma=Period; // Fixed Period
sigma=mySTDEVS->exponential(Period);
myLoggerToScilab->logSignal(t,sigma,"intergen");

// FOR TESTING PURPOSES ONLY:
//if (t>200) {sigma=mySTDEVS->exponential(Period*2.05);}; // FORCES A STEP IN THE INTERARRIVAL TIMES

// if (t>5) sigma=1E10;

return;
}
void dummy::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition
}
Event dummy::lambda(double t) {
//////////
}
void dummy::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flushToScilab();
}
