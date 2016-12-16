#ifndef STDEVSTOOL_H
#define STDEVSTOOL_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <sys/time.h>

#include "pdevslib.h"
#include <algorithm>    // std::binary_search, std::sort
#include <vector>

//#include "sink/ParameterReader.h"

#define STDEVS_REPRODUCIBLE_SEED 0  // Random reproducible generation. reproducibility of results
#define STDEVS_UNIQUE_SEED -1  // Random unique generation
#define STDEVS_DEFAULT_SEED STDEVS_UNIQUE_SEED  // default in case no configuration

#define SCILAB_VARNAME_REPRODUCIBLE_SIMULATION "ReproducibleSimu" // name to be used in the configuration

//* Forward declare the functions in ParameterReader.h. Including creates circular inclusion problems*/
//#include "sinks/ParameterReader.h"
void printLog(int level, const char *fmt,...);
template<typename T> T readDefaultParameterValue(std::string paramName);


/*
 * Class to generate random numbers.
 * It provides 3 singleton which can be used (it is advised to use always the first one):
 *     1) Random number generation based on the configuration (probably one of the instances below or a custom seeded)
 *     2) Reproducible random numbers (uses always the same seed)
 *     3) need unique random numbers (generates a new seed for every simulation)
 * It is advisable to use the IDistributionParameter instances, so that random number generation is based on the configuration.
 * If you don't want to read from the configuration, it is advisable to use the singleton instances available in this class. Only create new instances if you know what you are doing
 */
class STDEVS {
public:
 // Use this instance in models which need reproducible random numbers (uses always the same seed)
 static STDEVS* getReproducibleGenerationInstance();

 // Use this instance in models which need unique random numbers (generates a new seed)
 static STDEVS* getUniqueGenerationInstance();

 // Unless for specific needs, always use this instance which will point one of the previous based on the configuration or to a custom seeded.
 static STDEVS* getConfigurationInstance();


 /*
  * It is advisable to use the IDistributionParameter instances, so that random number generation is based on the configuration.
  *  If you don't want to read from the configuration, it is advisable to use the singleton instances available in this class. Only create new instances if you know what you are doing.
  *  If you are creating several instances of STDEVS in a very short time (for example when using VectorialDEVS), evaluate using the singletons available in this class to avoid generating the same seed in several instances.
  *
  */
 STDEVS(int randSeed);
	  
  ~STDEVS() {printLog(10, "STDEVS Destroyed \n");};

	unsigned int poisson(double mu) { return gsl_ran_poisson (this->r, mu); }
	double bernoulli (double p) { return gsl_ran_bernoulli (this->r, p); }
	double exponential(double mu) { return gsl_ran_exponential (this->r, mu); }
	double uniform () { return gsl_rng_uniform (this->r); }
	double normal(double mu, double var) { return gsl_ran_gaussian(this -> r, var) + mu;}
	double pareto(double order, double minValue) {	return gsl_ran_pareto(this -> r, order, minValue); }


	double getWeightedElement(std::vector<double> elements, std::vector<double> weights);
	std::vector<double> calculateAccumulatedWeights(std::vector<double> weights);
	std::vector<int> getSortedByWeightSubset(std::vector<double> elements, std::vector<double> weights, const int length);
	std::vector<int> getWeightedSubset(std::vector<double> elements, std::vector<double> weights, const uint length);
	double modifyCumsumAvoidIndex(const int avoidIndex,
			std::vector<double>& cumsum,
			std::vector<double>& weights,
			std::vector<double>& elements);
private:
	 // Use this instance in models which need reproducible random numbers (uses always the same seed)
	 static STDEVS* ReproducibleGenerationInstance;

	 // Use this instance in models which need unique random numbers (generates a new seed)
	 static STDEVS* UniqueGenerationInstance;

	 // Unless for specific needs, always use this instance which will point one of the previous based on the configuration or to a custom seeded.
	 static STDEVS* ConfigurationInstance;
	 static STDEVS* getConfiguredSTDEVS();


	gsl_rng * r; // random seed for the instance

	struct WeightedComparator
	{
	    const std::vector<double> & value_vector;

	    WeightedComparator(const std::vector<double> & val_vec):
	        value_vector(val_vec) {}

	    bool operator()(int i1, int i2)
	    {
	        return value_vector[i1] < value_vector[i2];
	    }
	};
};



#endif
