//CPP:ATLAS-TDAQ/HltsvStrategies/HltsvDcmLoad.cpp

#ifndef ATOMICS_ATLAS_TDAQ_HltsvStrategies_HLTSVDCMLOAD_H_
#define ATOMICS_ATLAS_TDAQ_HltsvStrategies_HLTSVDCMLOAD_H_

#include "simulator.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "sinks/ParameterReader.h"
#include "ATLAS-TDAQ/globals.h"
#include "ATLAS-TDAQ/HltsvStrategies/HltsvRackLoad.h"
#include "ATLAS-TDAQ/HltsvStrategies/DcmMeasuredStatistics.h"
#include "ATLAS-TDAQ/HltsvStrategies/DcmRunStatistics.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <list>

class HltsvDcmLoad {
		int dcmId;
		std::shared_ptr<HltsvRackLoad> rack;
		std::deque<int> idlePUIds;
		int totalPUs = 0;
		double lastAssigmentTime;
		DcmRunStatistics stats;

	public:
		HltsvDcmLoad(int myId, std::shared_ptr<HltsvRackLoad> myRack){
			dcmId = myId;
			rack = myRack;
//			rack->addIdleDcm(std::shared_ptr<DcmLoad>(this));
		}

		bool operator==(const HltsvDcmLoad& other) const { return other.dcmId == this->dcmId;};
		bool operator!=(const HltsvDcmLoad &other) const { return !(*this == other); }
//bool operator<(const DcmLoad& other) const { return this->countBusyPus() < other.countBusyPus();};
//		bool operator<(const int other) const { return this->countBusyPus() < other;};

		int getDcmId() { return this->dcmId;};
		std::shared_ptr<HltsvRackLoad> getRack() const { return this->rack;};
		double getLastAssigmentTime() const { return this->lastAssigmentTime;};
		DcmRunStatistics getStatistics() const { return this->stats;};

		int countIdlePus () const { return this->idlePUIds.size(); }; // Complexity constant for dequeue.size
		int countBusyPus () const { return totalPUs - countIdlePus(); }; // Complexity constant for countIdlePus
		void addPU(int puId){ this->idlePUIds.push_back(puId); totalPUs++;};
		int getNextPuId(){
                    if (this->idlePUIds.size() == 0) 
                        throw "Attempting to return puId from empty idlePUIds list";
                    return this->idlePUIds.front();
                }; // Complexity constant for dequeue.front()

		void markPUAsIdle(int puId, DcmMeasuredStatistics stats);

		void markPUAsBusy(int puId){	markPUAsBusy(puId, -1); };// Assignment time not taken into account
		void markPUAsBusy(int puId, double t);
};

#endif /* ATOMICS_ATLAS_TDAQ_HELPERS_HLTSVDCMLOAD_H_ */
