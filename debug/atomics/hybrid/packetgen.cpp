#include "packetgen.h"
void packetgen::init(double t,...) {
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters,t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	  %Type% is the parameter type

	//////////////////////////////// STDEVS
	//    mySTDEVS = new STDEVS(_STDEVS_DEFAULT_SEED);
		  mySTDEVS = new STDEVS(_STDEVS_UNIQUE_SEED);
	//    mySTDEVS = new STDEVS(12345678);

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

	myFields[IP_SRC] = 70.1; // foo value for testing purposes
	myFields[IP_DST] = 80.2; // foo value for testing purposes
	myFields[SEQ] = 90.3; // foo value for testing purposes

	myprotocol.headerFields=myFields;
	// myprotocol.METADATA[OVERHEAD]=mySTDEVS->poisson(500);
	this->myprotoStack.push_back(std::make_shared<protocol>(myprotocol));

	sigma=StartTime;

	counter=0;

	myLoggerToScilab = new aLoggerToScilab(1, this->myself,this->getName());
	myLoggerToScilab->initSignals(2,"pktlength","intergen");

	return;
}

double packetgen::ta(double t) {
	//This function returns a double.
	return sigma;
}

void packetgen::dint(double t) {
	// FOR TESTING PURPOSES ONLY:
	// for ( it=mymap.begin() ; it != mymap.end(); it++ )
	//     printLog("[%g] PacketGen: key = %u val = %u \n",t,(*it).first,(*it).second);
	// 	  sigma=Period; // Forces a Fixed Period

	sigma=mySTDEVS->exponential(Period);
	myLoggerToScilab->logSignal(t,sigma,"intergen");

	// FOR TESTING PURPOSES ONLY:
	//if (t>200) {sigma=mySTDEVS->exponential(Period*2.05);}; // FORCES A STEP IN THE INTERARRIVAL TIMES

	return;
}
void packetgen::dext(Event x, double t) {
	//The input event is in the 'x' variable.
	//where:
	//     'x.value' is the value (pointer to void)
	//     'x.port' is the port number
	//     'e' is the time elapsed since last transition
}
Event packetgen::lambda(double t) {
	//This function returns an Event:
	//     Event(%&Value%, %NroPort%)
	//where:
	//     %&Value% points to the variable which contains the value.
	//     %NroPort% is the port number (from 0 to n-1)

	///////////////
	// ADDs new protocols to the packet to track them along a full network path
	//  for (int i=1; i<=1; i++) {
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
			printLog("[%g] PacketGen: ID %u \n",t, (*myprotoStackit)->protoID);
			myFields = (*myprotoStackit)->headerFields;
			for ( mapIt=myFields.begin() ; mapIt != myFields.end(); mapIt++ )
			printLog("[%g] PacketGen: field value %g \n",t,(*mapIt).second);
			}

	printLog("[%g] PacketGen: emitting protocol ID %u \n",t, myprotocol.protoID);

	counter++;
	p = new Packet;
	p->ID = counter;
	p->FlowID = this->FlowID;

	///////////////
	// EXPERIMENTAL
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
	p->birthTime=t;

	// FOR TESTING PURPOSES ONLY:
	// myprotoStack.back().METADATA[OVERHEAD] = 10*8;
	// myprotoStack.back().METADATA[PAYLOADSIZE] = (int)mySTDEVS->exponential(490*8)%(1000*8);

	myprotoStack.back()->METADATA[PAYLOADSIZE_BITS] = Psize; // Deterministic Case
	// myprotoStack.back().METADATA[PAYLOADSIZE] = (int)mySTDEVS->exponential(Psize); // Random size
	printLog("[%g] PacketGen: created packet #%i with tiemstamp %g \n",t, p->ID, p->birthTime);

	p->ProtocolList = myprotoStack;
	printLog("[%g] PacketGen: packet list size %u \n",t, (int)p->ProtocolList.size());

	p->calcLength();
	double templength=(double)p->getLength_bits();
	printLog("[%g] PacketGen: Packet length %g \n",t, templength);

	myLoggerToScilab->logSignal(t,templength,"pktlength");

	return Event(p,0);
}

void packetgen::exit() {
	//Code executed at the end of the simulation.
	myLoggerToScilab->flush();
}
