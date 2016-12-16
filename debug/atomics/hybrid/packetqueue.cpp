#include "packetqueue.h"
void packetqueue::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//  %Name% is the parameter name
//	%Type% is the parameter type

char *fvar;
       fvar = va_arg(parameters,char*);

if (strcmp(fvar,"end_of_params")!=0) { // Assumes that the parameter list ends ALWAYS with a parameter "end_of_params"
       	// There are parameters for the queue, it is not a default infinite capacity queue
		MaxCapacity_bytes = (int)getScilabVar(fvar);
	   	fvar = va_arg(parameters,char*);
	   	ForcedPeriod = getScilabVar(fvar);
  } else { 
       	// No parameters passed, assumes infinite capacity and no forcing departure period
	   	MaxCapacity_bytes=(int)inf;
	   	ForcedPeriod=inf;
};

     printLog("[%g] Queue: MaxCapacity: %i \n",t,MaxCapacity_bytes);
   	 printLog("[%g] Queue: ForcedPeriod: %g \n",t,ForcedPeriod);

sigma=ForcedPeriod;

for (int i=0;i<10;i++) {
sizeout[i]=0;}

myLoggerToScilab = new aLoggerToScilab(1, this->myself,this->getName());
myLoggerToScilab->initSignals(2,"waittime","queueLength");

mystate=WAITING;
serverstate=SERVERBUSY;

return;
}
double packetqueue::ta(double t) {
//This function returns a double.
return sigma;
}
void packetqueue::dint(double t) {

switch (mystate) {
		case WAITING:
		   serverstate=SERVERWAITING;		   					  
  		   mystate=t_NOTIFYING;
           printLog("[%g] Queue: Ready to notify new size \n",t);
  		   sigma=0;
  		   return;
	  
		case t_NOTIFYING:  // Notified length, now emit 
  		   mystate=t_READYTOEMIT;
  		   sigma=0;
           return ; 

		case t_READYTOEMIT:  
	        // Emitted a packet, now notify length again
	        if (serverstate==SERVERWAITING && (int)myPacketQueue.size()>0) {
				myPacketQueue.pop();
				printLog("[%g] Queue: DEQUEUED SIZE: %u \n",t, (int) myPacketQueue.size()); 
				serverstate=SERVERBUSY;		   					  
				printLog("[%g] Queue: Ready to notify new size \n",t);
  		   		mystate=t_NOTIFYING;
  		   		sigma=0;
		   	 } else {
	        // Nothing else to do, go back waiting 
  		   		mystate=WAITING;
  		   		sigma=ForcedPeriod;
	   	        printLog("[%g] Queue: Go to wait again \n",t);
			 };
	 };
return;
}
void packetqueue::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition

if (x.port==0) {	    // Packet Arrived
	p = (Packet*)x.value;
	myprotoStack = p->ProtocolList;

	if (MaxCapacity_bytes==(int)myPacketQueue.size()) { // Queue Full: Rejects
	   printLog("[%g] Queue: Incoming Packet ID %i Refused (MaxCapacity %i reached) \n",t,p->ID,MaxCapacity_bytes); 
	   mystate=WAITING;
	   sigma=sigma-e;
	   return;
    }

    // Otherwise : Accepts
    p->currentReceptionTimestamp = t;
    myPacketQueue.push(p);

   	  // //// LOGGING
	  printLog("[%g] Queue: enqueueing myprotoStack contains: \n",t);
	  printLog("[%g] Queue: enqueueing Packet ID %u \n",t, p->ID); 
	  for (myprotoStackit=myprotoStack.begin(); myprotoStackit!=myprotoStack.end(); myprotoStackit++) {
	  printLog("[%g] Queue: enqueueing ID %u \n",t,(*myprotoStackit)->protoID);
	    myFields = (*myprotoStackit)->headerFields;
			 for ( mapIt=myFields.begin() ; mapIt != myFields.end(); mapIt++ )
	  			printLog("[%g] Queue: enqueueing field value %g \n",t,(*mapIt).second);
	  };
	  printLog("[%g] Queue: QUEUE SIZE: %u \n",t, (int)myPacketQueue.size());
   	  // \\\\ LOGGING

	mystate=t_NOTIFYING;
	sigma=0;
  }
  
if (x.port==1) {		// Request from Server Arrived
      printLog("[%g] Queue: Dequeue Requested by Server... \n",t); 

      if ((int)myPacketQueue.size()>0){ // Queue is not empty
    	   printLog("[%g] Queue: Non Empty Queue... \n",t);
   	  	   serverstate=SERVERWAITING;
   	  	   mystate=t_READYTOEMIT;
   	  	   sigma=0; // Force dequeueing next available element at the head of the queue;
	  } else {                       // Queue is empty
    	   printLog("[%g] Queue: Dequeue Failed: Empty Queue... \n",t); 
   	  	   serverstate=SERVERWAITING;
   	  	   mystate=t_READYTOEMIT;
		   sigma=sigma-e; // Continues waiting for a new packet
		   		};
   };
return;
}
Event packetqueue::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)
	int qz = (int) myPacketQueue.size();
		    
	switch (mystate) {
		case WAITING:  // Forced Period Elapsed
	       // Sends Nothing
	       return Event();
	       break;
		case t_NOTIFYING:  // Must notify new queue length
 			printLog("[%g] Queue: Notifying QUEUE SIZE: %u \n",t,qz);
			myLoggerToScilab->logSignal(t,(double)qz,"queueLength");
			sizeout[0] = (double)qz;
		    return Event(&sizeout,1);
		    break;
		case t_READYTOEMIT:  
	        if (serverstate==SERVERWAITING && (int)myPacketQueue.size()>0) {
			   // Must emit a packet
	            printLog("[%g] Queue: Trying to DEQUEUE... \n",t); 
			    printLog("[%g] Queue: NOW QUEUE SIZE: %u \n",t, (int) myPacketQueue.size());
					pout = myPacketQueue.front();
					pout->lastQueueSizeAfterLeaving=(int)myPacketQueue.size()-1;
				    printLog("[%g] Queue: DEqueueing Packet ID %u \n",t, pout->ID); 
					double waittime=t - p->currentReceptionTimestamp;
					myLoggerToScilab->logSignal(t,waittime,"waittime");
					return Event(pout,0);
  	         } else {
					return Event(&sizeout,1);
			 };
			 break;
	 };
}
void packetqueue::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flush();
}
