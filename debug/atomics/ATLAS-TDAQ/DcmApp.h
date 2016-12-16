//CPP:ATLAS-TDAQ/DcmApp.cpp
//CPP:ATLAS-TDAQ/ScalarSimulator.cpp
#if !defined DcmApp_h
#define DcmApp_h

#include "ATLAS-TDAQ/globals.h"
#include "simulator.h"
#include "event.h"
#include "stdarg.h"
#include "math.h"
#include "sinks/Loggers/IPowerDEVSLogger.h"
#include "common/BaseSimulator.h"
#include "hybrid/packettool.h"
#include "tdaqPacketTool.h"
#include "ATLAS-TDAQ/L1TConfiguration.h"
#include "ScalarSimulator.h"
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <list>
#include <utility>
#include <memory>

/*!
  - Accepts requests on inport 0 and forwards them on outport 0 if there are available tickets
  - Accepts new tickets on inport 1
  - Ouputs requests on outport 0

  - Delays requests until there are available credits

Update: a ticket is assigned for each fragments in the request

 */
class DcmApp: public BaseSimulator, ScalarSimulator  {
	/*
	 * This would be the one representing the RosDataCollector.cxx::Session
	 */
	class Session {
	public:
		int m_rosId; // there is a session per ROS (this would be the RosDataCollector.cxx::Session::m_remoteName and m_remoteEndpoint)
		std::map<int, std::shared_ptr<FragmentRequestInfo>> m_pendingTransactions; //<transactionId, [robIds]>  RosDataCollector.cxx::Session::m_pendingTransactions
		int m_credits;
		int m_nextTransactionId=0;

		Session(int rosId){
			m_rosId = rosId;
			m_credits=0;
			m_nextTransactionId=0;
		}

		/// Code from RosDataCollector.cxx::Session::minCreditsNeeded
		int minCreditsNeeded()
		{
		    if (m_pendingTransactions.empty()) {
		      return 0;
		    }
		    if (m_credits >= m_pendingTransactions.begin()->second->robIds.size()) {
		      return 0;
		    }
		    else {
		      return m_pendingTransactions.begin()->second->robIds.size() - m_credits;
		    }
		 }

		/// Code from RosDataCollector.cxx::Session::giveCredits
		void giveCredits(int n) {
			m_credits += n;
		    //doFetch(); // NOTE: this is done in the lambda function that calls popNextTransaction
		}

		bool hasEnoughCredits(){
			auto itPending = m_pendingTransactions.begin();  // get first transaction
			return itPending != m_pendingTransactions.end() && itPending->second->robIds.size() <= m_credits;
		}

		/// Code SIMILAR to RosDataCollector.cxx::Session::doFetch but only for one transaction, as in the simulation it has to be called from the labda function
		std::shared_ptr<FragmentRequestInfo> popNextTransaction(){
			auto itPending = m_pendingTransactions.begin();  // get first transaction

			if (!hasEnoughCredits()) {
				printLog(LOG_LEVEL_ERROR,"[???] popNextTransaction: ERROR!!! Trying to get a transaction for ROS=%i when there are no more available transactions. Sending empty transaction ...  \n", m_rosId);
				throw "Invalid state";
			}

			std::shared_ptr<FragmentRequestInfo> ret = itPending->second; // make a copy

			m_pendingTransactions.erase(itPending); // delete it from pending
			m_credits -= ret->robIds.size(); // decrease credits

			return ret;
		}

		/// Code SIMILAR to RosDataCollector.cxx::Session::asyncFetch
		void asyncFetch(int eventId, std::vector<int> robIds, int puId, int dcmId){
			std::sort(robIds.begin(), robIds.end());
			m_pendingTransactions[m_nextTransactionId] = std::make_shared<FragmentRequestInfo>(eventId, m_rosId, robIds, puId, dcmId);
			++m_nextTransactionId;
//			doFetch(); // NOTE: this is done in the lambda function that calls popNextTransaction
		}

		void printInfo(){
			printLog(LOG_LEVEL_FULL_LOGGING, "asyncFetch: %i pending transactions for ROS=%i  (m_nextTransactionId=%i):\n ", m_pendingTransactions.size(), m_rosId,m_nextTransactionId);
			for (auto transaction : m_pendingTransactions) {
				printLog(LOG_LEVEL_FULL_LOGGING, "asyncFetch: transaction %i has %i ROBs \n", transaction.first, transaction.second->robIds.size());
				for (auto robId : transaction.second->robIds) {
					printLog(LOG_LEVEL_FULL_LOGGING, "asyncFetch: transaction %i has ROB=%i (for event %i) \n", transaction.first, robId, transaction.second->eventId);
				}
			}
		}


	};

	/*
	* This would be the one representing the RosDataCollector.cxx::Transaction
	*/
	struct Transaction
	{
	  Transaction(/*Event* event_, void* context_*/) :
	      /*event(event_),
	      context(context_),*/
	      nPendingRosTransactions(0)
	  {
	  }

	  /*Event* event;
	  void* context;*/
	  int nPendingRosTransactions;
	};

// Variables as in RosDataCollector.cxx
std::vector<std::shared_ptr<Session>> m_sessions;
std::map<int, std::shared_ptr<Session>> m_robIdToSession;
std::list<Transaction> m_transactions;
int m_creditsAvailable; // available tickets
int total_credits;

// Model parameters
int requestSize; // Size of the requests sent

// state variables
std::queue<std::shared_ptr<Session>> outQueue; // sessions already selected to be sent in distributeCredits, but needs to be handled one by one in the lambda function


// Helpers
double lastAsigmentTime;

public:
	DcmApp(const char *n): BaseSimulator(n), ScalarSimulator(n) {};
	void init(double, ...);
	void dint(double);
	void dext(Event , double );
	Event lambda(double);
private:
	void distributeCredits();
	void setSigma(double t);
	int getTotalWaitingTransactions();

};
#endif
