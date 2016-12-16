//CPP:ATLAS-TDAQ/NetworkConfiguration.cpp
#if !defined NetworkConfiguration_h
#define NetworkConfiguration_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"

#include <vector>
#include <map> 
#include <unordered_set> 
#include <array>
#include <algorithm>    // std::binary_search, std::sort

#include "hybrid/stdevstool.h"
#include "sinks/ParameterReader.h"

#include "ATLAS-TDAQ/L1TConfiguration.h"


/***
 * This model exposes the configuration of ROBIds, ROSIds and their mapping using static methods (only one instance should exist)
 * Only the init function has behavior to read parameter and initialize the Ids.
 * If a model need to use these Ids in their init function, they need to have less priority than this model.
 */
class NetworkConfiguration: public Simulator {
private:
	// parameters
	static int nDCM;
	static int tpuPerRack;
	static int totalCoreSwitches;
        static std::map<int, int> puToDCM;
        static std::vector<double> acumulatedWeights;
        static double totalCumulatedWeight;
        static std::vector<double> weights;

	// Constants set at init

public:
	NetworkConfiguration(const char *n) : Simulator(n) { };
	~NetworkConfiguration() { };
	virtual void init(double, ...);
	virtual double ta(double t);
	virtual void dint(double);
	virtual void dext(Event, double);
	virtual Event lambda(double);
	virtual void exit();

        static void loadPuToDCMMap(const std::vector<double> pusPerDCM);
        static void loadWeightedRequestsStructures(const std::vector<double>, int);
        static std::vector<int> getWeightedRoSesSubset(const int length);
        static double modifyCumsumAvoidIndex(const int avoidIndex,
                std::vector<double> cumsum, std::vector<double> weights, std::vector<int> ros);

	static const int getTpuPerRack() { return NetworkConfiguration::tpuPerRack; }

	static const int getTotalCoreSwitches() { return NetworkConfiguration::totalCoreSwitches; }

	// Given the dcmId it returns in which Rack it is located
	static const int getTpuRackIndex(int dcmId) {
		// TODO: update this when each rack has different amount of DCMs. Use a map or similar structure.
		return dcmId / NetworkConfiguration::getTpuPerRack();
	}

	// Given the puId it returns in which DCM it is located
        static const int getDcmIndex(int puId) {
            // We look for the dcm that the PU was assigned to
            auto search = NetworkConfiguration::puToDCM.find(puId); 
            auto ret = search -> second;
            printLog(LOG_LEVEL_PRIORITY, "getDCMIndex[%i] = %i  \n", puId, (int) ret);
            return ret;
        }

	// Given the core number and the TPU, it returns the queueIndex where packets are buffered
	// This would represent the non-existent queue at the output port of the core, which is formed by the Brocade algorithm that chooses a packet from each VoQ related to the outport. Each outport is connected to a TPU-ToR
	static const int getCoreQueueIndex(int coreIndex, int tpuRackIndex) {
		return coreIndex + (tpuRackIndex * NetworkConfiguration::getTotalCoreSwitches());
	}


        static bool areWeightedRoSRequests;
};
#endif
