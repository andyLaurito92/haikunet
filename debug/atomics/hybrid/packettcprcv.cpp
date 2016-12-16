#include "packettcprcv.h"
void packettcprcv::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

////////////////////////////////// STDEVS
//mySTDEVS = new STDEVS(_STDEVS_DEFAULT_SEED);
//
//myLoggerToScilab = new aLoggerToScilab(this->myself,this->getName());
//myLoggerToScilab->initSignals(1,"cwnd");

for (int i=0;i<10;i++) {
	out_ack[i]=0;
	};

SEQ_NEXT_Expected=1;//
SEQ_LAST_Valid = 0;	// Last valid ACKnowledged packet sequence

SEQ_NXT = 1;			// Next Sequence Number to use for sending
DATA_SEQ_NUMBER = 0 ;

sigma = inf;  			//

return;
//
}
double packettcprcv::ta(double t) {
//This function returns a double.
return sigma;
}
void packettcprcv::dint(double t) {
printLog("[%g] tcpRCV: [INT] \n",t);
	sigma=inf;
return;
}
void packettcprcv::dext(Event x, double t) {
printLog("[%g] tcpRCV: [EXT] \n",t);

// For ANY current State do:

	if (x.port==0)	{ // Data
			p = (Packet*)x.value;
			DATA_SEQ_NUMBER = p->auxiliar_SEQ;
			DATA_TCP_SND_TIMESTAMP = p->lastModifTime;
			printLog("[%g] tcpRCV: Packet ID %u birthtime %g SEQ %u ExpSEQ %u Length %u \n",t, p->ID,p->birthTime,DATA_SEQ_NUMBER,SEQ_NEXT_Expected,p->getLength_bits());

			if (DATA_SEQ_NUMBER==SEQ_NEXT_Expected)	{ // This is a VALID ACK, the expected sequence number
				printLog("[%g] tcpRCV: SEQ %u is VALID \n",t,DATA_SEQ_NUMBER);
				SEQ_LAST_Valid = DATA_SEQ_NUMBER ;
				SEQ_DUP = 0 ;			// Clears the Duplicate ACK counter
				SEQ_NEXT_Expected ++ ;
			} else {						// This is an INVALID ACK, an unexpected acked sequence number
				printLog("[%g] tcpRCV: SEQ %u is INVALID \n",t,DATA_SEQ_NUMBER);
				SEQ_DUP ++ ;			// Clears the Duplicate ACK counter
			};
		};
			sigma=0;
return ;
}
Event packettcprcv::lambda(double t) {
		printLog("[%g] tcpRCV: Sending ACK %u \n",t,SEQ_LAST_Valid);

		// For ANY current State do:
		out_ack[0]=(double)SEQ_LAST_Valid ;
		out_ack[1]=(double)DATA_TCP_SND_TIMESTAMP;
		out_ack[2]=0.0 ;
		return Event(&out_ack,0) ;
}
void packettcprcv::exit() {
//Code executed at the end of the simulation.
// myLoggerToScilab->flushToScilab();
}
