#include "stdevstool.h"
#include <chrono> // temporary inclusion

STDEVS* STDEVS::ReproducibleGenerationInstance = NULL; // new STDEVS(STDEVS_REPRODUCIBLE_SEED);
STDEVS* STDEVS::UniqueGenerationInstance =  NULL; // new STDEVS(STDEVS_UNIQUE_SEED_SEED);
STDEVS* STDEVS::ConfigurationInstance = NULL;    // new STDEVS(configuredSeed);

// Use this instance in models which need reproducible random numbers (uses always the same seed)
STDEVS* STDEVS::getReproducibleGenerationInstance(){
	if(STDEVS::ReproducibleGenerationInstance == NULL){
		STDEVS::ReproducibleGenerationInstance = new STDEVS(STDEVS_REPRODUCIBLE_SEED);
	}
	return STDEVS::ReproducibleGenerationInstance;
}

// Use this instance in models which need unique random numbers (generates a new seed)
STDEVS* STDEVS::getUniqueGenerationInstance(){
	if(STDEVS::UniqueGenerationInstance == NULL){
		STDEVS::UniqueGenerationInstance = new STDEVS(STDEVS_UNIQUE_SEED);
	}
	return STDEVS::UniqueGenerationInstance;
}

// Unless for specific needs, always use this instance which will point one of the previous based on the configuration or to a custom seeded.
STDEVS* STDEVS::getConfigurationInstance(){
	if(STDEVS::ConfigurationInstance == NULL){
		STDEVS::ConfigurationInstance = getConfiguredSTDEVS();
	}
	return STDEVS::ConfigurationInstance;
}


STDEVS::STDEVS(int randSeed){
	printLog(10, "STDEVS was Created \n");
	printLog(10, "time %u \n",time(NULL));
	printLog(10, "STDEVS randseed received %d \n",randSeed);
	switch (randSeed) {
	case STDEVS_REPRODUCIBLE_SEED:  // Reamains 0 => Default Seed of the RND generator will be invoked by GSL
		// This guarantees reproducibility of results
		printLog(1, "creating STDEVS instance that will create random values which will be the same in every simulation (seed=0) \n");
		break;
	case STDEVS_UNIQUE_SEED:
		// randSeed=(int)(getTime()*1000)+time(NULL); // Another option 1)
		// srand(time(NULL)); randSeed = rand()+(int)(getTime()*1000); // and Another option 2)

		// option 3)
		// More accuracy (microsecond has 1 000 000), but seed will repeat every 24 days
		// (use 100 instead of 1000 to repeat every every 248 days)
		struct timeval time; gettimeofday(&time, NULL);
		srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
		randSeed = rand();

		printLog(1, "creating STDEVS instance that will create new random values in every simulations. This simulation seed=%u \n", randSeed);

		break;
	default:
		printLog(1, "creating STDEVS instance with custom seed: %u \n", randSeed);
		// use the value as the seed: if you want to reproduce a previously seeded simulation
		break;
	}

	/* specifying to use Mersenne twister MT-19937 as the uniform PRNG */
	printLog(10, "STDEVS randseed calculated %u \n",randSeed);
	r = gsl_rng_alloc(gsl_rng_mt19937);
	gsl_rng_set (r, randSeed);    /* seed the PRNG */

	// gsl_rng_env_setup();
	// T = gsl_rng_default;
	// r = gsl_rng_alloc (T);
}

STDEVS* STDEVS::getConfiguredSTDEVS(){
	// set configuration singleton only once (called first time by the static singletons)
	int configuredSeed = STDEVS_DEFAULT_SEED;

	try {
		configuredSeed = readDefaultParameterValue<int>(SCILAB_VARNAME_REPRODUCIBLE_SIMULATION);
	} catch(...){
		printLog(1, "No configuration for STDEVS instance. Using the default one \n");
	}

	switch (configuredSeed) {
	case STDEVS_REPRODUCIBLE_SEED:
		printLog(1, "Configuration STDEVS set to STDEVS::StDevsReproducibleGenerationInstance \n");
		return STDEVS::getReproducibleGenerationInstance();
		break;
	case STDEVS_UNIQUE_SEED:
		printLog(1, "Configuration STDEVS set to STDEVS::StDevsUniqueGenerationInstance \n");
		return STDEVS::getUniqueGenerationInstance();;
		break;
	default:
		printLog(1, "Configuration STDEVS set to new STDEVS with custom seed \n");
		return STDEVS::ConfigurationInstance = new STDEVS(configuredSeed);
		break;
	}

	throw std::runtime_error("STDEVS::getConfiguredSTDEVS: Should not arrive here.\n");
}

