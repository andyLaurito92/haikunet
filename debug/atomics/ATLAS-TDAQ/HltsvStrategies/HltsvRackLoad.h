#ifndef ATOMICS_ATLAS_TDAQ_HltsvStrategies_HLTSVRACKLOAD_H_
#define ATOMICS_ATLAS_TDAQ_HltsvStrategies_HLTSVRACKLOAD_H_

#include "simulator.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "ATLAS-TDAQ/globals.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <list>

class HltsvRackLoad {
		int rackId;
		int busyPUs;

		public:
			HltsvRackLoad(int myId)
			{
				rackId = myId;
				busyPUs = 0;
			}

			int getRackId() { return this->rackId;};
			void addBusyPU(){ busyPUs++; }
			void removeBusyPU(){ busyPUs--; }
			int countBusyPUs() { return busyPUs;};

			bool operator==(const HltsvRackLoad& other) const { return other.rackId == rackId;};
	};

#endif /* ATOMICS_ATLAS_TDAQ_HltsvStrategies_HLTSVRACKLOAD_H_ */
