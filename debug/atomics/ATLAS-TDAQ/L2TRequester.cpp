#include "L2TRequester.h"

void L2TRequester::init(double t, ...) {
	BaseSimulator::init(t);

	va_list parameters;
	va_start(parameters,t);

	
	stochasticSimulation = readDefaultParameterValue<int>(SCILAB_VARNAME_STOCHASTIC_SIMULATION);;
	printLog(LOG_LEVEL_INIT, "[INIT] %s_init: stochasticSimulation: %s \n", this->getName(), stochasticSimulation?"YES":"NO");

	char* fvar;

	fvar = va_arg(parameters,char*);
	l2Requests = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: l2Requests =  %i \n", t, this->getName(), l2Requests);

	fvar = va_arg(parameters,char*);
	requestsPerSource = readDefaultParameterValue<int>(fvar);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: requestsPerSource =  %i \n", t, this->getName(), requestsPerSource);

	if(L1TConfiguration::getTotalRos() * L1TConfiguration::getChannelsPerRos() < l2Requests || requestsPerSource > L1TConfiguration::getChannelsPerRos()){
		printLog(LOG_LEVEL_ERROR, "[INIT] %s_init: (WARNING!!!) The model is not correctly configured. It has too many requestsPerSource or l2Requests!. There are %i total fragments and %i channels per ROS configured in the ROSes \n", t, this->getName(), L1TConfiguration::getTotalRos() * L1TConfiguration::getChannelsPerRos(), L1TConfiguration::getChannelsPerRos());
		throw "Invalid configuration";
	}

	this->logger->initSignals(std::vector<std::string>{"eventId", "fragmentsRequested"});

	mystate = WAITING;
	totalSentRequests = 0;
	totalEventsRequested = 0;
	sigma = INF; // Waits for first signal to arrive

	va_end(parameters);
	return;
}

