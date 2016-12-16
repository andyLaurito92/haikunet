//CPP:TcpReceiverBuffer.cpp
#if !defined TcpReceiverBuffer_h
#define TcpReceiverBuffer_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "tdaqPacketTool.h"
#include <algorithm>

/***
 * This model buffer out-of-order packets and forwards them as they are ordered
 * (discards duplicated packets).
 *
 */
class TcpReceiverBuffer: public BaseSimulator {

	class BufferInfo {
	private:
			std::map<int, std::shared_ptr<TdaqPacket>> receivedBuffer;
			int nextExpectedSeqNumber = 0;

			int getFirstBufferedSeqNumber(){
				// TODO: check performance as it is called for every packet. This could be constant with a fixed buffer (like real TCP) or at least a sorted array (insertion is not O(1) in this case).
				// a std::map<K,V> is ordered based on the key, K, using std::less<K> to compare objects. So try just getting the first element from receivedBuffer.begin();
				auto min = std::min_element(receivedBuffer.begin(), receivedBuffer.end(), CompareFirst());
				return min->first;
			}

	public:
		struct CompareFirst	{
		    bool operator()(const std::pair<int ,std::shared_ptr<TdaqPacket>>& left, const std::pair<int ,std::shared_ptr<TdaqPacket>>& right) const
		    {
		        return left.first < right.first;
		    }
		};

		BufferInfo(std::shared_ptr<TdaqPacket> packet){
			int seqNumber = packet->getProtocol(PROTOCOL_ID_TCP)->headerFields[SEQ];

			receivedBuffer.insert({seqNumber, packet});
			printLog(LOG_LEVEL_FULL_LOGGING, "BufferInfo: added first packet #%i (seqNumber:%i).  nextExpectedSeqNumber=%i \n", packet->ID, seqNumber, nextExpectedSeqNumber);
		}

		std::shared_ptr<TdaqPacket> getNextPacket(){ return receivedBuffer.find(nextExpectedSeqNumber)->second; }

		void addPacket(std::shared_ptr<TdaqPacket> packet) {
			int seqNumber = packet->getProtocol(PROTOCOL_ID_TCP)->headerFields[SEQ];

			if(seqNumber < nextExpectedSeqNumber) { // discard already forwarded packets
				printLog(LOG_LEVEL_FULL_LOGGING, "BufferInfo: packet #%i (seqNumber:%i) already forwarded before (ignore it). nextExpectedSeqNumber=%i \n", packet->ID, seqNumber, nextExpectedSeqNumber);
				return;
			}

			if(receivedBuffer.count(seqNumber)) {// packet already in buffer, discard it
				printLog(LOG_LEVEL_FULL_LOGGING, "BufferInfo: packet #%i (seqNumber:%i) already in buffer (ignore it). nextExpectedSeqNumber=%i \n", packet->ID, seqNumber, nextExpectedSeqNumber);
				return;
			}

			receivedBuffer.insert({seqNumber, packet});
			printLog(LOG_LEVEL_FULL_LOGGING, "BufferInfo: added packet #%i (seqNumber:%i).  nextExpectedSeqNumber=%i receivedBuffer size=%i \n", packet->ID, seqNumber, nextExpectedSeqNumber, receivedBuffer.size());
		}

		void removePacket(std::shared_ptr<TdaqPacket> packet) {
			int seqNumber = packet->getProtocol(PROTOCOL_ID_TCP)->headerFields[SEQ];

			receivedBuffer.erase(seqNumber); nextExpectedSeqNumber++;
			printLog(LOG_LEVEL_FULL_LOGGING, "BufferInfo: removing packet #%i (seqNumber:%i).  nextExpectedSeqNumber=%i  receivedBuffer size=%i \n", packet->ID, seqNumber, nextExpectedSeqNumber, receivedBuffer.size());
		}

		bool shouldSendNextPacket(){

			return receivedBuffer.size() > 0 && getFirstBufferedSeqNumber() == nextExpectedSeqNumber;
		}
	};

	// State Variables
	double sigma;
	enum STATE { WAITING, FORWARDING_PACKET };STATE myState;

	std::map<int, std::shared_ptr<BufferInfo>> receivedBufferPerRos;

	// Helpers
	std::shared_ptr<IPowerDEVSLogger> logger;

public:
	TcpReceiverBuffer(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);

private:
	void addPacketToBuffer(std::shared_ptr<TdaqPacket> packet);
	void removePacketFromBuffer(std::shared_ptr<TdaqPacket> packet);
	double getNextSigma();
	std::shared_ptr<TdaqPacket> getNextPacketToSend();

};
#endif
