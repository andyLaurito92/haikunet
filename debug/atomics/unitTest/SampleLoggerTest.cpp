#include <stdlib.h>
#include <iostream>

#include "sinks/Loggers/SamplerLogger.h"
#include "MockClasses.h"

#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/included/unit_test.hpp>
BOOST_AUTO_TEST_SUITE(SamplerLoggerTests);

struct pair {
	std::pair<double, double> x;

	pair(std::pair<double, double> x) : x(x) {}
};
std::ostream& operator << ( std::ostream& out, const std::pair<double, double>& rhs ){
	out << "{" << rhs.first << ", " << rhs.second << "}";
	return out;
}

std::ostream& operator << ( std::ostream& out, const pair& rhs ){
	out << "{" << rhs.x.first << ", " << rhs.x.second << "}";
	return out;
}
inline bool operator==(const pair& lhs, const pair& rhs){
	return lhs.x == rhs.x;
}

BOOST_AUTO_TEST_CASE( initSignals_shouldAssertIfMoreThanOneVariable )
{
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1);

	bool asserted = false;
	try{
		logger->initSignals({
				{"count"},
				{"intergen"},
				{"sent_bits"}
		});
	} catch (...){
		asserted = true;
	}

	BOOST_REQUIRE_EQUAL(asserted, true);

}

BOOST_AUTO_TEST_CASE( initSignals_shouldThrowIfInitializeWithNegativeSamplingPeriod) {
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, -1);

	BOOST_REQUIRE_THROW(logger->initSignals({ {"count"} }), std::runtime_error);
}

BOOST_AUTO_TEST_CASE( initSignals_shouldInitializeAllSampleVariables) {
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1);

	logger->initSignals({ {"count"} });

	BOOST_REQUIRE_EQUAL(baseLogger->variableNames.size(), 6);
	BOOST_REQUIRE_MESSAGE(std::find(baseLogger->variableNames.begin(), baseLogger->variableNames.end(), "min") != baseLogger->variableNames.end(), "Base logger contains .min");
	BOOST_REQUIRE_MESSAGE(std::find(baseLogger->variableNames.begin(), baseLogger->variableNames.end(), "max") != baseLogger->variableNames.end(), "Base logger contains .max");
	BOOST_REQUIRE_MESSAGE(std::find(baseLogger->variableNames.begin(), baseLogger->variableNames.end(), "sum") != baseLogger->variableNames.end(), "Base logger  contains .sum");
	BOOST_REQUIRE_MESSAGE(std::find(baseLogger->variableNames.begin(), baseLogger->variableNames.end(), "avg") != baseLogger->variableNames.end(), "Base logger  contains .avg");
	BOOST_REQUIRE_MESSAGE(std::find(baseLogger->variableNames.begin(), baseLogger->variableNames.end(), "timeAvg") != baseLogger->variableNames.end(), "Base logger contains .timeAvg");
	BOOST_REQUIRE_MESSAGE(std::find(baseLogger->variableNames.begin(), baseLogger->variableNames.end(), "evCount") != baseLogger->variableNames.end(), "Base logger contains .evCount");
}

BOOST_AUTO_TEST_CASE(flush_shouldFlush) {
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1);

	BOOST_REQUIRE_EQUAL(baseLogger->flushed, false);

	logger->flush(1);

	BOOST_REQUIRE_EQUAL(baseLogger->flushed, true);
}

void RequireNumberOfLogs(std::shared_ptr<MockLogger> baseLogger, int numberOfLogs){
	// logged values
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues.size(), 6);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues.count("min"), true);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues.count("max"), true);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues.count("sum"), true);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues.count("avg"), true);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues.count("timeAvg"), true);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues.count("evCount"), true);

	// check amount of logs
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues["min"].size(), numberOfLogs);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues["max"].size(), numberOfLogs);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues["sum"].size(), numberOfLogs);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues["avg"].size(), numberOfLogs);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues["timeAvg"].size(), numberOfLogs);
	BOOST_REQUIRE_EQUAL(baseLogger->loggedValues["evCount"].size(), numberOfLogs);
}

