#include "RosApp.h"

void RosApp::init(double t, ...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//	%Type% is the parameter type
	stochasticSimulation = readDefaultParameterValue<int>(SCILAB_VARNAME_STOCHASTIC_SIMULATION);;
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: stochasticSimulation: %s \n", this->getName(), stochasticSimulation?"YES":"NO");

	char* fvar = va_arg(parameters,char*);
	processingTime = readDefaultParameterValue<double>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: processingTime: %f \n", t, this->getName(), processingTime);

	fvar = va_arg(parameters,char*);
	maxParallelJobs = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: maxParallelJobs: %i \n", t, this->getName(), maxParallelJobs);

	nextJobRequested = false;
	counter = 0;
	lastEmitionTime = 0;
	this->setNextSigma(t); // Immediately asks for a new job;

	this->logger->initSignals(std::vector<std::string>{"processingTime", "parallelJobs"});
}

void RosApp::dint(double t) {
	this->setNextSigma(t);
}

void RosApp::dext(Event x, double t) {
	// Request arrived
	auto requestPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: Received Packet ID #%u \n", t, this->getName(), requestPacket->ID);


	if(nextJobRequested == false) {
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: Model was not expecting a new job yet. Did you forget to put a queue before this model? \n",t, this->getName());
		sigma = sigma - e; // continue as before ツ
		throw "invalid model state";
		return; // NOTE: This Server does not allow to Preempt the previous task if a new packet arrives (unexpectedly) before the completion of the Processing processingTime
	}

	nextJobRequested = false; // requested job arrived.

	// Calculate the processing time for this request
	double jobProcessingTime = processingTime; // TODO: maybe calculate processing time according to #ROB and #ROS
	if(stochasticSimulation){
		jobProcessingTime = STDEVS::getConfigurationInstance()->exponential(processingTime);
	}

	// insert it in the queue of jobs being processed.
	auto job = std::make_shared<QueuedJob>(t+jobProcessingTime, requestPacket);
	this->requestsBeingProcessed.push(job);

	// calculate next sigma
	this->setNextSigma(t);

	this->logger->logSignal(t,jobProcessingTime,"processingTime");
	this->logger->logSignal(t,requestsBeingProcessed.size(),"parallelJobs");
	auto requestInfo = requestPacket->getFragmentRequestInfo();
	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: Processing Packet ID #%u(%s, eventId=%i) for %f seconds (will finish at %f). This requested ROS=%i for %i fragments. Total Requests being processed:%i \n", t, this->getName(), requestPacket->ID, requestPacket->FlowID, requestInfo->eventId, jobProcessingTime, t+jobProcessingTime, requestInfo->rosId, requestInfo->robIds.size(), requestsBeingProcessed.size());
}

Event RosApp::lambda(double t) {
	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb \n", t, this->getName());

	if(isNextJobNeeded()){ // request next job if needed
		nextJobRequested = true;
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: Asking for a new job. \n", t, this->getName());
		return Event(&counter, 1);
	}
	else{ // we finished processing a request, send response
		// get job and remove if from queue
		auto finishedJob = this->requestsBeingProcessed.top();
		this->requestsBeingProcessed.pop();

		printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lamb: Finished processing request (eventId=%i). Total Requests being processed:%i \n", t, this->getName(), finishedJob->getJobRequest()->getFragmentRequestInfo()->eventId, requestsBeingProcessed.size());

		// send response
		// TODO: now the RosApp sends again the request and the response is calculated in the Packetization. This is not conceptually correct as the response (requestInfo->toFragmentInfo()) should be calculated in the ROS
		return Event(finishedJob->getJobRequest(), 0);
	}
}

void RosApp::setNextSigma(double t){
	if(isNextJobNeeded())
		sigma = 0; // if we can process more jobs, request next job
	else if (!this->requestsBeingProcessed.empty())
		sigma = this->requestsBeingProcessed.top()->getFinishTime()- t; // if we are processing, send next jobs when ready
	else
		sigma = INF; // nothing to process and job request already sent, wait

	if (!this->requestsBeingProcessed.empty())
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] setNextSigma=%f: first request to finish will be #%i at %f \n", t, sigma, this->requestsBeingProcessed.top()->getJobRequest()->getFragmentRequestInfo()->eventId, this->requestsBeingProcessed.top()->getFinishTime());
	else
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] setNextSigma=%f: Empty queue, model will wait \n", t, sigma);
}


