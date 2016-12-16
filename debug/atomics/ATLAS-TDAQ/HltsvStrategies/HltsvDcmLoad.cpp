#include "HltsvDcmLoad.h"

void HltsvDcmLoad::markPUAsIdle(int puId, DcmMeasuredStatistics stats){
	this->idlePUIds.push_back(puId);
	rack->removeBusyPU();

	// validate to avoid division by 0
	if(stats.getRequestedFragmentsSize() == 0){
		printLog(LOG_LEVEL_ERROR, "[HltsvDcmLoad] getRequestedFragmentsSize==0, unable to use it as the latency measurement");
		throw std::runtime_error("[HltsvDcmLoad] getRequestedFragmentsSize==0, unable to use it as the latency measurement");
	}

	this->stats.addLatencyMeasurement(stats.getNetworkLatency());
	this->stats.addLatencyPerByte(stats.getNetworkLatency() / stats.getRequestedFragmentsSize());
	this->stats.addLoadMeasurement(this -> countBusyPus());
}

void HltsvDcmLoad::markPUAsBusy(int puId, double t){
	// validation, only the front PU can be mark busy
	if (idlePUIds.size() == 0 || puId != getNextPuId()) {
		printLog(LOG_LEVEL_ERROR, "HLTSV_markPUAsBusy: attempting to mark PU #%i as busy in DCM=%i. Idle PUs=%i . Next PU=%i has to be set busy  \n", puId, this->dcmId, this->idlePUIds.size(), this->getNextPuId());
		throw "Invalid operation: HLTSV_markPUAsBusy";
	}

	idlePUIds.pop_front();
	rack->addBusyPU();

	this->lastAssigmentTime = t;
};
