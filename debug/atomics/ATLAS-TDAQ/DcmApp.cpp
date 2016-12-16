#include "DcmApp.h"
#include <unordered_map>
#include <unordered_set>

void DcmApp::init(double t, ...) {
	BaseSimulator::init(t);
	//The 'parameters' variable contains the parameters transferred from the editor.
	va_list parameters;
	va_start(parameters, t);
	//To get a parameter: %Name% = va_arg(parameters,%Type%)
	//where:
	//      %Name% is the parameter name
	//      %Type% is the parameter type

	char* fvar;

	// Starting amount of m_creditsAvailable
	fvar = va_arg(parameters, char*);
	m_creditsAvailable = readDefaultParameterValue<int>(fvar);//(int) getScilabVar(fvar, true);
	if (m_creditsAvailable < 0) m_creditsAvailable = std::numeric_limits<int>::max();
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: m_creditsAvailable =  %i \n", this->getName(), m_creditsAvailable);

	fvar = va_arg(parameters,char*);
	requestSize = readDefaultParameterValue<int>(fvar);//(int) getScilabVar(fvar, true);
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: requestSize =  %i \n", this->getName(),requestSize);

	sigma = INF;

	this->logger->initSignals({"creditsAvailable", "queuedRequests", "eventId", "credits"});


	std::vector<int> m_sources = L1TConfiguration::getRosIds();
	//// CODE extracted from RosDataCollecto.cxx::asyncConnect
	for (auto& source : m_sources) {
		m_sessions.push_back(std::make_shared<Session>(source)); //TODO: replace with emplace_back
		for (auto it = L1TConfiguration::getSourceToRobIds().lower_bound(source);
				it != L1TConfiguration::getSourceToRobIds().upper_bound(source); ++it) {
			m_robIdToSession.insert({it->second, m_sessions.back()});
		}
	}
	//////////

}

void DcmApp::dint(double t) {
	this->setSigma(t);

}

void DcmApp::dext(Event x, double t) {
	if (x.port == 0) { // packet arrived from PU
		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: PACKET ARRIVED. \n", t, this->getName());
		auto receivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);

		// Safely validations
		if(receivedPacket == NULL || strcmp (receivedPacket->FlowID,FLOW_ID_L2_REQUEST) != 0){
			printLog(LOG_LEVEL_ERROR,"[%f] %s_ext: WARNING!!! Received packet has wrong format. IGNORING...  \n",t, this->getName());
			receivedPacket->printPacketInfo(10);
			sigma = sigma - e; // continue as before
			return;
		}

		// Log assignment interval
		if(lastAsigmentTime != 0){
			double interval = t - lastAsigmentTime;
			this->logger->logSignal(t, interval, "asigmentInterval");
		}
		lastAsigmentTime = t;

		// get ROB ids from packet payload (safe cast due to FLOW_ID)
		auto requestInfo = std::static_pointer_cast<FragmentRequestInfo>(receivedPacket->payload);

		////////// Code extracted from RosDataCollector::asyncFetchRobs()
		std::unordered_map<std::shared_ptr<Session>, std::vector<int> > requests;
		for (auto robId : requestInfo->robIds) {
			auto it = m_robIdToSession.find(robId);
			// Check for non-existent ROB IDs
			if (it == m_robIdToSession.end()) {
				printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: ERROR!!!Could not fetch ROB with non-existent ID (ROB ID: %i) \n",t, this->getName(), robId);
				return;
			}
			// NOTE: DCM caching is not implemented in simulation
			//		    if (!event->block[robId]) { // The ROB wasn't already collected. Queue it for fetching.
			requests[it->second].push_back(robId);
			//		    }

			printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: received request for rosId=%i and ROB=%i  \n", t, this->getName(), it->second->m_rosId, robId);
		}

		printLog(LOG_LEVEL_PRIORITY,"[%f] %s_ext: Received request for %i ROSes and a total of %i fragments. creditsAvailable=%i queuedRequests=%i  \n",t, this->getName(), requests.size(), requestInfo->robIds.size(), m_creditsAvailable, getTotalWaitingTransactions());

		// NOTE: DCM caching is not implemented in simulation
		//		if (requests.empty()) {
		//		    // All ROBs were already collected. Notify the caller.
		//		    m_main.ioService().post(
		//		        std::bind(&Processor::onFetchRobs, m_processor, event, context));
		//		    return;
		//		  }

		// We have a legitimate request. Do it.
		//		m_transactions.emplace_back(/*event, context*/);
		//		auto& transaction = m_transactions.back();
		for (auto& request : requests) {
			//		    ++transaction.nPendingRosTransactions;
			//		    ++m_main.isInfo()->DcPendingRequests;
			//		    m_histograms->robsPerRequest->Fill(request.first->remoteName().c_str(), request.second.size(), 1);
			request.first->asyncFetch(requestInfo->eventId, request.second, requestInfo->puId, this->getScalarIndex());
		}
		distributeCredits();

		////////////////////
	}

	if (x.port == 1) { // Ticket arrived
		m_creditsAvailable++;
		printLog(LOG_LEVEL_PRIORITY,"[%f] %s_ext: TICKET ARRIVED. creditsAvailable=%i queuedRequests=%i \n",t, this->getName(), m_creditsAvailable, getTotalWaitingTransactions());
		//		applyCreditDelta();
		distributeCredits();

		auto receivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
		auto requestInfo = std::static_pointer_cast<FragmentRequestInfo>(receivedPacket->payload);
		this->logger->logSignal(t, m_creditsAvailable, "credits");
		this->logger->logSignal(t,requestInfo -> eventId,"eventId");

	}

	this->setSigma(t);

	this->logger->logSignal(t, this -> total_credits, "creditsAvailable");
	this->logger->logSignal(t, getTotalWaitingTransactions(), "queuedRequests");
}

