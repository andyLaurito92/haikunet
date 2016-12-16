//CPP:hybrid/WRRScheduler.cpp
#if !defined WRRScheduler_h
#define WRRScheduler_h

#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include <iterator>
#include <vector>

#include "sinks/ParameterReader.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "vector/VectorialEvent.h"

#include "NetworkData/NetworkPacket.h"
#include "common/BaseSimulator.h"


class WRRScheduler: public BaseSimulator {
protected: // we set variables as protected to be able to read them from derived classes (like wrappers in tests)
	// parameters
	std::vector<double> queueWeights; // TODO: this should be a vector<int>, but there is a limitation in scilab that allows to read only vector<double>

	// State Variables
	int WRRQueueIndex;
	int pendingWeightForCurrentIndex;
	std::vector<int> queueSizes_bytes;
	bool delayedRequestPending; // it is true if a request was requested, but it was delayed because all queues were empty

	// Counters
	uint sumQueueSizes = 0; // counter to track the sizes of all queues (just for logging purposes)


public:
	WRRScheduler(const char *n): BaseSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
private:
	void dext_queueSize(Event x, double t);
	void dext_dequeRequest();
	int getNextWRRIndex(); // WRR algorithm

};

#endif
