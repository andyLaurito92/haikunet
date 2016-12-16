#include "packetchannel.h"
void packetchannel::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type
char *fvar;

fvar = va_arg(parameters,char*);
latency = (double)getScilabVar(fvar);
printLog("[%g] Channel: Latency: %g ms. \n",t,latency);

fvar = va_arg(parameters,char*);
bandwidth = (double)getScilabVar(fvar);
printLog("[%g] Channel: BandWidth: %g bits/sec \n",t,bandwidth);

fvar = va_arg(parameters,char*);
overflowAction = fvar;
printLog("[%g] Channel: OnOverflow: %s \n",t,overflowAction);

fvar = va_arg(parameters,char*);
printLog("[%g] Channel: LogStatsToScilab: %s \n",t,fvar);
if (strcmp(fvar,"On")==0){enableLogSignals=true;} else {enableLogSignals=false;};

BWDelayProduct=(latency/1000)*bandwidth;
tAllowNewPacket=t; // Should be t=0 at init
currentInFlightSize = 0;

nextState=IDLE;
state=nextState;

nextSigma=inf;
sigma=nextSigma;  // Goes waiting for a new packet;

printLog("[%g] Channel: Model ID: %s \n",t,this->getName());

arrived=0;
departed=0;
dropped=0;

myLoggerToScilab = new aLoggerToScilab(this->myself,this->getName());
myLoggerToScilab->initSignals(5,"arrived","departed","dropped","queuesize","proptime");

return;
}
double packetchannel::ta(double t) {
//This function returns a double.
return sigma;
}
void packetchannel::dint(double t) {
switch (state)
		{
		case SENDING:
			double channelsize = (double)myPacketChannel.size();
            myLoggerToScilab->logSignal(t,channelsize,"queuesize");
	 	    printLog("[%g] Channel: now contains %u packets (in flight: %i) \n",t, channelsize, currentInFlightSize);
			if (channelsize>0){ // Channel is still not empty
					p = myPacketChannel.front(); // Gets the next awaiting packet
					nextSigma=(p->currentReceptionTimestamp) + (p->currentChannelPropagationTime / 1000) - t;
				 	printLog("[%g] Channel: Next propagation time: %g (ArrTs:%g ChProp:%g t:%g) \n",t, nextSigma,p->currentReceptionTimestamp,p->currentChannelPropagationTime / 1000,t); 
					nextState=SENDING;
				} else {		// Channel is empty
					nextSigma=inf;
				 	printLog("[%g] Channel: Nothing left to transmit \n",t); 
					nextState=IDLE;	            
				};
		}
state=nextState;
sigma=nextSigma;
}
void packetchannel::dext(Event x, double t) {
//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
	int packetLength;
	double violationInterval;
	bool drop = false;
	enum overflowtype {NONE,BWDELPROD,BW};
	overflowtype overflow;
	
	p = (Packet*)x.value;

	printLog("[%g] Channel: received Packet ID %u \n",t, p->ID); 
	printLog("[%g] Channel: with birthtime %g \n",t, p->birthTime); 

	arrived++;
    myLoggerToScilab->logSignal(t,arrived,"arrived");

	// FOR TESTING PURPOSES ONLY:
		// p->setLength(rand()%15000); // Forced artificial packet length; Testing purpose
		// p->setLength(0); // Forced artificial packet length; Testing purpose
	    // packetLength=p->calcLength(); // the true packet length, summing up each protocol size
	packetLength=p->getLength_bits(); 
	// packetLength=500*8; //1; //500;
	
	propTime=p->setPropagationTime(packetLength,latency,bandwidth); // in ms.
	printLog("[%g] Channel: Current Packet propagation time: %g ms. \n",t, propTime); 
	
	// Overflow Control

	if (t<tAllowNewPacket){   								// Violates BW restriction
	        violationInterval=tAllowNewPacket-t;
	        overflow=BW;
			printLog("[%g] Channel: BW Overflow (t violation by: %g) \n",t, violationInterval); 
			printLog("[%g] Channel: next t Allowed was: %g) \n",t, tAllowNewPacket); 
			if ((currentInFlightSize+packetLength)>BWDelayProduct) {   // Violates BWDelayProduct restriction
				// Allow stacking a burst of packets exceeding BW, but Controls BW-Delay Product Limit
				printLog("[%g] Channel: BWDelayP Overflow (in flight: %i) \n",t, currentInFlightSize); 
				overflow=BWDELPROD; 
				}
	} else {                                                // DO NOT Violate BW restriction
	        violationInterval=0;
			overflow=NONE; };
	
	drop = false;
	
	if (overflow==BW) {
         if ( strcmp(overflowAction,"DropIfBWExceeded")==0 || strcmp(overflowAction,"DropAlways")==0 ) {
	 	   			 drop=true; };}
	if (overflow==BWDELPROD) {
         if ( strcmp(overflowAction,"DropIfCapacityExceeded")==0 || strcmp(overflowAction,"DropAlways")==0 ) {
	 	   			 drop=true; };}
						  
    if (drop) {	   							// DROP
	 	   		printLog("[%g] Channel: Overflow: DROP Packet ID %u \n",t, p->ID); 
                delete p;
                
				dropped++;
				myLoggerToScilab->logSignal(t,dropped,"dropped");
				
	   			nextState=state; // Remains in the previous state
	   			nextSigma=sigma-e; // Continues counting with the active timer
		} else { 							// ACCEPT
		  	   p->currentReceptionTimestamp = t + violationInterval; // Also considers a potential VIRTUAL Reception Time (allows bursts exceeding bandwidth)
			   								  	  					 // NOTICE: This is an ABSOLUTE time reference
	           currentInFlightSize+=packetLength;
		       tAllowNewPacket+=propTime/1000;	// NOTICE: This is an ABSOLUTE time reference
 		  	   myPacketChannel.push(p);
 		  	   
         	   double channelsize = (double)myPacketChannel.size();
 		  	   myLoggerToScilab->logSignal(t,channelsize,"queuesize");
 		  	   
			   switch (state) {
						case IDLE: 
				             nextState=SENDING ;
							 nextSigma=propTime/1000 ; break; // This is the only packet in the channel
						case SENDING:
				             nextState=state ;
						     nextSigma=sigma-e ; break;
						default: printLog("[%g] Channel: Error: Do not know what to do... \n",t); break;
						}
		};
		
sigma=nextSigma;
state=nextState;
}
Event packetchannel::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)
		switch (state)
		{
		case IDLE: 
  			printLog("[%g] Channel: Not expected ! \n",t);
			return Event();
			break;
		case SENDING:
			if ((int)myPacketChannel.size()>0){ // Channel is not empty
				pout = myPacketChannel.front();
				printLog("[%g] Channel: Emitting Packet ID %u \n",t, pout->ID); 
				departed++;
				myLoggerToScilab->logSignal(t,departed,"departed");
				double proptime = t - pout->currentReceptionTimestamp;
				myLoggerToScilab->logSignal(t,proptime,"proptime");
				
  			 	currentInFlightSize-=p->getLength_bits();
				myPacketChannel.pop(); // REMOVES THE PACKET FROM THE CHANNEL // TBD: This could be moved to dint, but this crashes.
				return Event(pout,0);
			} else {
			    printLog("[%g] Channel: Error: No packet to send... \n",t); 
				return Event();
			};
			break;

         default:
			printLog("[%g] Channel: Error: Do not know what to do... \n",t);
			return Event();
			break;
		}
}
void packetchannel::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flushToScilab();
}
