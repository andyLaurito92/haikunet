#include "packetserver.h"
void packetserver::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

char* fvar;

fvar = va_arg(parameters,char*);
MinTransitDelay = (double)getScilabVar(fvar);
printLog("[%g] Server: MinTransitDelay: %g \n",t,MinTransitDelay);

fvar = va_arg(parameters,char*);
bitsPerSec = (double)getScilabVar(fvar);
printLog("[%g] Server: bitsPerSec: %g \n",t,bitsPerSec);

fvar = va_arg(parameters,char*);
LayerFunction = fvar;
printLog("[%g] Server: LayerFunction: %s \n",t,LayerFunction);

myLoggerToScilab = new aLoggerToScilab(this->myself,this->getName());
myLoggerToScilab->initSignals(3,"serviceTime","packetLength","fluidLength");

ServiceTime=inf;
ForcedServiceTime=-1; // Internal constant period for testing purposes. When >=0 => the ServiceTime is forcet to the ConstantPeriod

mystate=REQUESTING;
sigma=0;  // Inmediately asks for a new packet;
counter=0;
return;
}
double packetserver::ta(double t) {
//This function returns a double.
return sigma;
}
void packetserver::dint(double t) {
//for ( it=mymap.begin() ; it != mymap.end(); it++ )
//      printLog("[%g] Server: key = %u val = %u \n",t,(*it).first,(*it).second);

		switch (mystate)
		{
		case PROCESSING: 
                mystate=REQUESTING;
			 	sigma=0;
                break;
		case REQUESTING:
		        sigma=inf;
			    break;
		default: 
	            printLog("[%g] Server: Error: Do not know what to do... \n",t); 
		    	break;
		}
}
void packetserver::dext(Event x, double t) {
//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
	
	mystate=PROCESSING;  // NOTE: This Server Preempts the previous task if a new packet arrives (unexpectedly) before the completion of the Processing period
		
	p = (Packet*)x.value;
	myprotoStack = p->ProtocolList;
	
	int lastProtoID;
	
	///////////////
	  printLog("[%g] Server: received myprotoStack contains: \n",t);
	  printLog("[%g] Server: received Packet ID %u \n",t, p->ID); 
	  printLog("[%g] Server: with birthtime %g \n",t, p->birthTime);
	  printLog("[%g] Server: Last Queue was left with %u packets \n",t, p->lastQueueSizeAfterLeaving); 
	
	  for (myprotoStackit=myprotoStack.begin(); myprotoStackit!=myprotoStack.end(); myprotoStackit++) {
	  lastProtoID=(*myprotoStackit).protoID;
	  printLog("[%g] Server: received ID %u \n",t,lastProtoID);
	    myFields = (*myprotoStackit).headerFields;	
			 for ( mapIt=myFields.begin() ; mapIt != myFields.end(); mapIt++ )
	  			printLog("[%g] Server: received field value %g \n",t,(*mapIt).second);
	////////////////
			 };

	if (strcmp(LayerFunction,"TX")==0) {  // ADDs a Protocol Layer

	          printLog("[%g] Adding a Protocol Layer to Packet ID %u \n",t, p->ID);
			  myprotocol.protoID=lastProtoID+1;
			  myFields[IP_SRC]++;
			  myFields[IP_DST]++;
			  myFields[SEQ]++;
			  myprotocol.headerFields=myFields;
			  myprotocol.METADATA[OVERHEAD]=0; // In case we would like to add some overhead
		  	  myprotoStack.push_back(myprotocol); // 1 2 3 4 5
		  	  p->ProtocolList = myprotoStack;
			  };	

	if (strcmp(LayerFunction,"RX")==0) {   // REMOVES a Layer
	          printLog("[%g] Removing a Protocol Layer to Packet ID %u \n",t, p->ID);
		  	  myprotoStack.pop_back(); // 1 2 3 4 5
		  	  p->ProtocolList = myprotoStack;	
		};

    // p->calcLength();
    double disclength=(double)p->getLength_bits();
    // templength=500*8; //1; //500;
	printLog("[%g] Server: Received Packet Length %g \n", t, disclength);
	myLoggerToScilab->logSignal(t,disclength,"packetLength");

	int fluidpackets = p->getFluidPackets();
    double fluidlength = disclength * fluidpackets;
	printLog("[%g] Server: Received Packet Fluid Length %g \n", t, fluidlength);
	myLoggerToScilab->logSignal(t,fluidlength,"fluidLength");

	if (p->ComplexityFactor == 0) p->ComplexityFactor=1 ;
	disclength = disclength * p->ComplexityFactor;
	
	ServiceTime = (1 / bitsPerSec) * (disclength + fluidlength) + (MinTransitDelay / 1000) ;
	// ServiceTime = (templength / bitsPerSec) ;
	if (ForcedServiceTime >= 0) {ServiceTime=ForcedServiceTime;} ;
	printLog("[%g] Server: CalculatedServiceTime: %g \n",t, ServiceTime); 
	myLoggerToScilab->logSignal(t,ServiceTime,"serviceTime");
	
	sigma = ServiceTime;
}
Event packetserver::lambda(double t) {
//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)
		switch (mystate)
		{
		case PROCESSING: 
	  			printLog("[%g] Server: Emitting Packet ID %u \n",t, p->ID); 
				return Event(p,0);
            break;
		case REQUESTING:
			 	printLog("[%g] Server: Asking for a new packet... \n",t);
				counter++; 
				return Event(&counter,1);
		    break;
		default: 
				 printLog("[%g] Server: Error: Do not know what to do... \n",t); 
		    break;
		}
}
void packetserver::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flushToScilab();
}