BOOST_AUTO_TEST_CASE( logAndFlush_ShouldLogSingleValueInSinglePeriod) {
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1); // log every 1s

	logger->initSignals({ {"count"} });
	logger->logSignal(0.9, 1, "count"); // log at 0.5s  (first period)
	logger->flush(1); // flush at 1

	// logged values
	RequireNumberOfLogs(baseLogger, 1);

	// check values
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][0], pair({1, 1}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][0], pair({1, 1}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][0], pair({1, 1}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][0], pair({1, 1}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][0].first, 1);
	BOOST_CHECK_GE(baseLogger->loggedValues["timeAvg"][0].second, 0.09999999999999); // equal to 0.1 fails!
	BOOST_CHECK_LE(baseLogger->loggedValues["timeAvg"][0].second, 0.100000001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][0], pair({1, 1}));
}

BOOST_AUTO_TEST_CASE( logAndFlush_MultipleLogsInSameTimeShouldBeLoggedOnlyOnce) {
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1); // log every 1s

	logger->initSignals({ {"count"} });
	logger->logSignal(0.1, 1, "count"); // log at 0.5s  (first period)
	logger->logSignal(0.2, -1, "count"); // should not be taken into account
	logger->logSignal(0.2, 10, "count"); //
	logger->logSignal(0.3, 100, "count"); // should not be taken into account
	logger->logSignal(0.3, 9, "count"); //
	logger->logSignal(0.5, 7, "count"); //
	logger->flush(1); // flush at 1

	// logged values
	RequireNumberOfLogs(baseLogger, 1);

	// check values
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][0], pair({1, 1})); // -1 should not be taken into account
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][0], pair({1, 10})); // 100 should not be taken into account
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][0], pair({1, 27}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][0], pair({1, (double)27/4}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][0].first, 1);
	BOOST_CHECK_GE(baseLogger->loggedValues["timeAvg"][0].second, 6.3999999999); // equal to 6.4 fails!
	BOOST_CHECK_LE(baseLogger->loggedValues["timeAvg"][0].second, 6.400000001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][0], pair({1, 6}));
}

BOOST_AUTO_TEST_CASE( logAndFlush_ShouldLogMultipleValuesInSinglePeriod) {
	// Initialize!
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1); // log every 1s

	// Prepare!
	logger->initSignals({ {"count"} });
	logger->logSignal(0.1, 3, "count"); // (first period)
	logger->logSignal(0.4, -1, "count");
	logger->logSignal(0.6, 5, "count");
	logger->logSignal(0.9, 10, "count");
	logger->flush(1); // flush at 1

	// CHECK!
	RequireNumberOfLogs(baseLogger, 1);

	// check values
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][0], pair({1, -1}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][0], pair({1, 10}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][0], pair({1, 17}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][0], pair({1, (double)17/4}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][0].first, 1);
	BOOST_CHECK_GE(baseLogger->loggedValues["timeAvg"][0].second, 3.1999999999); // equal to 3.2 fails!
	BOOST_CHECK_LE(baseLogger->loggedValues["timeAvg"][0].second, 3.2000000001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][0], pair({1, 4}));
}

