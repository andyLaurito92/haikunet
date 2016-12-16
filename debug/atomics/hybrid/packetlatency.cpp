#include "packetlatency.h"
void packetlatency::init(double t,...) {
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

nextState=IDLE;
state=nextState;
nextSigma=inf;
sigma=nextSigma;  // Goes waiting for a new packet;

printLog("[%g] Channel: Model ID: %s \n",t,this->getName());

arrived=0;
departed=0;

myLoggerToScilab = new aLoggerToScilab(this->myself,this->getName());
myLoggerToScilab->initSignals(4,"arrived","departed","queuesize","proptime");

return;
}
double packetlatency::ta(double t) {
//This function returns a double.
return sigma;
}
void packetlatency::dint(double t) {
switch (state)
		{
		case SENDING:
			double channelsize = (double)myPacketChannel.size();
            myLoggerToScilab->logSignal(t,channelsize,"queuesize");
	 	    printLog("[%g] Channel: now contains %u packets \n",t, channelsize);
			if (channelsize>0){ // Channel is still not empty
					p = myPacketChannel.front(); // Gets the next awaiting packet
					nextSigma=(p->currentReceptionTimestamp) + latency/1000 - t;
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
void packetlatency::dext(Event x, double t) {
//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
	p = (Packet*)x.value;
	arrived++;

    myLoggerToScilab->logSignal(t,arrived,"arrived");
	printLog("[%g] Channel: received Packet ID %u \n",t, p->ID);
	printLog("[%g] Channel: with birthtime %g \n",t, p->birthTime); 

	propTime=latency/1000;
	printLog("[%g] Channel: Current Packet propagation time: %g ms. \n",t, propTime); 
	
	   p->currentReceptionTimestamp = t ;
	   myPacketChannel.push(p);

	   double channelsize = (double)myPacketChannel.size();
	   myLoggerToScilab->logSignal(t,channelsize,"queuesize");

	   switch (state) {
				case IDLE:
		             nextState=SENDING ;
					 nextSigma=propTime ; break; // This is the only packet in the channel
				case SENDING:
		             nextState=state ;
				     nextSigma=sigma-e ; break;
				default: printLog("[%g] Channel: Error: Do not know what to do... \n",t); break;
				}

sigma=nextSigma;
state=nextState;
}
Event packetlatency::lambda(double t) {
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
void packetlatency::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flushToScilab();
}
