#ifndef FLOW_H_
#define FLOW_H_

#include <stdlib.h>
#include <string>

#include "Route.h"

#include "sinks/ParameterReader.h"
#include "hybrid/DistributionParameters/IDistributionParameter.h"


/**
 *  Flows (or classes in the the MVA parlance) define the communication between different nodes.
 *
 */
class Flow {
private:
	std::string flowId;
	double startTime;
	int typeOfService; // to use in QoS and priority queues (see PacketPrioritySelect.h)
	std::shared_ptr<IDistributionParameter> period; // generation rate
	std::shared_ptr<IDistributionParameter> size;   // generation size
	// TODO: implement --> finishTime, Priority

public:
	Flow(std::string flowId, double startTime, int typeOfService, std::shared_ptr<IDistributionParameter> period, std::shared_ptr<IDistributionParameter> size) :
		flowId(flowId), startTime(startTime), typeOfService(typeOfService), period(period), size(size)  {
	}

	~Flow() {	}


	std::string getFlowId() { return this->flowId;}
	double getStartTime(){ return this->startTime; }
	std::shared_ptr<IDistributionParameter> getPeriod(){ return this->period; }
	std::shared_ptr<IDistributionParameter> getSize(){ return this->size; }
	int getTypeOfService(){ return this->typeOfService; }/** Returns the type of service for the flow. Used in QoS and priority queues (see PacketPrioritySelect.h) **/

};


#endif
