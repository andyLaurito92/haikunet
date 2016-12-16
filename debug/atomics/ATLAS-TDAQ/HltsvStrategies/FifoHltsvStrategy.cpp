#include "FifoHltsvStrategy.h"

FifoHltsvStrategy::FifoHltsvStrategy() : IHltsvStrategy() {
	printLog(LOG_LEVEL_IMPORTANT, "Using Strategy: FifoHltsvStrategy\n");
}

void FifoHltsvStrategy::addPuToDcm(int puId, int dcmId, int rackId){
	this->idlePUIds.push_back(puId); // mark idle PU
}

void FifoHltsvStrategy::init(){
}

void FifoHltsvStrategy::setIdlePu(int puId, DcmMeasuredStatistics ){
	this->idlePUIds.push_back(puId); // mark idle PU
}

int FifoHltsvStrategy::setNextPuBusy(double) {
	int puId = this->idlePUIds.front();
	idlePUIds.pop_front(); // remove it from idles

	return puId;
}

int FifoHltsvStrategy::getIdleCores(){
	return this->idlePUIds.size();
}