BOOST_AUTO_TEST_CASE( logAndFlush_ShouldLogMultipleValuesInMultiplePeriods) {
	// Initialize!
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1); // log every 1s

	// Prepare!
	logger->initSignals({ {"count"} });
	logger->logSignal(0.1, 3, "count");  // (first period)
	logger->logSignal(0.4, -1, "count");
	logger->logSignal(0.6, 5, "count");
	logger->logSignal(0.9, 10, "count");
	logger->logSignal(1.1, 20, "count"); // (second period)
	logger->logSignal(1.2, 30, "count");
	logger->logSignal(1.3, 40, "count");
	logger->logSignal(1.4, 50, "count");
	logger->logSignal(1.5, 60, "count");
	logger->logSignal(1.6, 70, "count");
	logger->logSignal(1.7, 80, "count");
	logger->logSignal(1.8, 90, "count");
	logger->logSignal(1.9, 100, "count");
	logger->logSignal(2.5, 1, "count"); // (third period)
	logger->logSignal(2.9, 90, "count"); //
	logger->flush(3); // flush at 3

	// CHECK!
	RequireNumberOfLogs(baseLogger, 3);

	// check values (first period)
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][0], pair({1, -1}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][0], pair({1, 10}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][0], pair({1, 17}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][0], pair({1, (double)17/4}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][0].first, 1);
	BOOST_CHECK_GE(baseLogger->loggedValues["timeAvg"][0].second, 3.1999999999); // equal to 3.2 fails!
	BOOST_CHECK_LE(baseLogger->loggedValues["timeAvg"][0].second, 3.2000000001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][0], pair({1, 4}));

	// check values (second period)
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][1], pair({2, 10})); // from previous period
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][1], pair({2, 100}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][1], pair({2, 550}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][1], pair({2, (double)550/10}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][1].first, 2);
	BOOST_CHECK_CLOSE(baseLogger->loggedValues["timeAvg"][1].second, 55, 0.0001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][1], pair({2, 9}));

	// check values (3rd period)
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][2], pair({3, 1}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][2], pair({3, 100}));// from previous period
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][2], pair({3, 191}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][2], pair({3, (double)191/3}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][2].first, 3);
	BOOST_CHECK_CLOSE(baseLogger->loggedValues["timeAvg"][2].second, 59.4, 0.0001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][2], pair({3, 2}));
}

BOOST_AUTO_TEST_CASE( logAndFlush_LoggingWithEmptyPeriodsShouldPreseveValuesFromPreviousPeriods) {
	// Initialize!
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 2); // log every 2s

	// Prepare!
	logger->initSignals({ {"count"} });
	logger->logSignal(0.1, 3, "count"); // (first period)
	logger->logSignal(0.9, 10, "count");
										// second period empty
	logger->logSignal(5.1, 20, "count");// 3rd period
	logger->logSignal(5.9, 30, "count");
	logger->flush(6); // flush at 6

	// CHECK!
	RequireNumberOfLogs(baseLogger, 3);

	// check values - first period
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][0], pair({2, 3}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][0], pair({2, 10}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][0], pair({2, 13}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][0], pair({2, (double)13/2}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][0].first, 2);
	BOOST_CHECK_CLOSE(baseLogger->loggedValues["timeAvg"][0].second, 6.7, 0.0001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][0], pair({2, 2}));

	// check values - second period
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][1], pair({4, 10})); // from previous periods
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][1], pair({4, 10})); // same, nothing happend
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][1], pair({4, 10}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][1], pair({4, 10}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][1].first, 4);
	BOOST_CHECK_CLOSE(baseLogger->loggedValues["timeAvg"][1].second, 10, 0.0001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][1], pair({4, 0}));

	// check values - 3rd period
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][2], pair({6, 10})); // from previous periods
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][2], pair({6, 30})); // same, nothing happend
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][2], pair({6, 60}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][2], pair({6, (double)60/3}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][2].first, 6);
	BOOST_CHECK_CLOSE(baseLogger->loggedValues["timeAvg"][2].second, 15, 0.0001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][2], pair({6, 2}));
}

BOOST_AUTO_TEST_CASE( logAndFlush_FlushingWithNoLogsShouldLogCorrectValues) {
	// TODO: this test is currently not passing, fix implementation

	// Initialize!
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1); // log every 1s

	// Prepare!
	logger->initSignals({ {"count"} });
	logger->logSignal(2.5, 33, "count");
	logger->flush(3); // flush at 3

	// CHECK!
	RequireNumberOfLogs(baseLogger, 3);

	// check values - first period
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["min"][0].first, 1); // t=1
	BOOST_CHECK(std::isnan(baseLogger->loggedValues["min"][0].second)); // nan as no values were received
	BOOST_CHECK(std::isnan(baseLogger->loggedValues["max"][0].second)); // nan as no values were received
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][0], pair({1, 0}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][0].first, 1);
	BOOST_CHECK(std::isnan(baseLogger->loggedValues["timeAvg"][0].second)); // nan
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][0], pair({1, 0}));

	// check values - second period
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["min"][1].first, 2); // t=2
	BOOST_CHECK(std::isnan(baseLogger->loggedValues["min"][1].second)); // nan as no values were received
	BOOST_CHECK(std::isnan(baseLogger->loggedValues["max"][1].second)); // nan as no values were received
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][1], pair({2, 0}));
	BOOST_CHECK(std::isnan(baseLogger->loggedValues["timeAvg"][1].second)); // nan
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][1], pair({2, 0}));

	// check values - 3rd period
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][2], pair({3, 33})); // from previous periods
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][2], pair({3, 33})); // same, nothing happend
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][2], pair({3, 33}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][2], pair({3, 33}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][2].first, 3);
	BOOST_CHECK_CLOSE(baseLogger->loggedValues["timeAvg"][2].second, 33/2, 0.0001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][2], pair({3, 1}));

}

