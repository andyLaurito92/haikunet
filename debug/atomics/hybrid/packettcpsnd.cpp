#include "packettcpsnd.h"
void packettcpsnd::init(double t,...) {
//The 'parameters' variable contains the parameters transferred from the editor.
va_list parameters;
va_start(parameters,t);
//To get a parameter: %Name% = va_arg(parameters,%Type%)
//where:
//      %Name% is the parameter name
//	%Type% is the parameter type

////////////////////////////////////////////////////////////////
// Prefixes
// BUF_ : Pointer into the SEND BUFFER
// WND_ : Pointer into SEND Sliding WINDOW
// SEQ_ : Packet Sequence NUMBER
// APP_ : Relative to the APPlication (upper) Layer
// T_	: Time



char* fvar;
fvar = va_arg(parameters,char*);
// MAX Segment Size
MSS = (int)getScilabVar(fvar);
printLog("[%g] tcpSND: Param MSS: %u \n",t,MSS);

fvar = va_arg(parameters,char*);
// MAX Congestion Window (in packets) inf==NONE, Typical: 64
WND_CWND_MAX = (int)getScilabVar(fvar);
printLog("[%g] tcpSND: Param WND_CWND_MAX: %i \n",t,WND_CWND_MAX);

fvar = va_arg(parameters,char*);
// Experimental Slow Start, 0==OFF
WND_SSTHRESH = (int)getScilabVar(fvar);
printLog("[%g] tcpSND: Param WND_SSTHRESH: %i \n",t,WND_SSTHRESH);

fvar = va_arg(parameters,char*);
// Typical value 0.125. RTT_alfa==0 => No moving average for RTT, only instantaneous measurements
RTT_alfa = (double)getScilabVar(fvar);
printLog("[%g] tcpSND: Param RTT_alfa: %g \n",t,RTT_alfa);

fvar = va_arg(parameters,char*);
// Initial Calculated Round Trip Time
T_RTT = (double)getScilabVar(fvar);
printLog("[%g] tcpSND: Param Initial T_RTT: %g \n",t,T_RTT);

fvar = va_arg(parameters,char*);
// Typical value 3
DUP_ACK_LIMIT = (int)getScilabVar(fvar);
printLog("[%g] tcpSND: Param DUP_ACK_LIMIT: %u \n",t,DUP_ACK_LIMIT);

fvar = va_arg(parameters,char*);
// T_FORCED_RTO>=0 => Forces RTO instead of calculating it
T_FORCED_RTO = (double)getScilabVar(fvar);
printLog("[%g] tcpSND: Param T_FORCED_RTO: %g \n",t,T_FORCED_RTO);

fvar = va_arg(parameters,char*);
// Arbitrary Practical Criteria. If 1 mseg. (i.e. Limits (P)acket (T)hroughput to T=1 KPps, which at 1KByte per Segment renders T=1MBps of user data. or T=8Mbps of bandwidth
// Typical value: 0.000001 (Should look like "instantaneous" compared to RTT, maybe RTT/1000 is a good rule of thumb)
INTERPACKET_SND_TIME = (double)getScilabVar(fvar);
printLog("[%g] tcpSND: Param INTERPACKET_SND_TIME: %g \n",t,INTERPACKET_SND_TIME);

fvar = va_arg(parameters,char*);
// Arbitrary Practical Criteria. 0.1 mseg. Time to wait
// Typical value: 0.000001 (Should look like "instantaneous" compared to RTT, maybe RTT/1000 is a good rule of thumb)
INTER_REQ_TIME = (double)getScilabVar(fvar);
printLog("[%g] tcpSND: Param INTER_REQ_TIME: %g \n",t,INTER_REQ_TIME);

// EXPERIMENTAL:
NchangeFactor=1;  // 1=No Effect (i.e. this is disabled)
NchangeFactorInit=10;
NchangeFactorEnd=20;
//

//////////////////////////////// STDEVS
mySTDEVS = new STDEVS(_STDEVS_DEFAULT_SEED);

myLoggerToScilab = new aLoggerToScilab(this->myself,this->getName()); 
myLoggerToScilab->initSignals(5,"cwnd","packetdrop","measuredRTT","RTO","calcRTT");

////////////////////////////////////////
WND_firsttime_SSTHRESH = true;
BUF_Fully_SNT = false ; //
BUF_SIZE = 0 ;			//
WND_Fully_SNT = false ; //
WND_NXT_SND = 0 ;		// Next position to be sent (when allowed)
						// -> It can overflow and be 1 ahead of the current SEND BUFFER size
SEQ_NEXT_Expected_ACK=1 ;//
SEQ_LAST_Valid_ACK = 1 ;	// Last valid ACKnowledged packet sequence
WND_DUP_ACK = 0 ;		// Amount of Duplicate ACKs received so far
WND_CWND = 1 ;		    // 0 ;	// Congestion Window (in packets)
WND_CWND_precise = 1 ;	// Congestion Window (with fractions)
SEQ_CYCLE_ACK = 0;		// Indicates which sent SEQ number will be considered to determine a RTT CYCLE
						// -> This will be the expected ACK capable of growing W_CWND 1 unit
SEQ_NXT = 1;			// Next Sequence Number to use for sending

ReceivedACKedFromPeerLayer = 0;			// This is the most recent ACK number that 
								// "the other end is confirming as valid"
SendACKtoPeerLayer = 0;			// This is the most actual ACK number
   								// "that can be confirmed to the other end"
T_MEASURED_RTT =  0;		// [[PENDING: init to something standard
T_RTT_DEVIATION = 0; 		// [[PENDING: init to something standard

if (T_FORCED_RTO>=0) {
		T_RTO = T_FORCED_RTO ;                  // FORCED
	} else {
		T_RTO = T_RTT + 4 * T_RTT_DEVIATION;    // CALCULATED
	};

////////////////////////////////////////////////////////////////


// State Transition
mystate = tr_REQxAPP;	// Inmediately asks for the first new packet
sigma = 0;  			// 

return;
//
}
double packettcpsnd::ta(double t) {
//This function returns a double.
return sigma;
}
void packettcpsnd::dint(double t) {
printLog("[%g] tcpSND: [INT] \n",t);

// According to the State do:
	switch (mystate)
		{
		case tr_REQxAPP: // Asks for more data from the upper layer
 			printLog("[%g] tcpSND: int@ tr_REQxAPP \n",t);
			// State Transition
			mystate=WAITxAPP;
		 	sigma=INTER_REQ_TIME;
			printLog("[%g] tcpSND: to WAITxAPP during %g \n",t,sigma);
			//
			break;

		case WAITxAPP: // Waits for data from the upper layer
 			printLog("[%g] tcpSND: int@ WAITxAPP \n",t);
			if (BUF_SIZE>0) { // Buffer NOT Empty
					// State Transition
					mystate=SENDING;
				 	sigma=INTERPACKET_SND_TIME;
					printLog("[%g] tcpSND: to SENDING during %g \n",t,sigma);
					//
					break;
			} else {		// Buffer Empty, nothing to do, stays waiting forever
					// State Transition
					mystate=WAITxAPP;
				 	sigma=inf;
					printLog("[%g] tcpSND: to WAITxAPP during %g \n",t,sigma);
					//
					break;
			};
			
		case SENDING: // Flushing the sender window
			printLog("[%g] tcpSND: int@ SENDING \n",t);

			if (SEQ_CYCLE_ACK==0) {
					// PENDING: SEQ_CYCLE_ACK=pout->myprotocol.headerFields[SEQ];
					SEQ_CYCLE_ACK = pout->auxiliar_SEQ ;
					printLog("[%g] tcpSND: CYCLE_ACK set to: %u \n",t,SEQ_CYCLE_ACK);
					};

      		if (BUF_SIZE>0 && WND_NXT_SND<=BUF_SIZE && WND_NXT_SND<=WND_CWND) { // Send Buffer ! empty AND ! completely sent

            	WND_NXT_SND++ ;
				// State Transition
		  		mystate=SENDING;	//
		  		sigma=INTERPACKET_SND_TIME;
				printLog("[%g] tcpSND: to SENDING during %g \n",t,sigma);

			} else {		// Send Buffer empty

				// State Transition
				mystate=WAITxACK;	//
				sigma=T_RTO;
				printLog("[%g] tcpSND: to WAITxACK during %g \n",t,sigma);

			};
            break;

		case WAITxACK: // Waiting For ACK after having flushed the whole CWND
                printLog("[%g] tcpSND: int@ WAITxACK \n",t);
				printLog("[%g] tcpSND: TCP TIMEOUT -> Try to retransmit buffer \n",t,sigma);

				WND_DUP_ACK=0 ; // [[ to Reconsider

				// CLASSICAL OPTION:
						// WND_CWND = 1;
				// MODIFIED OPTION: (Treats TO as a DUP ACK)
						WND_CWND = abs(WND_CWND / 2);
                        WND_firsttime_SSTHRESH=false;
				if (WND_CWND<1) {WND_CWND=1;};
		 		WND_CWND_precise = WND_CWND ;
		 		myLoggerToScilab->logSignal(t,(double)WND_CWND,"cwnd");
                WND_NXT_SND = 1 ;	   // Update de pointer , NORMAL case
				printLog("[%g] tcpSND: BUF_SIZE %u WND_CWND %u WND_NXT_SND %u \n",t, BUF_SIZE, WND_CWND, WND_NXT_SND);
				BUF_Fully_SNT = false ; 	//
				WND_Fully_SNT = false ; 	//

				SEQ_LAST_Valid_ACK = -1; 	// prevents identifying the next dup acks that might be on-fly as a repetition of this condition (dup ack is detected only once inside the same cycle)
				SEQ_CYCLE_ACK = 0;  		// A new measurement cycle starts all over again
											// This Signals the SENDING state that
											// the next SEQ number will represent a CYCLE_ACK
				printLog("[%g] tcpSND: CYCLE_ACK set to: %u (open to next) \n",t,SEQ_CYCLE_ACK);

				if (BUF_SIZE<WND_CWND) {
					// State Transition
					mystate=tr_REQxAPP; 	// Check if: there is DATA to send ASAP ?
					sigma=0;
					printLog("[%g] tcpSND: to tr_REQxAPP during %g \n",t,sigma);
				} else {
       				// State Transition
					mystate=SENDING;		// Start re-sending from the updated pointer
					sigma=0;
					printLog("[%g] tcpSND: to SENDING during %g \n",t,sigma);
				};
			break;

		default: 
				printLog("[%g] tcpSND: Error: Do not know what to do... \n",t);
		break;
		}

		int first; int last;
		if (BUF_SIZE>0) {
				first=(int)(SendBuffer.at(0)->ID) ;
				last=(int)(SendBuffer.at(BUF_SIZE-1)->ID) ;
			} else {
				first=0;
				last=0;
			};

		printLog("[%g] tcpSND: BUF_SIZE %u WND_CWND %u WND_NXT_SND %u (%i-%i) \n",t, BUF_SIZE, WND_CWND, WND_NXT_SND,first,last);

return;
}
void packettcpsnd::dext(Event x, double t) {
printLog("[%g] tcpSND: [EXT] \n",t);

// For ANY current State do:

	bool _IS_DATA_PACKET ;
	bool _IS_ACK_PACKET ;
	
	bool _IS_VALID_ACK_PACKET ;
	bool _IS_DUP_ACK_LIMIT_PACKET ;

	if (x.port==0)	{ // Data from upper layer (application)
			_IS_DATA_PACKET = true;
			_IS_ACK_PACKET = false;

			p = (Packet*)x.value;
			printLog("[%g] tcpSND: received Packet ID %u \n",t, p->ID); 
			printLog("[%g] tcpSND: with birthtime %g \n",t, p->birthTime); 
			printLog("[%g] tcpSND: Queue was left with %u packets \n",t, p->lastQueueSizeAfterLeaving); 
// Fills packet header with proper fields
//			myprotocol.protoID=TCP;					PENDING: Use these structures properly
//			myprotocol.METADATA[OVERHEAD]=20;
//			myFields[SRC] = 1; myFields[DST] = 2;
//			myFields[SEQ] = SEQ_NXT ;  
			p->auxiliar_SEQ = SEQ_NXT ; // PENDING: Remove
			SEQ_NXT++ ;
//			myprotocol.headerFields = myFields;
//			myprotoStack.push_back(myprotocol);                    
//			p->ProtocolList = myprotoStack;
			
			// NEW packet enters the buffer
    		SendBuffer.push_back(p);   			// New packets are stored at the tail of the double ended queue
			BUF_Fully_SNT = false; 				// The Buffer gets a new packet, so it is not fully sent for sure
			BUF_SIZE = (int)SendBuffer.size();
			if (BUF_SIZE==1) {
                WND_NXT_SND=1;
			} ;

		} else {       // an ACK from RCV module

            _IS_DATA_PACKET = false;
			_IS_ACK_PACKET = true;
			
			double *xv;
			xv=(double*)(x.value);

			// Update the proper ACK sequences of interest (local, and remote)
			ReceivedACKedFromPeerLayer=(int)xv[0];	// This is the most recent ACK number that 
   													// "the other end is confirming as valid"
			OriginalACKedSEQ_TimeStamp=xv[1];	// TimeStamp of the DATA Packet that Originated this ACK

			SendACKtoPeerLayer=(int)xv[2];			// This is the most actual ACK number
				   									// "that can be confirmed to the other end"

// FOR TESTING: (Always the ACK is the expected one)
//		            ReceivedACKedFromPeerLayer = SEQ_NEXT_Expected_ACK;
//
//		            // Also Eventually forces a DUP ACK
//		            if (mySTDEVS->bernoulli(0)==1) {
//		                ReceivedACKedFromPeerLayer = SEQ_NEXT_Expected_ACK-1;
//					};

			printLog("[%g] tcpSND: Reveived ACK info from RCV Module... \n",t);
			printLog("[%g] tcpSND: ACK %u from remote peer (expected %u) \n",t,ReceivedACKedFromPeerLayer,SEQ_NEXT_Expected_ACK);

			if (ReceivedACKedFromPeerLayer==SEQ_NEXT_Expected_ACK)	{ // This is a VALID ACK, i.e., the expected sequence number

				_IS_VALID_ACK_PACKET = true;
      			_IS_DUP_ACK_LIMIT_PACKET = false;

				// OLD packet leaves the buffer
		 		SendBuffer.pop_front();	// DROP the just ACKNOWLEDGED PACKET FROM THE SEND BUFFER
				//

                T_MEASURED_RTT = t - OriginalACKedSEQ_TimeStamp ;
		 		myLoggerToScilab->logSignal(t,T_MEASURED_RTT,"measuredRTT");
		 		
				SEQ_LAST_Valid_ACK = ReceivedACKedFromPeerLayer ;
				SEQ_NEXT_Expected_ACK++ ;
				WND_DUP_ACK=0 ;		// Clears the Duplicate ACK counter
				WND_NXT_SND-- ;		// Updates the pointer to the next packet to be sent, because it is relative to the back of the queue
				if (WND_NXT_SND<1) {WND_NXT_SND=1;}; // Saturates at 1 at low values
				BUF_SIZE = (int)SendBuffer.size();

	 			if (ReceivedACKedFromPeerLayer==SEQ_CYCLE_ACK) { // THIS IS A CYCLE ACK

					T_RTT = RTT_alfa * T_RTT + (1-RTT_alfa) * T_MEASURED_RTT ;
	 				printLog("[%g] tcpSND: RTT: %g \n",t,T_RTT);
		 			myLoggerToScilab->logSignal(t,(double)T_RTT,"calcRTT");
		 			
				 	T_RTT_DEVIATION = RTT_alfa * T_RTT_DEVIATION + (1-RTT_alfa) * fabs(T_RTT - T_MEASURED_RTT);

					if (T_FORCED_RTO>=0) {
							T_RTO = T_FORCED_RTO ;                  // FORCED
						} else {
							T_RTO = T_RTT + 4 * T_RTT_DEVIATION;    // CALCULATED
						};

		 			myLoggerToScilab->logSignal(t,T_RTO,"RTO");

	 				printLog("[%g] tcpSND: Received CYCLE ACK %u (measuredRTT: %g) Now WND_CWND: %u \n",t,SEQ_CYCLE_ACK,T_MEASURED_RTT,WND_CWND);
					if (WND_CWND<WND_SSTHRESH && WND_firsttime_SSTHRESH==true) {
							WND_CWND=WND_CWND*2;    // Simil Slow Start, but only the first time
								if (WND_CWND>WND_SSTHRESH) {
										WND_CWND=WND_SSTHRESH;  // Maximum Window
									};
						} else {
							WND_CWND++;
									// EXPERIMENTAL: User Load Factor (N) Modifier
									if (t>NchangeFactorInit && t<NchangeFactorEnd) {
											WND_CWND = WND_CWND - abs(WND_CWND * (1-NchangeFactor));
										}
									// EXPERIMENTAL
							WND_firsttime_SSTHRESH=false;
							}            // Congestion Avoidance

					if (WND_CWND>WND_CWND_MAX) {
							WND_CWND=WND_CWND_MAX;  // Maximum Window
						};
						
		 			myLoggerToScilab->logSignal(t,(double)WND_CWND,"cwnd");

					SEQ_CYCLE_ACK = 0;  // A new measurement cycle starts all over again
										// This Signals the SENDING state that
										// the next SEQ number will represent a CYCLE_ACK
					printLog("[%g] tcpSND: CYCLE_ACK set to: %u (open to next) \n",t,SEQ_CYCLE_ACK);
					};

			} else {					// This is an INVALID ACK, an unexpected acked sequence number

				printLog("[%g] tcpSND: ACK %u is INVALID (last valid: %d expected: %d) \n",t,ReceivedACKedFromPeerLayer,SEQ_LAST_Valid_ACK,SEQ_NEXT_Expected_ACK);

				_IS_VALID_ACK_PACKET = false;
				_IS_DUP_ACK_LIMIT_PACKET = false;

			   	if (ReceivedACKedFromPeerLayer==SEQ_LAST_Valid_ACK) {  // It is a DUPLICATED ACK
		    			WND_DUP_ACK++;

		    			if (WND_DUP_ACK==DUP_ACK_LIMIT) {		// and Reached LIMIT duplicate ACKs
				          		printLog("[%g] tcpSND: ACK %u -PLE DUP_ACK w/WND_CWND: %u \n",t,DUP_ACK_LIMIT,WND_CWND);
								_IS_DUP_ACK_LIMIT_PACKET = true;

								WND_DUP_ACK=0 ;
								WND_CWND = abs(WND_CWND / 2);
								WND_firsttime_SSTHRESH=false;
								if (WND_CWND<1) {WND_CWND=1;};
						 		WND_CWND_precise = WND_CWND ;
						 		myLoggerToScilab->logSignal(t,(double)WND_CWND,"cwnd");

								WND_NXT_SND = 1 ;	   		// Update the pointer
								BUF_Fully_SNT = false ; 	//
								WND_Fully_SNT = false ; 	//

								SEQ_LAST_Valid_ACK = -1; 	// prevents identifying the next dup acks that might be on-fly as a repetition of this condition (dup ack is detected only once inside the same cycle)
                                
								SEQ_CYCLE_ACK = 0;  // A new measurement cycle starts all over again
													// This Signals the SENDING state that
													// the next SEQ number will represent a CYCLE_ACK
								printLog("[%g] tcpSND: CYCLE_ACK set to: %u (open to next) \n",t,SEQ_CYCLE_ACK);
											};
	                  	} else {

							// Just ignores the Invalid ACK
					};
				};
		};

		int first; int last; int id;
		if (BUF_SIZE>0) {
				first=(int)(SendBuffer.at(0)->ID) ;
				last=(int)(SendBuffer.at(BUF_SIZE-1)->ID) ;
				// printLog("[%g] tcpSND: BUFFER \n",t);
				// for (int j=0;j<=(BUF_SIZE-1);j++) {
				// 	int id = (int)(SendBuffer.at(j)->ID) ;
				// 	printLog("%i \n",id);
				// };
			} else {
				first=0;
				last=0;
			};

		printLog("[%g] tcpSND: BUF_SIZE %u WND_CWND %u WND_NXT_SND %u (%i-%i) \n",t, BUF_SIZE, WND_CWND, WND_NXT_SND,first,last);

// According to the State do:
	
	switch (mystate)
		{
			case WAITxAPP: //  Waiting for data from the upper layer
				printLog("[%g] tcpSND: ext@ WAITxAPP \n",t);
				if(_IS_DATA_PACKET) {
					 if (BUF_SIZE<WND_CWND && p->lastQueueSizeAfterLeaving>0) {  	// The Congestion Window allows the Buffer to be filled with more packets
																					// AND there are more to grab from the Upper Layer
							// State Transition
							mystate=tr_REQxAPP;		//
							sigma=0;
							printLog("[%g] tcpSND: to tr_REQxAPP during %g \n",t,sigma);
							//
							break;
	                    } else { 					// The upper layer buffer is empty
													// OR The Congestion Window is Full
							// State Transition
							mystate=SENDING;		//
							sigma=INTERPACKET_SND_TIME;
							printLog("[%g] tcpSND: to SENDING during %g \n",t,sigma);
							break;
							//
						};
		             };

			case tr_REQxAPP: // Asks for more data from the upper layer
				printLog("[%g] tcpSND: ext@ tr_REQxAPP \n",t);
	   			// State Transition
				sigma=sigma-e;
	 		 break;

			case SENDING: // Flushing the sender window
				printLog("[%g] tcpSND: ext@ SENDING \n",t);
	   			// State Transition
				sigma=sigma-e;
	 		 break;

			case WAITxACK:  //  Waiting for ACKs from the pair RCV node
				printLog("[%g] tcpSND: ext@ WAITxACK \n",t);
				if(_IS_ACK_PACKET) {
				  	if (_IS_VALID_ACK_PACKET) {
			                // State Transition
							mystate=tr_REQxAPP; 	//  Forces an ACK timeout, to consider retransmissions (but here a triple dup ACK happened)
							sigma=0;
							printLog("[%g] tcpSND: to tr_REQxAPP during %g \n",t,sigma);
							break;
						} else {
			                if (_IS_DUP_ACK_LIMIT_PACKET) {
									printLog("[%g] tcpSND: DUP_ACK_LIMIT -> Try to retransmit buffer \n",t,sigma);
									// State Transition
									mystate=SENDING; 	//  Retry sending again with the new WND_CWND and WND_NXT_SND
									sigma=INTERPACKET_SND_TIME;
									printLog("[%g] tcpSND: to SENDING during %g \n",t,sigma);
					  	           	break;
								} else {
									// State Transition
									mystate=WAITxACK; 	//  Remains Waiting for a valid ACK
									sigma=sigma-e;		//  The TIME OUT timer keeps running
									printLog("[%g] tcpSND: to WAITxACK during %g \n",t,sigma);
					  	           	break;
								};
						};
				};

		 		if(_IS_DATA_PACKET) {
		                   // State Transition
						mystate=WAITxACK;  	//  Remains where it was
						sigma=T_RTO ;  		//  Waits again a full TIMEOUT period
						printLog("[%g] tcpSND: to WAITxACK during %g \n",t,sigma);
						break ;
				     };

			default:
					printLog("[%g] tcpSND: Error: Do not know what to do... \n",t);
				break;
			}
return ;
}
Event packettcpsnd::lambda(double t) {
printLog("[%g] tcpSND: [OUT] \n",t);

// For ANY current State do:

double out_cwnd[10]; for (int i=0;i<10;i++) {out_cwnd[i]=0;}
out_cwnd[0]=(double)WND_CWND;

	if (BUF_SIZE<WND_CWND) {
		int MAX_ALLOWED_TO_SND=BUF_SIZE;
	} else {
		int MAX_ALLOWED_TO_SND=WND_CWND;
	};

// According to the current State do:
	
	switch (mystate)
		{
		case tr_REQxAPP: // Asks for more data from the upper layer
		printLog("[%g] tcpSND: out@ tr_REQxAPP \n",t);
			// We only need to send ANY event value to the queue, but we send the cwnd value to be able to route this signal to a plot
			return Event(&out_cwnd,1);
			break;

		case WAITxAPP:
		printLog("[%g] tcpSND: out@ WAITxAPP [outs nothing] \n",t);
			// Do not send any Output
			return Event();
            break;

		case SENDING: // Flushing the sender window 
		printLog("[%g] tcpSND: out@ SENDING \n",t);
      		if (BUF_SIZE>0 && WND_NXT_SND<=BUF_SIZE && WND_NXT_SND<=WND_CWND)  { // Send Buffer !empty AND !completely sent
					pout = SendBuffer.at(WND_NXT_SND-1);  // -1 is because first element is pointed as 0
					//// FOR TESTING: Drops a packet randomly ->  Emulates a "SOURCE FAILURE"
					//		            if (mySTDEVS->uniform()<=0.001) {
					//						printLog("[%g] tcpSND: SOURCE FAILURE for Packet ID %u (at position %u) [sends nothing]\n",t, pout->ID,WND_NXT_SND);
					//		 				myLoggerToScilab->logSignal(t,1,"packetdrop");
					//		                return Event();
					//					} else {
					//						pout->lastModifTime=t; // Sets the sending timestamp
					//						printLog("[%g] tcpSND: Sending Packet ID %u (at position %u) with timestamp %g \n",t, pout->ID,WND_NXT_SND,pout->lastModifTime);
					//						return Event(pout,0);
					//				};
					//// NORMAL:
					pout->lastModifTime=t; // Sets the sending timestamp
					printLog("[%g] tcpSND: Sending Packet ID %u (at position %u) with timestamp %g \n",t, pout->ID,WND_NXT_SND,pout->lastModifTime);
					return Event(pout,0);
			} else {				// Send Buffer empty
					printLog("[%g] tcpSND: Buffer Empty or Fully Sent ! [outs nothing] \n",t);
					return Event();
			 		// Do not send any Output
					};
            break;

		case WAITxACK:
		printLog("[%g] tcpSND: out@ WAITxACK [outs nothing] \n",t);
			 // Do not send any Output
			return Event();
            break;

		default: 
	      	printLog("[%g] tcpSND: Error: Do not know what to do... \n",t);
		  	return Event();
		  	break;
		};
}
void packettcpsnd::exit() {
//Code executed at the end of the simulation.
myLoggerToScilab->flushToScilab();
}
