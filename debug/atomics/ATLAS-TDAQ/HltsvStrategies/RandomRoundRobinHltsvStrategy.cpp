#include "RandomRoundRobinHltsvStrategy.h"

RandomRoundRobinHltsvStrategy::RandomRoundRobinHltsvStrategy() :
IHltsvStrategy(), totalIdlePus(0), nextDcmIndex(0) {
	printLog(LOG_LEVEL_IMPORTANT, "Using Strategy: RandomRoundRobinHltsvStrategy\n");
}

void RandomRoundRobinHltsvStrategy::addPuToDcm(int puId, int dcmId, int rackId){
	auto existingDcmIt = this->findDcm(dcmId);

	std::shared_ptr<HltsvDcmLoad> dcmLoad;
	if(existingDcmIt == dcms.end()){ // does not exist
		auto rack = this->findRackOrCreate(rackId);

		// create a new HltsvDcmLoad
		dcmLoad = std::make_shared<HltsvDcmLoad>(dcmId, rack);

		// add it to the list
		this->dcms.push_back(dcmLoad);
	}else {
		dcmLoad = *existingDcmIt;
	}

	// add the PU
	dcmLoad->addPU(puId);
	this->totalIdlePus++;
}

void RandomRoundRobinHltsvStrategy::init(){
	std::random_shuffle(this->dcms.begin(), this->dcms.end()); // randomize idle DCMs
}

void RandomRoundRobinHltsvStrategy::setIdlePu(int puId, DcmMeasuredStatistics stats ){
	// TODO: do nothing as we call this class only from the MinLatencyStrategy.
}

int RandomRoundRobinHltsvStrategy::setNextPuBusy(double) {
	// get next DCM
	auto dcm = this->dcms[this->nextDcmIndex];

	// update the next index
	this->nextDcmIndex++;
	if(this->nextDcmIndex == this->dcms.size()) this->nextDcmIndex = 0; // restart round robin

	// get the PU id & set it busy
	int nextPuId = dcm->getNextPuId();
	dcm->markPUAsBusy(nextPuId);
	this->totalIdlePus--;

	return nextPuId;

}

int RandomRoundRobinHltsvStrategy::getIdleCores(){
	return this->totalIdlePus;
}
