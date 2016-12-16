//CPP:PhaseI/QueueSampler.cpp
//CPP:PhaseI/IValueSampler.cpp
#if !defined QueueSampler_h
#define ProcessingTimeValueCouterSampler_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "engine.h"

#include "IValueSampler.h"

/**
 * This model should not be necessary any more. Use the SampleLogger which directly samples without the need of an atomic model.
 */
class QueueSampler: public IValueSampler {
private:
	double discards = 0;

public:
	QueueSampler(const char *n): IValueSampler(n) {};
	~QueueSampler() { }

	void init(double t, ...){
		va_list parameters;
		va_start(parameters, t);

		IValueSampler::init(t, parameters);

		this->logger = createDefaultLogger(this->getFullName());
		this->logger->initSignals({"max", "timeAvg", "min", "sum", "count", "discards"});
	}

	double getValue(Event x){
		auto value = castEventPointer<double>(x).get();

		double newQueueSize =  value[0];
		double discard =  value[1];

		this->discards += discard;
		return newQueueSize;
	}

	Event lambda(double t){
		auto e = IValueSampler::lambda(t);

		logger->logSignal(t, this->discards,"discards");

		this->discards = 0;

		return e;

	}


};
#endif
