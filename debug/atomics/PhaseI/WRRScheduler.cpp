#include "WRRScheduler.h"

void WRRScheduler::init(double t, ...) {
	BaseSimulator::init(t);

	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//  %Name% is the parameter name
	//	%Type% is the parameter type

	char *fvar;

	fvar = va_arg(parameters, char*);
	int nQueues = readDefaultParameterValue<long>(fvar);
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: nQueues: %u  \n", t, this->getFullName().data(), nQueues);

	fvar = va_arg(parameters, char*);
	this->queueWeights = readDefaultParameterValue<std::vector<double> >(fvar);
	if(this->queueWeights.size() != nQueues){ // validate weights matches queues
		debugMsg(LOG_LEVEL_ERROR, "[%f] %s_init: Wrong configuration. Number weights does not match number of queues. There were %u queues configured and %u weights \n", t, this->getFullName().data(), nQueues, this->queueWeights.size());
		throw std::runtime_error("WRRScheduler::dext_init: Wrong configuration. Number weights does not match number of queues. Check log for details. \n");
	}
	debugMsg(LOG_LEVEL_INIT, "[%f] %s: weights: %u  \n", t, this->getFullName().data(), this->queueWeights.size());


	// initialize state variables
	this->queueSizes_bytes = std::vector<int>(nQueues, 0); // all sizes to 0;
	this->WRRQueueIndex = 0; // does not really matter the starting index, as it will start with the queue receiving the first packet
	this->pendingWeightForCurrentIndex = this->queueWeights[this->WRRQueueIndex];
	this->delayedRequestPending = false;

	this->sigma = std::numeric_limits<double>::infinity();

	this->logger->initSignals(std::map<std::string,int>{
		{"sum_queue_sizes_bytes", SCILAB_LOG_LEVEL_ALWAYS } // sum of the sizes of all queues
	});

	return;
}

void WRRScheduler::dint(double t) {
	// a request was just sent, so wait for the next request
	this->sigma = std::numeric_limits<double>::infinity();

}
void WRRScheduler::dext(Event x, double t) {
	debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: event arrived to port %u \n", t, this->getFullName().data(), x.port);

	switch (x.port) {
	case 0: {
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: new queue size arrived \n", t, this->getFullName().data());

		this->dext_queueSize(x, t);

		break;
	} case 1: {
		debugMsg(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: new dequeue request arrived \n", t, this->getFullName().data());

		// validate
		if(this->sigma == 0 || this->delayedRequestPending) {
			debugMsg(LOG_LEVEL_ERROR, "[%f] %s_ext: a dequeue request arrived before the previous one has been sent \n", t, this->getFullName().data());
			throw std::runtime_error("WRRScheduler::dext_dequeRequest: a dequeue request arrived before the previous one has been sent \n");
		}

		this->dext_dequeRequest();

		break;
	} default: {
		throw std::runtime_error("WRRScheduler::dext invalid input port " + std::to_string(x.port));
		break;
	}
	}

	return;
}

Event WRRScheduler::lambda(double t) {
	// a request arrived, so the request should be sent
	// send a vectorialEvent with index according to the WRR algorithm
	auto vec = std::make_shared<VectorialEvent>(&this->WRRQueueIndex , this->WRRQueueIndex);

	debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_lambda: sending dequeue request to queue %u \n", t, this->getFullName().data(), vec->index);

	return Event(vec, 0);
}


void WRRScheduler::dext_queueSize(Event x, double t){
	// get the values from the Vectorial Event
	auto vec = std::static_pointer_cast<VectorialEvent> (x.valuePtr);
	double* value = (std::static_pointer_cast<double> (vec->valuePtr)).get();

	int index = vec->index;
	double newQueueSize =  value[0];
	//	double discard =  value[1];

	// validate
	if(this->queueSizes_bytes.size() <= index) {
		debugMsg(LOG_LEVEL_ERROR, "[%f] %s_dext_queueSize: queue size arrived for queue %u, but only %u queues configured \n", t, this->getFullName().data(), index, this->queueSizes_bytes.size());
		throw std::runtime_error("WRRScheduler::dext_queueSize: queue size arrived for a queue with index bigger than configured. Check log for details. \n");
	}

	// store new queue size
	this->sumQueueSizes += newQueueSize - this->queueSizes_bytes[index];
	this->queueSizes_bytes[index] = newQueueSize;

	// log value
	this->logger->logSignal(t, this->sumQueueSizes, "sum_queue_sizes_bytes");


	debugMsg(LOG_LEVEL_PRIORITY, "[%f] %s_dext_queueSize: new queue size arrived for queue %u and stored: %u \n", t, this->getFullName().data(), index, this->queueSizes_bytes[index]);

	// set sigma
	if(this->delayedRequestPending && newQueueSize != 0){ // if a request was delayed and there are packets in the queue, send the request that was delayed
		this->dext_dequeRequest(); // calculate indexes as usual
	} else{
		this->sigma = this->sigma - this->e; // continue as before
	}
}

void WRRScheduler::dext_dequeRequest(){
	// find next queue
	this->WRRQueueIndex = this->getNextWRRIndex();
	if(this->WRRQueueIndex >= 0){
		this->sigma = 0; // send dequeue request immediately
		this->delayedRequestPending = false;

		// decrease the pending weight
		if(this->pendingWeightForCurrentIndex == 0){ // needs resetting first
			this->pendingWeightForCurrentIndex = this->queueWeights[this->WRRQueueIndex];
		}

		this->pendingWeightForCurrentIndex--;

	} else { // all queues are empty, can not send request now. Set the delayRequest flag and wait
		debugMsg(LOG_LEVEL_PRIORITY, "[??????] %s_ext: WRRScheduler::getNextQueueIndex: all queues empty. Wait for next queue to have packets \n",  this->getFullName().data());

		// reset index and weight
		this->WRRQueueIndex = 0;
		this->pendingWeightForCurrentIndex = this->queueWeights[this->WRRQueueIndex];
		this->sigma = std::numeric_limits<double>::infinity(); // wait for next queue
		this->delayedRequestPending = true;
	}
}

int WRRScheduler::getNextWRRIndex(){ // this method implements the Round Robin algorithm, skipping 0-size queues
	if(this->pendingWeightForCurrentIndex == 0) { // if current index was served enough times
		this->WRRQueueIndex++; // start with the following index
	}

    // search for the first non empty queue in the range [WRRQueueIndex+1, end)
	auto it = std::find_if (this->queueSizes_bytes.begin() + this->WRRQueueIndex, this->queueSizes_bytes.end(), [](int queueSize){ return queueSize != 0; } );
	if(it == this->queueSizes_bytes.end()){ // search in the range [begin, WRRQueueIndex]
		it = std::find_if (this->queueSizes_bytes.begin(), this->queueSizes_bytes.begin() + WRRQueueIndex, [](int queueSize){ return queueSize != 0; } );

		if(it == this->queueSizes_bytes.begin() + WRRQueueIndex){ // all queues empty
			return -1;
		}
	}

	return it - this->queueSizes_bytes.begin(); // return position
}