double STDEVS::getWeightedElement(std::vector<double> elements, std::vector<double> weights){
	std::vector<double> acumulatedWeights = this->calculateAccumulatedWeights(weights);
	double totalCumulatedWeight = acumulatedWeights[acumulatedWeights.size()-1];

	double rnd = STDEVS::getConfigurationInstance()->uniform() * totalCumulatedWeight;
	//printLog(LOG_LEVEL_IMPORTANT,  "totalWeight=%f; random value =%f \n", totalCumulatedWeight, rnd);

	auto elementIt = std::upper_bound(acumulatedWeights.begin(),
			acumulatedWeights.end(), rnd);
	int elementIndex = std::distance(acumulatedWeights.begin(), elementIt);
	auto element = elements[elementIndex];
	return element;
}

std::vector<double> STDEVS::calculateAccumulatedWeights(std::vector<double> weights){
	std::vector<double> acumulatedWeights(weights.size());
	double totalCumulatedWeight = 0;
	for(unsigned int i = 0; i < weights.size(); i++){
		double actualWeight = weights[i];
		totalCumulatedWeight += actualWeight;
		acumulatedWeights[i] = totalCumulatedWeight;
	}

	return acumulatedWeights;
}

std::vector<int> STDEVS::getSortedByWeightSubset(std::vector<double> elements,
		std::vector<double> weights, const int length){

	sort(elements.begin(), elements.end(), WeightedComparator(weights));

	return(std::vector<int> (elements.begin(), elements.begin() + length));
}

std::vector<int> STDEVS::getWeightedSubset(std::vector<double> elements,
		std::vector<double> weights, const uint length){
	// validate
	if(elements.size() != weights.size() || elements.size() < length){
		printLog(1, "[STDEVSTool::getWeightedSubset] invalid call. elementsSize(%i) != weightsSize(%i) or lenght (%i) is bigger than amount of elements\n", elements.size(),weights.size(), length);
		throw std::runtime_error("[STDEVSTool::getWeightedSubset] invalid call");
	}

	std::vector<int> output;
	std::vector<double> acumulatedWeights = this->calculateAccumulatedWeights(weights);
	double totalCumulatedWeight = acumulatedWeights[acumulatedWeights.size()-1];

	int remaining = length;

	for(int i = 0; i < remaining; i ++){
		//            	printLog(LOG_LEVEL_IMPORTANT, "acumulatedWeights.size()=%u ;elements.size()=%u ; weights.size()=%u \n", acumulatedWeights.size(), elements.size(), weights.size());
		double rnd = STDEVS::getConfigurationInstance()->uniform() * totalCumulatedWeight;
		auto elementIt = std::upper_bound(acumulatedWeights.begin(),
				acumulatedWeights.end(), rnd);
		int elementIndex = std::distance(acumulatedWeights.begin(), elementIt);
		auto element = elements[elementIndex];
		output.push_back(element);

		// Recalculate cumsum
		totalCumulatedWeight = modifyCumsumAvoidIndex(elementIndex, acumulatedWeights,
				weights, elements);
	}
	return output;
}

double STDEVS::modifyCumsumAvoidIndex(const int avoidIndex,
		std::vector<double>& cumsum,
		std::vector<double>& weights,
		std::vector<double>& elements){

	cumsum.resize(avoidIndex); // Drop the old cumsum values as need to recalculate
	weights.erase(weights.begin() + avoidIndex);
	elements.erase(elements.begin() + avoidIndex);

	double totalCumWeight = cumsum[avoidIndex - 1];  // cumsum avoid -1 haves the last partial sum
	for(unsigned int i = avoidIndex + 1; i < weights.size(); i++){
		totalCumWeight += weights[i];
		cumsum.push_back(totalCumWeight);
	}
	return totalCumWeight;
}

