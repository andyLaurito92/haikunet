#include "L2TIterator.h"

void L2TIterator::init(double t, ...) {
	BaseSimulator::init(t);
    //The 'parameters' variable contains the parameters transferred from the editor.
    va_list parameters;
    va_start(parameters, t);
    //To get a parameter: %Name% = va_arg(parameters,%Type%)
    //where:
    //      %Name% is the parameter name
    //	%Type% is the parameter type
    printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_init: Initializing \n", t, this->getName());

    char* fvar;

    printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_init: initializing model.\n", t, this->getName());
    fvar = va_arg(parameters,char*);
    meanSteps = (double) getScilabVar(fvar, true);

    fvar = va_arg(parameters,char*);
    stochasticSteps = (int) getScilabVar(fvar, true); 

    fvar = va_arg(parameters,char*);
    forwardingPercentageThreshold = (float) getScilabVar(fvar, true); 

    // printLog(LOG_LEVEL_IMPORTANT, "[%f] %s_init: initializing model. Stochastic steps retrieve %d\n", t, this->getName(), stochasticSteps);
    printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_init: meanSteps =  %f \n", t, this->getName(), meanSteps);

    srand (time(NULL));

    sigma = INF;
    arrived = 0; accepted = 0; rejected = 0;

    steps = 0;

    this->logger->initSignals(std::vector<std::string>{"steps" , "event"});

}

void L2TIterator::dint(double t) {

}

void L2TIterator::dext(Event x, double t) {
    //The input event is in the 'x' variable.
    //where:
    //     'x.value' is the value (pointer to void)
    //     'x.port' is the port number
    //     'e' is the time elapsed since last transition

    // Iterative steps used
    printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_dext: %d steps remaining.\n", t, this->getName(), steps);
    auto arrivedPacket = std::static_pointer_cast<TdaqPacket>(x.valuePtr);

    if(steps < 1 || this -> fragmentsThresholdCalculate()){
        printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_dext: 0 steps remaining, restarting counter.\n", t, this->getName());

        // First run of the model
        steps = this -> getNextStepsAmount(); 
        std::shared_ptr<TdaqPacket> packetBeingProcessed = std::static_pointer_cast<TdaqPacket> (x.valuePtr);
        int event = packetBeingProcessed -> getEventInfo() -> eventId;
        this->logger->logSignal(t, (double)steps, "steps");
        this->logger->logSignal(t, (double)event, "event");
        printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_dext: assigned %d steps.\n", t, this->getName(), steps);
        // This cleans the arrivedSignals queue
        std::queue<std::shared_ptr<void>> empty;
        std::swap(arrivedSignals, empty); 

        eventInfo = std::static_pointer_cast<EventInfo>(arrivedPacket->payload);
        newEvent = false;
        printLog(LOG_LEVEL_PRIORITY, "[%f] %s_dext: new event.\n", t, this->getName());
    }
    else{
        auto updatedEventInfo = std::static_pointer_cast<EventInfo>(arrivedPacket->payload);
        eventInfo -> fragmentsInfo.insert(eventInfo -> fragmentsInfo.end(),
                updatedEventInfo -> fragmentsInfo.begin(), updatedEventInfo ->
                fragmentsInfo.end());
        eventInfo -> l2ProcTime += updatedEventInfo -> l2ProcTime;
        printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_dext: updating event info.\n", t, this->getName());

    }
    --steps;

    // buffer signal
    arrivedSignals.push(x.valuePtr);

    // Execute the internal transition now
    sigma = 0;
}

Event L2TIterator::lambda(double t) {
    //This function returns an Event:
    //     Event(%&Value%, %NroPort%)
    //where:
    //     %&Value% points to the variable which contains the value.
    //     %NroPort% is the port number (from 0 to n-1)

    printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lambda: Returning signals.\n", t, this->getName());
    sigma = INF;
    // If steps is >0 then forward event to L2TRequester
    // Go to L2TAcceptance otherwise.
    int outport = steps != 0? 0: 1;
    if(steps < 1 || this -> fragmentsThresholdCalculate() ){
        outport = 1; 
        auto packetEvent = TdaqPacket::createEventBuildRequestPacket(t,
                eventInfo -> eventId, eventInfo -> puId, eventInfo -> fragmentsInfo,
                eventInfo -> birthTime, eventInfo -> l2ProcTime);
        printLog(LOG_LEVEL_FULL_LOGGING, "[%f] %s_lambda: Returning signal to Acceptance with requested fragments [%d].\n",
                t, this->getName(), eventInfo -> fragmentsInfo.size());
        return Event(packetEvent, outport); 
    }else {
        outport = 0; 
    }

    auto arrivedSignal = arrivedSignals.front();
    arrivedSignals.pop();
    return Event(arrivedSignal, outport);

}

int L2TIterator::getNextStepsAmount(){

    int steps = STDEVS::getConfigurationInstance()->poisson(meanSteps - 1) + 1;
    return stochasticSteps? steps: meanSteps;
}
bool L2TIterator::fragmentsThresholdCalculate(){
    int fragmentsRequested = eventInfo -> fragmentsInfo.size();
    int totalEventFragments = L1TConfiguration::getFragmentsPerEvent();

    return (fragmentsRequested >= (totalEventFragments * this -> forwardingPercentageThreshold));

}
