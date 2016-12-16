#include "RandomHltsvStrategy.h"

RandomHltsvStrategy::RandomHltsvStrategy() : IHltsvStrategy() {
	printLog(LOG_LEVEL_IMPORTANT, "Using Strategy: RandomHltsvStrategy\n");
}

void RandomHltsvStrategy::addPuToDcm(int puId, int dcmId, int rackId){
	this->idlePUIds.push_back(puId); // mark idle PU
}

void RandomHltsvStrategy::init(){
	std::random_shuffle(idlePUIds.begin(), idlePUIds.end()); // randomize idle PUs
}

void RandomHltsvStrategy::setIdlePu(int puId, DcmMeasuredStatistics ){
	this->idlePUIds.push_back(puId); // mark idle PU
}

int RandomHltsvStrategy::setNextPuBusy(double) {
	std::random_shuffle(this->idlePUIds.begin(), this->idlePUIds.end()); // randomize idle PUs
	int puId = this->idlePUIds.front();
	idlePUIds.pop_front(); // remove it from idles

	return puId;
}

int RandomHltsvStrategy::getIdleCores(){
	return this->idlePUIds.size();
}

