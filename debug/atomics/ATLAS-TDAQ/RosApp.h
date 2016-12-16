//CPP:ATLAS-TDAQ/RosApp.cpp
//CPP:hybrid/stdevstool.cpp
#if !defined RosApp_h
#define RosApp_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "hybrid/stdevstool.h"
#include "hybrid/packettool.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "tdaqPacketTool.h"
#include <queue>          // std::priority_queue

class RosApp: public BaseSimulator {
	// Classes to support queuedJobs sorting
	class QueuedJob{
		double finishTime;
		std::shared_ptr<TdaqPacket> jobRequest;
	public:
		QueuedJob(double processingTime, std::shared_ptr<TdaqPacket>& jobRequest){
			this->finishTime = processingTime;
			this->jobRequest = jobRequest;
		}

		const std::shared_ptr<TdaqPacket>& getJobRequest() const {
			return jobRequest;
		}

		double getFinishTime() const {
			return finishTime;
		}
	};

	class QueuedJobComparison
	{
	public:
	    bool operator() (std::shared_ptr<QueuedJob> left, std::shared_ptr<QueuedJob> right)
	    {
	        return left->getFinishTime() > right->getFinishTime();
	    }
	};

	bool nextJobRequested = false;
	std::priority_queue<std::shared_ptr<QueuedJob>, std::vector<std::shared_ptr<QueuedJob>>,QueuedJobComparison> requestsBeingProcessed;

	// Parameters
	double processingTime;
	int fragmentSize;
	int maxParallelJobs;

	//	std::shared_ptr<TdaqPacket> requestPacket;
	long counter;
	double lastEmitionTime;
	int stochasticSimulation;

	// Helpers


	public:
		RosApp(const char *n): BaseSimulator(n) {};
		void init(double, ...);
		void dint(double);
		void dext(Event , double );
		Event lambda(double);
	private:
		void setNextSigma(double t);
		bool isNextJobNeeded() { return maxParallelJobs > this->requestsBeingProcessed.size() && nextJobRequested == false; };
};
#endif
