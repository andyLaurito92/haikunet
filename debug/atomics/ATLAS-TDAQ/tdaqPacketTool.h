//CPP:ATLAS-TDAQ/tdaqPacketTool.cpp
#if !defined TDAQPACKETTOOL
#define TDAQPACKETTOOL

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "hybrid/packettool.h"
#include <vector>
#include <memory>

// Type definitions
struct HltsvRequest { // fragment information sent by ROSes
	int puId;
	int eventId;

	HltsvRequest(int pu,int event){
		puId = pu;
		eventId = event;
	}
};

struct FragmentInfo { // fragment information sent by ROSes
	int eventId;
	int rosId;
	int robId;
	int fragmentSize;

	FragmentInfo (int eventId, int rosId, int robId){
		this->eventId = eventId;
		this->rosId=rosId;
		this->robId=robId;
		this->fragmentSize=fragmentSize;

	}

	FragmentInfo (int eventId, int rosId, int robId, int fragmentSize){
		this->eventId = eventId;
		this->rosId=rosId;
		this->robId=robId;
		this->fragmentSize=fragmentSize;

	}

	bool operator==(const FragmentInfo& other) const {
		return other.robId == robId && other.rosId == rosId && other.eventId == eventId;
	};
};

struct FragmentRequestInfo { // a request for fragments sent from DCM to ROS
	int eventId;
	int rosId;
	std::vector<int> robIds;
	int puId;
	int dcmId;

	FragmentRequestInfo (int eventId, int rosId, std::vector<int> robIds, int puId, int dcmId){
		this->eventId = eventId;
		this->rosId=rosId;
		this->robIds=robIds;
		this->puId=puId;
		this->dcmId=dcmId;
	}

	//	bool operator==(const FragmentInfo& other) const {
	//		return other.rob == rob && other.ros == ros;
	//	};

	const std::vector<std::shared_ptr<FragmentInfo>> toFragmentInfo() const{
		std::vector<std::shared_ptr<FragmentInfo>> ret;

		for(int robId : robIds){
			auto info =  std::make_shared<FragmentInfo> (eventId, rosId, robId);
			ret.push_back(info);
		}

		return ret;
	}
};

struct EventInfo {
	int eventId;
	int puId;
	std::vector<FragmentInfo> fragmentsInfo;
	double birthTime;
	double l2ProcTime;

	EventInfo(int eventId,int puId, std::vector<FragmentInfo> fragmentsInfo, double birthTime, double procTime){
		this->eventId=eventId;
		this->puId=puId;
		this->fragmentsInfo=fragmentsInfo;
		this->birthTime=birthTime;
		this->l2ProcTime = procTime;
	}

	unsigned int calculateFragmentsSize(){
		int eventSize = 0;
		for (std::vector<FragmentInfo>::iterator it = this->fragmentsInfo.begin() ; it != this->fragmentsInfo.end(); ++it){
			eventSize += (*it).fragmentSize;
		}

		return eventSize;
	}

};



class TdaqPacket: public Packet {

public:
	std::shared_ptr<FragmentInfo> getFragmentInfo();
	std::shared_ptr<FragmentRequestInfo> getFragmentRequestInfo();
	std::shared_ptr<EventInfo> getEventInfo();
	std::shared_ptr<protocol>  getProtocol(int protocolID);

	/* Factory methods */
	static std::shared_ptr<TdaqPacket> createHltsvRequestPacket(double t, std::shared_ptr<HltsvRequest> hltsvRequest);
	static std::shared_ptr<TdaqPacket> createBuiltEventPacket(double t, std::shared_ptr<EventInfo> eventInfo);
	static std::shared_ptr<TdaqPacket> createL2RequestPacket(double t, std::shared_ptr<FragmentRequestInfo> request);
	static std::shared_ptr<TdaqPacket> createFirstLevelTriggerRequestPacket(double t, std::shared_ptr<FragmentRequestInfo> requestInfo, int requestSize_bytes);
	static std::shared_ptr<TdaqPacket> createFirstLevelTriggerResponsePacket(double t, int fragmentSize, std::shared_ptr<FragmentInfo> requestedInfo,  std::shared_ptr<TdaqPacket> requestPacket);
	static std::shared_ptr<TdaqPacket> createEventBuildRequestPacket(double t, int eventId, int puIds,  std::vector<FragmentInfo> fragments, double eventBirthTime, double procTime);
	static std::shared_ptr<TdaqPacket> createAckPacket(double t, int sequenceNumber, double packetToAckBirthTime, int destinationMachineId, int destinationApplicationId, int sourceMachineId, int sourceApplicationId);
	static std::shared_ptr<TdaqPacket> createPacket(double t, char* flowID);

	static std::shared_ptr<protocol> createTcpProtocol(double t, int destinationApplicationId, int sourceApplicationId);
	static std::shared_ptr<protocol> createIpProtocol(double t, int destinationMachineId, int sourceMachineId);

private:
	static int nextPacketId;
	static int getNextPacketID() { return nextPacketId++; }

};

#endif

