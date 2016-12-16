#include "packetred.h"
void packetred::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//  %Name% is the parameter name
//	%Type% is the parameter type

//////////////////////////////// STDEVS
//		mySTDEVS = new STDEVS(_STDEVS_DEFAULT_SEED);
//		mySTDEVS = new STDEVS(_STDEVS_UNIQUE_SEED);
		mySTDEVS = new STDEVS(12345678);

char *fvar;
fvar = va_arg(parameters,char*);
minTH = (int)getScilabVar(fvar);
fvar = va_arg(parameters,char*);
PminTH = getScilabVar(fvar);
fvar = va_arg(parameters,char*);
maxTH = (int)getScilabVar(fvar);
fvar = va_arg(parameters,char*);
PmaxTH = getScilabVar(fvar);

/* initialize random seed: */
srand(time(NULL));

for (int i=0;i<10;i++) {
dropped[i]=0;}

measuredQueueState=0;
drop_prob=0;

myLoggerToScilab = new aLoggerToScilab(this->myself,this->getName());
myLoggerToScilab->initSignals(2,"probdrop","measuredQueue");

phase=WAITINGFORPACKET;
sigma=inf;  // Goes waiting 
return;
}
double packetred::ta(double t) {
//This function returns a double.
return sigma;
}
void packetred::dint(double t) {
phase = WAITINGFORPACKET ;
sigma = inf ;
}
void packetred::dext(Event x, double t) {
//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition

if (x.port==0) {   // A new packet enters 
	// LOG
	p = (Packet*)x.value;
	myprotoStack = p->ProtocolList;
	int lastProtoID;
	///////////////
	  printLog("[%g] RED: received myprotoStack contains: \n",t);
	  printLog("[%g] RED: received Packet ID %u \n",t, p->ID); 
	  printLog("[%g] RED: with birthtime %g \n",t, p->birthTime); 
	
	  for (myprotoStackit=myprotoStack.begin(); myprotoStackit!=myprotoStack.end(); myprotoStackit++) {
	  lastProtoID=(*myprotoStackit).protoID;
	  printLog("[%g] RED: received ID %u \n",t,lastProtoID);
	    myFields = (*myprotoStackit).headerFields;	
			 for ( mapIt=myFields.begin() ; mapIt != myFields.end(); mapIt++ )
	  			  printLog("[%g] RED: received field value %g \n",t,(*mapIt).second);};

	// DECIDE DROP ACTION
	double coinflip=mySTDEVS->uniform();
    printLog("[%g] RED: coinflip: %g drop_prob: %g \n",t,coinflip,drop_prob);

	// CORRECT:
			if (coinflip<=drop_prob)
			{
		           sigma=0;    // DROP: Notify it
		           phase=T_NOTIFYINGDROP;
		           // delete p;     // Destroy the dropped packet object // TBD: Should destroy the packet to release memory, but this
		                            // would require TCP to send COPIES of packets, instead of the pointers TCP holds in its buffer
		                            // Deleting here would require TCP to mantain an "Internal Copy Buffer"; this should be implemented in the future
				   printLog("[%g] RED: DROPPED ! \n",t);
				   dropped[0]++;
		           return;
			} else {
		           sigma=0;    // DO NOT DROP: Emit the packet
		   	  	   phase=T_EMITTINGPACKET;
				   printLog("[%g] RED: ADMITTED ! \n",t);
		   	  	   return;
		    } ;
};

if (x.port==1) {   // A new Queue Length is notified 
	double *Aux = (double*)x.value;// LOG
	measuredQueueState=(int)(*Aux);
	
	if (measuredQueueState<minTH) {
	   drop_prob=0; }
	if (measuredQueueState==minTH) {
	   drop_prob=PminTH; }
	if (measuredQueueState==maxTH) {
	   drop_prob=PmaxTH; }
	if (measuredQueueState>maxTH) {
	   drop_prob=1; }
	if (measuredQueueState>minTH && measuredQueueState<maxTH ) {
	   drop_prob=PminTH+(PmaxTH-PminTH)/(maxTH-minTH)*(measuredQueueState-minTH); }

    printLog("[%g] RED: measuredQueueState: %u new drop_prob: %g \n",t,measuredQueueState,drop_prob);
    printLog("[%g] RED: minTH %i maxTH %i PminTH %g PmaxTH %g \n",t,minTH,maxTH,PminTH,PmaxTH);
	myLoggerToScilab->logSignal(t,drop_prob,"probdrop");
	myLoggerToScilab->logSignal(t,measuredQueueState,"measuredQueue");

    phase=WAITINGFORPACKET;
    sigma=inf;    
    return;
};
}
Event packetred::lambda(double t) {
//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)
switch (phase)
		{
		case T_NOTIFYINGDROP: 
			return Event(&dropped,1);
            break;
		case T_EMITTINGPACKET: 
			return Event(p,0);
            break;
        default:
			printLog("[%g] Channel: Error: Do not know what to do... \n",t);
			return Event();
			break;
		};
}
void packetred::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flushToScilab();
}
