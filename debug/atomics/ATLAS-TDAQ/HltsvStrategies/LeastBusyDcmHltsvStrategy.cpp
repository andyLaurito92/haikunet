#include "LeastBusyDcmHltsvStrategy.h"

LeastBusyDcmHltsvStrategy::LeastBusyDcmHltsvStrategy() :
IHltsvStrategy(), totalIdlePus(0) {
	printLog(LOG_LEVEL_IMPORTANT, "Using Strategy: LeastBusyDcmHltsvStrategy\n");
}

void LeastBusyDcmHltsvStrategy::addPuToDcm(int puId, int dcmId, int rackId){
	auto existingDcmIt = this->findDcm(dcmId);

	std::shared_ptr<HltsvDcmLoad> dcmLoad;
	if(existingDcmIt == dcms.end()){ // does not exist
		auto rack = this->findRackOrCreate(rackId);

		// create a new HltsvDcmLoad
		dcmLoad = std::make_shared<HltsvDcmLoad>(dcmId, rack);

		// add it to the list
		dcms.push_back(dcmLoad);
	}else {
		dcmLoad = *existingDcmIt;
	}

	// add the PU
	dcmLoad->addPU(puId);
	this->totalIdlePus++;

	// sort
	//std::push_heap (dcms.begin(),dcms.end()); // sorting is not necessary as they are all added with no load
}

void LeastBusyDcmHltsvStrategy::init(){
}

void LeastBusyDcmHltsvStrategy::setIdlePu(int puId, DcmMeasuredStatistics stat){
	int dcmId = NetworkConfiguration::getDcmIndex(puId);
	auto dcmIt = this->findDcm(dcmId);

	// validation
	if(dcmIt == dcms.end()){
		printLog(LOG_LEVEL_ERROR, "removeBusyPuFromDcm: DCM %i (with PU #%i) was no previously in the system, PU can not be set idle : \n",  dcmId, puId);
		for(auto dcm : dcms){
			printLog(LOG_LEVEL_ERROR, "DCM id=%i \n", dcm->getDcmId());
		}
		throw "Invalid Operation: removeBusyPuFromDcm";
	}

	auto dcm = *dcmIt;

	// add busy PU. This breaks sorting so it must be thread safe (which its not)
	dcm->markPUAsIdle(puId, stat);
	this->totalIdlePus++;
	printLog(LOG_LEVEL_DEBUG, "Removing busy PU. DCM %i now has %i busy PUs,  \n", dcm->getDcmId(), dcm->countBusyPus());

	// TODO: it would be better to search from dcmIt-1 to begin. i.e: in reverse order
	auto rfirstSmallerIt = std::find_if(dcms.begin(), dcmIt, [&](std::shared_ptr<HltsvDcmLoad> const& p) {
		return LessBusyDcm(dcm, p);
	});

	dcms.splice(rfirstSmallerIt, dcms, dcmIt); // move it to the new position

	printLoad(LOG_LEVEL_DEBUG);
}

int LeastBusyDcmHltsvStrategy::setNextPuBusy(double) {
	std::list<std::shared_ptr<HltsvDcmLoad>>::iterator dcmIt =  this->dcms.begin();
	auto dcm = *dcmIt;

	printLog(LOG_LEVEL_DEBUG, "setNextPuBusy: found minimal DCM. dcmId:%i (busy PUs:%i) at rack %i (with %i busy PUs)  \n", dcm->getDcmId(), dcm->countBusyPus(), dcm->getRack()->getRackId(),dcm->getRack()->countBusyPUs());

	// add busy PU. This breaks sorting so it must be thread safe (which its not)
	int nextPuId = dcm->getNextPuId();
	dcm->markPUAsBusy(nextPuId);
	this->totalIdlePus--;
	printLog(LOG_LEVEL_DEBUG, "setNextPuBusy:Adding busy PU. DCM %i now has %i busy PUs,  \n", dcm->getDcmId(), dcm->countBusyPus());

	// find position where to move it to maintain sorting: last of all the ones that are now equal to this element. It will be moved back in the list, just before the first item with than is bigger
	auto firstBiggerIt = std::find_if(std::next(dcmIt), dcms.end(), [&](std::shared_ptr<HltsvDcmLoad> const& p) {
		return LessBusyDcm(dcm, p);
	});

	dcms.splice(firstBiggerIt, dcms, dcmIt); // move it to the new position


	printLoad(LOG_LEVEL_DEBUG);

	return nextPuId;
}

int LeastBusyDcmHltsvStrategy::getIdleCores(){
	return this->totalIdlePus;
}


void LeastBusyDcmHltsvStrategy::printLoad(int logLevel){
	printLog(logLevel, "System Load: \n");
	printLog(logLevel, "DCM Ids :[");
	for(auto dcm : dcms){
		printLog(logLevel, "%i (%i), ", dcm->getDcmId(), dcm->getRack()->getRackId());
	}
	printLog(logLevel, "] \nDCM Load:[");

	for(auto dcm : dcms){
		printLog(logLevel, "%i (%i), ", dcm->countBusyPus(), dcm->getRack()->countBusyPUs());
	}
	printLog(logLevel, "] \n");
}


