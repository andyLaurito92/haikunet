#include "packetsink.h"
void packetsink::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

sigma=1E10;
counter=0;

logger = new aLoggerToScilab(this->myself,this->getName());
logger->initSignals(1,"arrived");

return;
}
double packetsink::ta(double t) {
//This function returns a double.
return sigma;
}
void packetsink::dint(double t) {
//for ( it=mymap.begin() ; it != mymap.end(); it++ )
//      printLog("[%g] PacketSink: key = %u val = %u \n",t,(*it).first,(*it).second);
sigma=1E10;
}
void packetsink::dext(Event x, double t) {
//The input event is in the 'x' variable.
//where:
//     'x.value' is the value (pointer to void)
//     'x.port' is the port number
//     'e' is the time elapsed since last transition

p = (Packet*)x.value;
myprotoStack = p->ProtocolList;

counter++;
logger->logSignal(t,(double)counter,"arrived");

///////////////
  printLog("[%g] PacketSink: received myprotoStack contains: \n",t);
  printLog("[%g] PacketSink: received Packet ID %u \n",t, p->ID); 
  printLog("[%g] PacketSink: with birthtime %g \n",t, p->birthTime); 
  printLog("[%g] PacketSink: from FLOW: %s \n",t, p->FlowID); 
  
  for (myprotoStackit=myprotoStack.begin(); myprotoStackit!=myprotoStack.end(); myprotoStackit++) {
  printLog("[%g] PacketSink: received ID %u \n",t,(*myprotoStackit).protoID);
    myFields = (*myprotoStackit).headerFields;	
		 for ( mapIt=myFields.begin() ; mapIt != myFields.end(); mapIt++ )
  			printLog("[%g] PacketSink: received field value %g \n",t,(*mapIt).second);
			};
  printLog("[%g] PacketSink: received Packet Length %i \n",t,p->getLength_bits());
  printLog("[%g] PacketSink: Destroying Packet \n",t);  
  delete p;
}
Event packetsink::lambda(double t) {
//This function returns an Event:
//     Event(%&Value%, %NroPort%)
//where:
//     %&Value% points to the variable which contains the value.
//     %NroPort% is the port number (from 0 to n-1)
}
void packetsink::exit() {
//Code executed at the end of the simulation.
logger->flushToScilab();
}