void L2TRequester::dint(double t) {
	// already sent all requests. Wait for next signal to restart
	totalEventsRequested++;
	mystate = WAITING;
	sigma = INF;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_int: sigma: %f \n", t, this->getName(), sigma);

	return;
}
void L2TRequester::dext(Event x, double t) {
	// Signal to request for new event arrived
	printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: A signal arrived  from port %d\n", t,this->getName(), x.port);
	auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);
        if(x.port == 0) {
            auto hltsvRequest = std::static_pointer_cast<HltsvRequest>(arrivedPacket->payload);
            this -> puId = hltsvRequest -> puId;
            this -> eventId = hltsvRequest->eventId;
            requestedRobIds.clear();
            printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: A signal to start requesting event %i arrived  \n", t,this->getName(), eventId);
        }
        else if (x.port == 1){
            printLog(LOG_LEVEL_PRIORITY, "[%f] %s_ext: A signal to re-request fragments from event %i arrived  \n", t,this->getName(), eventId);

        }

	if (mystate == REQUESTING) {
		printLog(LOG_LEVEL_ERROR, "[%f] %s_ext: WARNING!!! A signal to start requesting arrived while the model was still sending previous packets. CANCELING previous job and restarting with the new one  \n", t,this->getName());
		totalEventsRequested++;
		throw "Invalid model";
	}

	int nRobs = l2Requests; // Total fragments to request
	if(stochasticSimulation){
		double value = STDEVS::getConfigurationInstance()->uniform(); // [0, 1]
		nRobs = std::max(1, (int)(value * l2Requests * 2));
	}

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: nRobs=%i to be requested \n", t,this->getName(), nRobs);

	///// Algorithm extracted from DummyProcessor.cpp (DummyProcessor::l2DataCollection)
	std::vector<int> robIdsToRequest;
	std::vector<int> robSources; 
        if(NetworkConfiguration::areWeightedRoSRequests){
            robSources = NetworkConfiguration::getWeightedRoSesSubset(20);
        }else{
            std::srand(unsigned (std::time(0)));
            robSources = L1TConfiguration::getRosIds();
            std::random_shuffle(robSources.begin(), robSources.end()); // get random ROSes (robSource=ROS)
        }
        
	for (int robSource : robSources) {
	      std::vector<int> rosRobIds; // ROB IDs for this ROS
	      for (auto it = L1TConfiguration::getSourceToRobIds().lower_bound(robSource); it != L1TConfiguration::getSourceToRobIds().upper_bound(robSource); ++it) {
                  // If it has not been requested in the past
                  int robId = it->second;
                  int rosId = robSource;
                  if (!requestedRobIds.count(robId) > 0){ 
                      rosRobIds.push_back(robId);
                  }
	      }
              if(rosRobIds.empty()){
                  continue;
              }


	      // printLog(LOG_LEVEL_FULL_LOGGING, "[DEBUG] %s_ext: selected ROS=%i has %i ROBs  \n", this->getName(), robSource, rosRobIds.size());

	      int fragmentsForRos = requestsPerSource;
//	      if(stochasticSimulation)
//	      {
//	    	  double value = STDEVS::StDevsDefaultInstance->uniform(); // [0, 1]
//	    	  fragmentsForRos = (value * requestsPerSource) + 1; // TODO: will never request all fragments (Probability for the uniform to be 1 is 0);
//	    	  printLog(LOG_LEVEL_IMPORTANT, "[%f] %s_ext: fragmentsForRos=%i \n",t, this->getName(), fragmentsForRos);
//
//	    	  int nRosRobs = std::min(fragmentsForRos, nRobs - (int) robIdsToRequest.size());
//	    	  printLog(LOG_LEVEL_IMPORTANT, "[%f] %s_ext: min=%i  max=%i \n",t, this->getName(), nRosRobs, nRobs - (int) robIdsToRequest.size());
//	      }

	      int nRosRobs = std::min((int) rosRobIds.size(), std::min(fragmentsForRos, nRobs - (int) robIdsToRequest.size()));

	      auto shuffleEnd = random_shuffle_n(rosRobIds.begin(), rosRobIds.end(), nRosRobs);
	      robIdsToRequest.insert(robIdsToRequest.end(), rosRobIds.begin(), shuffleEnd);
              requestedRobIds.insert(robIdsToRequest.begin(), robIdsToRequest.end());
	      if (robIdsToRequest.size() == nRobs) {
	        break;
	      }
	}
    ///////////////////////////////////////////////
	pendingRequest = std::make_shared<FragmentRequestInfo>(eventId, -1 /*RosId*/, robIdsToRequest, puId, -1 /*DcmId*/); // Ros and Dcm Ids are set by the DCMApp

        

	// start requesting immediately
	mystate = REQUESTING;
	sigma = 0;

	printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_ext: will request a total of %i fragments  \n", t,this->getName(), robIdsToRequest.size());

}
Event L2TRequester::lambda(double t) {
    sigma = INF;
    auto packetToSend = TdaqPacket::createL2RequestPacket(t, pendingRequest);

    printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lamb: Sending request for %i fragments (eventId=%i) with the following packet : \n", t, this->getName(), pendingRequest->robIds.size(),pendingRequest->eventId );
    this->logger->logSignal(t, (int) pendingRequest->robIds.size(), "fragmentsRequested");
    this->logger->logSignal(t,(double) eventId, "eventId");


    return Event(packetToSend, 0);
}

// Fisher-Yates shuffle (copied from dcm/dcm/utilities.h)
template<typename ForwardIterator>
ForwardIterator L2TRequester::random_shuffle_n(ForwardIterator begin, ForwardIterator end, std::size_t num_random)
{
  size_t left = std::distance(begin, end);
  while (num_random--) {
    ForwardIterator r = begin;
    std::advance(r, std::rand() % left);
    using std::swap;
    swap(*begin, *r);
    ++begin;
    --left;
  }
  return begin;
}