BOOST_AUTO_TEST_CASE( logAndFlush_LogJustAtPeriodShouldLogInNextPeriod) {
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1); // log every 1s

	logger->initSignals({ {"count"} });
	logger->logSignal(0.5, 9, "count"); // log at 0.5s  (first period)
	logger->logSignal(1, 10, "count"); // log at 1s     (second period)
	logger->flush(1); // flush exactly at 1s

	// CHECK!
	RequireNumberOfLogs(baseLogger, 2);

	// check values (first period)
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][0], pair({1, 9}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][0], pair({1, 9}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][0], pair({1, 9}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][0], pair({1, 9.0}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][0].first, 1);
	BOOST_CHECK_CLOSE(baseLogger->loggedValues["timeAvg"][0].second, 4.5, 0.0001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][0], pair({1, 1}));

	// check values (second period)
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][1], pair({1, 9}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][1], pair({1, 10}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][1], pair({1, 10}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][1], pair({1, 10.0}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][1].first, 1);
	BOOST_CHECK(std::isnan(baseLogger->loggedValues["timeAvg"][1].second)); // the value elapsed 0 seconds
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][1], pair({1, 1}));
}


BOOST_AUTO_TEST_CASE( logAndFlush_FlushAtMiddleOfPeriodShouldAdjustTimeAvg) {
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 1); // log every 1s

	logger->initSignals({ {"count"} });
	logger->logSignal(0.5, 9, "count"); // log at 0.5s  (first period)
	logger->logSignal(1, 10, "count"); // log at 1s     (second period)
	logger->flush(1.5); // flush at middle of second period

	// CHECK!
	RequireNumberOfLogs(baseLogger, 2);

	// check values (first period)
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][0], pair({1, 9}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][0], pair({1, 9}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][0], pair({1, 9}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][0], pair({1, 9.0}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][0].first, 1);
	BOOST_CHECK_CLOSE(baseLogger->loggedValues["timeAvg"][0].second, 4.5, 0.0001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][0], pair({1, 1}));

	// check values (second period)
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["min"][1], pair({1.5, 9}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["max"][1], pair({1.5, 10}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["sum"][1], pair({1.5, 10}));
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["avg"][1], pair({1.5, 10.0}));
	BOOST_CHECK_EQUAL(baseLogger->loggedValues["timeAvg"][1].first, 1.5);
	BOOST_CHECK_CLOSE(baseLogger->loggedValues["timeAvg"][1].second, 10.0, 0.0001);
	BOOST_CHECK_EQUAL((pair)baseLogger->loggedValues["evCount"][1], pair({1.5, 1}));
}

BOOST_AUTO_TEST_CASE( logAndFlush_FlushJustAtPeriodWithoutLogShouldLogAllPeriods) {
	Simulator* model = new MockSimulator("someName");
	std::shared_ptr<MockLogger>  baseLogger (new MockLogger());
	SamplerLogger* logger = new SamplerLogger(model, baseLogger, 0.01); // log every 0.01

	logger->initSignals({ {"count"} });
	logger->flush(1); // flush exactly at 1s

	// CHECK!
	RequireNumberOfLogs(baseLogger, 99);
}

BOOST_AUTO_TEST_SUITE_END()


