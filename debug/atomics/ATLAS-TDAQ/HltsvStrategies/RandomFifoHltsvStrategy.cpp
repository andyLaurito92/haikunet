#include "RandomFifoHltsvStrategy.h"

RandomFifoHltsvStrategy::RandomFifoHltsvStrategy() : IHltsvStrategy() {
	printLog(LOG_LEVEL_IMPORTANT, "Using Strategy: RandomFifoHltsvStrategy\n");
}

void RandomFifoHltsvStrategy::addPuToDcm(int puId, int dcmId, int rackId){
	this->idlePUIds.push_back(puId); // mark idle PU
}

void RandomFifoHltsvStrategy::init(){
	std::random_shuffle(idlePUIds.begin(), idlePUIds.end()); // randomize idle PUs
}

void RandomFifoHltsvStrategy::setIdlePu(int puId, DcmMeasuredStatistics ){
	this->idlePUIds.push_back(puId); // mark idle PU
}

int RandomFifoHltsvStrategy::setNextPuBusy(double) {
	int puId = this->idlePUIds.front();
	idlePUIds.pop_front(); // remove it from idles

	return puId;
}

int RandomFifoHltsvStrategy::getIdleCores(){
	return this->idlePUIds.size();
}

