//CPP:ATLAS-TDAQ/TcpSender.cpp
#if !defined TcpSender_h
#define TcpSender_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <vector> // For std::vector
#include <algorithm>    // std::find_if, for_each, min, etc

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "tdaqPacketTool.h"

class TcpSender: public Simulator { 
	class TcpQueuedPacket {
	public:
		TcpQueuedPacket(std::shared_ptr<TdaqPacket> packet){
			this->packet = packet;
		}

		std::shared_ptr<TdaqPacket> packet;
		bool forwarded = false; // once it is forwarded once, this flags stays in true
		bool onTheFly = false; // can be forwarded and !onTheFly if a retransmission occurred (so we consider is no onTheFly any more, maybe was dropped)
		int retransmissions = false;
		bool ackReceived = false;
		std::shared_ptr<TdaqPacket> ackPacket = NULL;
	};

	// Parameters
	double minRetransmissionTimeOut;
//	int startingCongestionWindow;
	int slowStartThreshold;
	double rttAlpha;

	// Helpers
	//std::shared_ptr<IPowerDEVSLogger> logger;

	// State Variables
	double sigma;
	std::deque<std::shared_ptr<TcpQueuedPacket>> sendBuffer; // all packets which where not ACKed
	double lastTransmissionTime; // last t a packet was sent (in conjunction with the RTO is the retransmission timer)
	double retransmissionTimeOut; // RTO
	int congestionWindow; // CWND

	int lastSequenceNumber; // SEQ numbers to use in packets

	// For RTT estimation calculation
	double calculatedRtt;
	double calculatedRttDeviation;

	// to manage CA
	int pendingCongestionAvoidanceAcksToIncreaseWindow;

	// to manage FAST-Retransmit


	enum STATE { FORWARDING_PACKET, RETRANSMITTING_PACKET, WAITING };
	STATE myState;
	enum TCP_CONGESTION_AVOIDANCE_STATE { SLOW_START, CONGESTION_AVOIDANCE};
	TCP_CONGESTION_AVOIDANCE_STATE congestionAvoidanceState;

	// for logging
	int totalRetransmissions;
	int totalSentPackets;
	int ignoredAcks;

public:
	TcpSender(const char *n): Simulator(n) {};
	void init(double, ...);
	double ta(double t);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
	void exit();
private:
	double setNextSigma(double t);
	double getNextRetransmissionTime();

	int getAvailableCongestionWindow();
	int getUnAckPackets();

	int setNewRTOAfterAckReceived(double t, std::shared_ptr<TcpQueuedPacket> receivedPacket);
	int setNewCAAfterAckReceived(double t, std::shared_ptr<TcpQueuedPacket> receivedPacket);
	int setNewRTOAfterARetransmission();
	int setNewCAAfterARetransmission();


	std::shared_ptr<TcpQueuedPacket> getNextPacketToRetransmit();
	std::shared_ptr<TcpQueuedPacket> getNextPacketPendingForward();

	std::deque<std::shared_ptr<TcpSender::TcpQueuedPacket>>::iterator validateReceivedAckPacket(double t, std::shared_ptr<TdaqPacket> arrivedPacket);

};
#endif
