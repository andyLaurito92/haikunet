#include "NetworkConfiguration.h"

// Static references
int NetworkConfiguration::tpuPerRack;
int NetworkConfiguration::totalCoreSwitches;
int NetworkConfiguration::nDCM;
double NetworkConfiguration::totalCumulatedWeight = 0; 
bool NetworkConfiguration::areWeightedRoSRequests;

std::map<int, int> NetworkConfiguration::puToDCM;
std::vector<double> NetworkConfiguration::acumulatedWeights;
std::vector<double> NetworkConfiguration::weights;
std::vector<int> getWeightedRoSesSubset;
double modifyCumsumAvoidIndex;


void NetworkConfiguration::init(double t, ...) {
	va_list parameters;
	va_start(parameters,t);

	char* fvar;

	fvar = va_arg(parameters,char*);
	NetworkConfiguration::nDCM = readDefaultParameterValue<int>(fvar);

        // read the pus per dcm vector
	fvar = va_arg(parameters,char*); 
	auto pusPerDCM = readDefaultParameterValue<std::vector<double>>(fvar);
    NetworkConfiguration::loadPuToDCMMap(pusPerDCM);
	printLog(LOG_LEVEL_INIT, "[%f] %s_init: NetworkConfiguration::puPerDcm has %i values \n", t, this->getName(), pusPerDCM.size());

	fvar = va_arg(parameters,char*);
	NetworkConfiguration::tpuPerRack = readDefaultParameterValue<int>(fvar);
    printLog(LOG_LEVEL_INIT, "[%f] %s_init: NetworkConfiguration::tpuPerRack =  %i \n", t, this->getName(),
                NetworkConfiguration::tpuPerRack);

        // read the core switches amount
	fvar = va_arg(parameters,char*);
	NetworkConfiguration::totalCoreSwitches = readDefaultParameterValue<int>(fvar);
        printLog(LOG_LEVEL_INIT, "[%f] %s_init: NetworkConfiguration::totalCoreSwitches =  %i \n", t,
                this->getName(), NetworkConfiguration::totalCoreSwitches);

        // read the amount of RoSes
	fvar = va_arg(parameters,char*);
	int nROS =  readDefaultParameterValue<int>(fvar);
	
        // read the weighted RoSes information
	fvar = va_arg(parameters,char*);
	auto weightedRoSes = readDefaultParameterValue<std::vector<double>>(fvar);
        NetworkConfiguration::loadWeightedRequestsStructures(weightedRoSes, weightedRoSes.size());


	va_end(parameters);
	return;
}

void NetworkConfiguration::loadPuToDCMMap(const std::vector<double> pusPerDCM){
    // Load the puIds. This goes from 0 to nPU totaling the ammount of PUs
    int puId=0;
    for(int i = 0; i < NetworkConfiguration::nDCM; i++){
        for(int j = 0; j < (int) pusPerDCM[i]; j++){
            NetworkConfiguration::puToDCM.insert(std::pair<int, int>(puId, (int) i));
            puId++;
        }
    }
}
void NetworkConfiguration::loadWeightedRequestsStructures(const std::vector<double> weightedRoSes, const int length){
    // We load the Cumulated distribution array. This 
    // is for amortized performance.

    NetworkConfiguration::totalCumulatedWeight = 0; 
    for(int i = 0; i < length; i++){
        double actualWeight = weightedRoSes[i];
        NetworkConfiguration::totalCumulatedWeight += actualWeight; 
        NetworkConfiguration::acumulatedWeights.push_back(NetworkConfiguration::totalCumulatedWeight);
        NetworkConfiguration::weights.push_back(actualWeight);
    } 
    NetworkConfiguration::areWeightedRoSRequests = (NetworkConfiguration::totalCumulatedWeight > 0);
}

std::vector<int> NetworkConfiguration::getWeightedRoSesSubset(const int length){
    std::vector<int> output;
    std::vector<double> localCumSum(NetworkConfiguration::acumulatedWeights);
    std::vector<double> localWeights(NetworkConfiguration::weights);
    std::vector<int> localRoses(L1TConfiguration::getRosIds());

    int remaining = length;
    double totalCumWeight = NetworkConfiguration::totalCumulatedWeight;

    for(int i = 0; i < remaining; i ++){
        double rnd = STDEVS::getConfigurationInstance()->uniform() * totalCumWeight;
        auto elementIt = std::upper_bound(localCumSum.begin(),
                localCumSum.end(), rnd);
        int elementIndex = std::distance(localCumSum.begin(), elementIt);
        auto element = localRoses[elementIndex];
        output.push_back(element);

        // Recalculate cumsum
        totalCumWeight = NetworkConfiguration::modifyCumsumAvoidIndex(elementIndex, localCumSum,
                localWeights, localRoses);
    }
    return output;
}

double NetworkConfiguration::modifyCumsumAvoidIndex(const int avoidIndex,
        std::vector<double> cumsum, std::vector<double> weights, std::vector<int> ros){

    cumsum.resize(avoidIndex); // Drop the old cumsum values as need to recalculate
    weights.erase(weights.begin() + avoidIndex);
    ros.erase(ros.begin() + avoidIndex);

    double totalCumWeight = cumsum[avoidIndex - 1];  // cumsum avoid -1 haves the last partial sum
    for(int i = avoidIndex + 1; i < weights.size(); i++){
        totalCumWeight += weights[i];
        cumsum.push_back(totalCumWeight);
    }
    return totalCumulatedWeight;
}

double NetworkConfiguration::ta(double t) {
	return INF;
}

void NetworkConfiguration::dint(double t) {

}
void NetworkConfiguration::dext(Event x, double t) {

}
Event NetworkConfiguration::lambda(double t) {
	return Event();
}

void NetworkConfiguration::exit() {

}