Event DcmApp::lambda(double t) {
	// get next packet
	auto session = outQueue.front(); outQueue.pop();
	auto requestInfo = session->popNextTransaction();

	// Create output packet
	auto packetToSend = TdaqPacket::createFirstLevelTriggerRequestPacket(t, requestInfo, requestSize);

	// Logging
	auto info = requestInfo;//(FragmentRequestInfo*)packetToSend->payload;
	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_lamda: Packet #%i forwarded (requesting ROS=%i for %i fragments, eventId=%i). creditsAvailable=%i queuedRequests=%i  \n", t, this->getName(), packetToSend->ID, info->rosId, info->robIds.size(), info->eventId, m_creditsAvailable, getTotalWaitingTransactions());
	//	for (int robId : info->robIds) {
	//		printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lambda: selected rosId=%i for ROB=%i  \n", t, this->getName(), info->rosId, robId);
	//	}
	//	output->printPacketInfo(LOG_LEVEL_FULL_LOGGING);

	return Event(packetToSend, 0);
}

void DcmApp::distributeCredits() {
	////////// Code extracted from RosDataCollector::distributeCredits()
	printLog(LOG_LEVEL_FULL_LOGGING,"[??] distributeCredits: creditsAvailable=%i \n", m_creditsAvailable);

	std::vector<std::shared_ptr<Session>> selectedSessions;
	do {
		selectedSessions.clear();
		for (auto session : m_sessions) {
			if (session->minCreditsNeeded() != 0 && m_creditsAvailable >= session->minCreditsNeeded()) {
				selectedSessions.push_back(session);
				printLog(LOG_LEVEL_FULL_LOGGING,"[??] distributeCredits: Session for ROS %i pre-selected to be given credits  \n", session->m_rosId);
			}
		}

		std::random_shuffle(selectedSessions.begin(), selectedSessions.end());
		for (auto& session : selectedSessions) {
			auto creditsGiven = session->minCreditsNeeded();
			if (m_creditsAvailable >= creditsGiven) {
				session->giveCredits(creditsGiven);
				outQueue.push(session); // added line to program lambda function. In the original code, inside giveCredits() the doFetch is called. In the simulation doFetch needs to be done in lambda via the popNextTransaction
				// TODO: this might not be exactly the same, as adding all sessions to the queue and handling the transaction later might not be the same as handling them as soon as they are ready. Specially in the case when the session has more than 1 transaction pending to be handled
				m_creditsAvailable -= creditsGiven;

				printLog(LOG_LEVEL_FULL_LOGGING,"[??] distributeCredits: Session for ROS %i was given %i credits. creditsAvailable=%i \n", session->m_rosId,creditsGiven, m_creditsAvailable);
			}
		}
	} while (!selectedSessions.empty());
	//////////////
}


void DcmApp::setSigma(double t){
	if (!outQueue.empty()) { // there are still things to send
		sigma = 0; // transition immediately
		printLog(10,"[%f] %s: Programmed to SEND immediately. creditsAvailable=%i queuedRequests=%i outQueue=%i sigma=%f \n",t, this->getName(), m_creditsAvailable, getTotalWaitingTransactions(), outQueue.size(), sigma);
	} else {
		sigma = INF; // needs to wait for tickets or packets
		printLog(10, "[%f] %s: Programmed to WAIT. creditsAvailable=%i queuedRequests=%i  sigma=%f \n", t, this->getName(), m_creditsAvailable, getTotalWaitingTransactions(), sigma);
	}
}

int DcmApp::getTotalWaitingTransactions(){
	// TODO: if this function affects performance could be moved to a field to track size instead of iterating every time
	int waitingFragments = 0;
	for(auto session : m_sessions){
		waitingFragments += session->m_pendingTransactions.size();
	}

	return waitingFragments;
}
