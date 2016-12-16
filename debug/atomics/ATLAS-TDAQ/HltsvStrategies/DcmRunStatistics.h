#ifndef ATOMICS_ATLAS_TDAQ_HltsvStrategies_DcmRunStatistics_H_
#define ATOMICS_ATLAS_TDAQ_HltsvStrategies_DcmRunStatistics_H_

#include "simulator.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "ATLAS-TDAQ/globals.h"

#include <algorithm>
#include <vector>
#include <queue>
#include <list>

class DcmRunStatistics {
	double accumulatedLatencies;
	double accumulatedLatenciesPerByte;
	double accumulatedLoad;
    int latencyMeassurementsCount;
	int loadMeassurementsCount;
	double latencyLowPassFilter;

    public:
        DcmRunStatistics() :
            accumulatedLatencies(1),
            accumulatedLatenciesPerByte(1),
            accumulatedLoad(1),
            latencyMeassurementsCount(0),
            loadMeassurementsCount(0),
            latencyLowPassFilter(1)
    { } ;

	double getAccumulatedLatencies() { return this->accumulatedLatencies;};
	int getLatenciesCount() { return this->loadMeassurementsCount;};

        void addLoadMeasurement(double loadMeasurement){
            if(this->loadMeassurementsCount == 0) this->accumulatedLoad = 0;
            this -> accumulatedLoad += loadMeasurement;
            this -> loadMeassurementsCount++; 
        };
        void addLatencyPerByte(double latencyPerByte){
            if(this->latencyMeassurementsCount == 0){
                this->accumulatedLatenciesPerByte = 0;
                this->latencyLowPassFilter = 0;
            }

            this->accumulatedLatenciesPerByte  += latencyPerByte * 1000; // To ms conversion
            this->latencyMeassurementsCount++;
        }
        void addLatencyMeasurement(double latencyMeasurement) {
            double lowPassFilterAlpha=0.125;
            if(this->latencyMeassurementsCount == 0){
                this->accumulatedLatencies = 0;
                this->latencyLowPassFilter = 0;
            }

            this->accumulatedLatencies += latencyMeasurement * 1000; // To ms conversion
            this->latencyMeassurementsCount++;

            // low pass filter
            this->latencyLowPassFilter = (lowPassFilterAlpha * latencyMeasurement) + ((1-lowPassFilterAlpha) * this->latencyLowPassFilter);
        };
        double getAverageLoad() {
            if(this->loadMeassurementsCount == 0){
                return this->accumulatedLoad; // returns 1
            }
            return this -> accumulatedLoad / this -> loadMeassurementsCount;
        }
        double getAverageLatencyPerByte() {
            if(this->latencyMeassurementsCount == 0){
                return this->accumulatedLatenciesPerByte; // returns 1
            }
            return this->accumulatedLatenciesPerByte / this->latencyMeassurementsCount;
        };

        double getAverageLatency() {
            if(this->latencyMeassurementsCount == 0){
                return this->accumulatedLatencies; // returns 1
            }
            return this->accumulatedLatencies / this->latencyMeassurementsCount;
        };

	double getLatencyLowPassFilter(){
		return this->latencyLowPassFilter;
	}

	double getPower(){
		return this->getAverageLoad() /  pow(this->getAverageLatency(), 2);
	}
};

#endif /* ATOMICS_ATLAS_TDAQ_HltsvStrategies_DcmRunStatistics_H_ */
