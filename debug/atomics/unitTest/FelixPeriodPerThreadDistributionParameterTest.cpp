#include <stdlib.h>
#include <iostream>

#include "hybrid/DistributionParameters/FelixPeriodPerThreadDistributionParameter.h"
#include "sinks/ParameterReader.h"
#include "MockClasses.h"

#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/included/unit_test.hpp>
BOOST_AUTO_TEST_SUITE(FelixPeriodPerThreadDistributionParameterTest);

class FelixPeriodPerThreadDistributionParameter_wrapper : public FelixPeriodPerThreadDistributionParameter { // just exposes the protected members
public:
	FelixPeriodPerThreadDistributionParameter_wrapper(): FelixPeriodPerThreadDistributionParameter() {};

	std::shared_ptr<IDistributionParameter> getSizeDistribution() {  return this->sizeDistribution; }
	std::shared_ptr<IDistributionParameter> getPeriodDistribution() {  return this->periodDistribution; }
	std::vector<std::string> getNames() {  return this->parameterNames; }
	std::vector<double> getValues() {  return this->parameterValues; }
	FelixPeriodPerThreadDistributionParameter::FELIX_MODE getMode() { return this->mode;};
	double getBuffer_bytes() { return FelixPeriodPerThreadDistributionParameter::getBuffer_bytes(); } ;
	double getTimeout() { return FelixPeriodPerThreadDistributionParameter::getTimeout(); } ;
	double getOutSize_bytes() { return FelixPeriodPerThreadDistributionParameter::getOutSize_bytes(); } ;
};

BOOST_AUTO_TEST_CASE(getName_shouldReturnFelix)
{
	CmdLineModelParams.clear();
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	BOOST_REQUIRE_EQUAL(distribution->getName(), "FelixGBTLink");
}

BOOST_AUTO_TEST_CASE(getFullName_withoutReadingParams_shouldReturnFelix)
{
	CmdLineModelParams.clear();

	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();

	// without reading params
	std::string fullName = distribution->getFullName();

	BOOST_MESSAGE("The full name is: " + fullName);
	BOOST_REQUIRE(fullName.find("FelixGBTLink") != std::string::npos);
}

BOOST_AUTO_TEST_CASE(readParameters_whenLowLatencyModel_shouldReadOnlyPeriodAndModeParameters)
{
	CmdLineModelParams.clear();
	setCmdLineParameter("NotGoodParam", 1234);
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	auto paramDistributionName = "felix_dist";

	// base parameters: size, period, buffer, mode, timeout
	distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	BOOST_REQUIRE_EXCEPTION(distribution->readParameters(paramDistributionName), std::runtime_error, [](std::runtime_error const& ex)
				{ return std::string(ex.what()).find("felix_dist_period") != std::string::npos; /*found*/} );
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::EXPONENTIAL);

	distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	BOOST_REQUIRE_EXCEPTION(distribution->readParameters(paramDistributionName), std::runtime_error, [](std::runtime_error const& ex)
					{ return std::string(ex.what()).find("felix_dist_mode") != std::string::npos; /*found*/} );
	setCmdLineParameter("felix_dist_mode", 66799);

	// specific values
	distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	BOOST_REQUIRE_EXCEPTION(distribution->readParameters(paramDistributionName), std::runtime_error, [](std::runtime_error const& ex)
						{ return std::string(ex.what()).find("mode") != std::string::npos; /*found*/} );
	CmdLineModelParams.clear();
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::EXPONENTIAL);
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_LOW_LATENCY);

	distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	BOOST_REQUIRE_EXCEPTION(distribution->readParameters(paramDistributionName), std::runtime_error, [](std::runtime_error const& ex)
										{ return std::string(ex.what()).find("felix_dist_period_mu") != std::string::npos; /*found*/} );
	setCmdLineParameter("felix_dist_period_mu", 77);

	// no exceptions
	auto distribution_wrapper = std::make_shared<FelixPeriodPerThreadDistributionParameter_wrapper>();
	BOOST_REQUIRE_NO_THROW(distribution_wrapper->readParameters(paramDistributionName));

	// only 2 values read
	BOOST_REQUIRE_EQUAL(distribution_wrapper->getNames().size(), 2);
	BOOST_REQUIRE_EQUAL(distribution_wrapper->getValues().size(), 2);
}

BOOST_AUTO_TEST_CASE(readParameters_shouldThrowExceptionIfParametersAreNotDefined)
{
	CmdLineModelParams.clear();
	setCmdLineParameter("NotGoodParam", 1234);
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	auto paramDistributionName = "felix_dist";

	// base parameters: size, period, buffer, mode, timeout
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::EXPONENTIAL);
	setCmdLineParameter("felix_dist_period_mu", 77);
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT);

	BOOST_REQUIRE_EXCEPTION(distribution->readParameters(paramDistributionName), std::runtime_error, [](std::runtime_error const& ex)
			{ return std::string(ex.what()).find("felix_dist_size_bytes") != std::string::npos; /*found*/} );
	setCmdLineParameter("felix_dist_size_bytes", DISTRIBUTION_PARAMETER::CONSTANT);

	distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	BOOST_REQUIRE_EXCEPTION(distribution->readParameters(paramDistributionName), std::runtime_error, [](std::runtime_error const& ex)
				{ return std::string(ex.what()).find("felix_dist_buffer_bytes") != std::string::npos; /*found*/} );
	setCmdLineParameter("felix_dist_buffer_bytes", 33);


	distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	BOOST_REQUIRE_EXCEPTION(distribution->readParameters(paramDistributionName), std::runtime_error, [](std::runtime_error const& ex)
						{ return std::string(ex.what()).find("felix_dist_timeout") != std::string::npos; /*found*/} );
	setCmdLineParameter("felix_dist_timeout", 55);

	distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	BOOST_REQUIRE_EXCEPTION(distribution->readParameters(paramDistributionName), std::runtime_error, [](std::runtime_error const& ex)
						{ return std::string(ex.what()).find("felix_dist_out_size_bytes") != std::string::npos; /*found*/} );
	setCmdLineParameter("felix_dist_out_size_bytes", 11);


	// sub distributions: constant and exp (set before)
	distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	BOOST_REQUIRE_EXCEPTION(distribution->readParameters(paramDistributionName), std::runtime_error, [](std::runtime_error const& ex)
								{ return std::string(ex.what()).find("felix_dist_size_bytes_value") != std::string::npos; /*found*/} );
	setCmdLineParameter("felix_dist_size_bytes_value", 66);

	// no exceptions
	distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	distribution->readParameters(paramDistributionName);

}

BOOST_AUTO_TEST_CASE(readParameters_withLowLatency_shouldLoadParametersCorrectly)
{
	CmdLineModelParams.clear();
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter_wrapper>();
	auto paramDistributionName = "felix_dist";

	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::EXPONENTIAL);
	setCmdLineParameter("felix_dist_period_mu", 44);
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_LOW_LATENCY);

	// test
	distribution->readParameters(paramDistributionName);

	// expections
	auto names = distribution->getNames();
	auto values = distribution->getValues();
	BOOST_REQUIRE_EQUAL(names.size(), 2);
	BOOST_REQUIRE_EQUAL(values.size(), 2);

	int paramIndex = 0;
	BOOST_CHECK_EQUAL(names[paramIndex], "period");
	BOOST_CHECK_EQUAL(values[paramIndex++], DISTRIBUTION_PARAMETER::EXPONENTIAL);
	auto distribution_period = distribution->getPeriodDistribution();
	BOOST_CHECK_EQUAL(distribution_period->getName(), "Exponential");
	BOOST_CHECK(distribution_period->getFullName().find("mu=44") != std::string::npos);

	BOOST_CHECK_EQUAL(names[paramIndex], "mode");
	BOOST_CHECK_EQUAL(values[paramIndex++], FelixPeriodPerThreadDistributionParameter::FELIX_MODE_LOW_LATENCY);
	BOOST_CHECK_EQUAL(distribution->getMode(), FelixPeriodPerThreadDistributionParameter::FELIX_MODE_LOW_LATENCY);
}

BOOST_AUTO_TEST_CASE(readParameters_withHighThroughoput_shouldLoadParametersCorrectly)
{
	CmdLineModelParams.clear();
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter_wrapper>();
	auto paramDistributionName = "felix_dist";

	setCmdLineParameter("felix_dist_size_bytes", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_size_bytes_value", 66.5);
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::EXPONENTIAL);
	setCmdLineParameter("felix_dist_period_mu", 77.1);
	setCmdLineParameter("felix_dist_buffer_bytes", 33.5);
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT);
	setCmdLineParameter("felix_dist_timeout", 22.123);
	setCmdLineParameter("felix_dist_out_size_bytes", 31);

	// test
	distribution->readParameters(paramDistributionName);

	// expections
	auto names = distribution->getNames();
	auto values = distribution->getValues();
	BOOST_REQUIRE_EQUAL(names.size(), 6);
	BOOST_REQUIRE_EQUAL(values.size(), 6);

	int paramIndex = 0;
	BOOST_CHECK_EQUAL(names[paramIndex], "period");
	BOOST_CHECK_EQUAL(values[paramIndex++], DISTRIBUTION_PARAMETER::EXPONENTIAL);
	auto distribution_period = distribution->getPeriodDistribution();
	BOOST_CHECK_EQUAL(distribution_period->getName(), "Exponential");
	BOOST_CHECK(distribution_period->getFullName().find("mu=77.1") != std::string::npos);

	BOOST_CHECK_EQUAL(names[paramIndex], "mode");
	BOOST_CHECK_EQUAL(values[paramIndex++], FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT);
	BOOST_CHECK_EQUAL(distribution->getMode(), FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT);

	BOOST_CHECK_EQUAL(names[paramIndex], "size_bytes");
	BOOST_CHECK_EQUAL(values[paramIndex++], DISTRIBUTION_PARAMETER::CONSTANT);
	auto distribution_size = distribution->getSizeDistribution();
	BOOST_CHECK_EQUAL(distribution_size->getName(), "Constant");
	BOOST_CHECK_EQUAL(distribution_size->nextValue(), 66.5);

	BOOST_CHECK_EQUAL(names[paramIndex], "buffer_bytes");
	BOOST_CHECK_EQUAL(values[paramIndex++], 33.5);
	BOOST_CHECK_EQUAL(distribution->getBuffer_bytes(), 33.5);

	BOOST_CHECK_EQUAL(names[paramIndex], "timeout");
	BOOST_CHECK_EQUAL(values[paramIndex++], 22.123);
	BOOST_CHECK_EQUAL(distribution->getTimeout(), 22.123);

	BOOST_CHECK_EQUAL(names[paramIndex], "out_size_bytes");
	BOOST_CHECK_EQUAL(values[paramIndex++], 31);
	BOOST_CHECK_EQUAL(distribution->getOutSize_bytes(), 31);
}

BOOST_AUTO_TEST_CASE(getFullName_readingParamsLowLatency_shouldReturnFelixAndParams)
{
	CmdLineModelParams.clear();
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::EXPONENTIAL);
	setCmdLineParameter("felix_dist_period_mu", 44);
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_LOW_LATENCY);

	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	distribution->readParameters("felix_dist");

	std::string fullName = distribution->getFullName();

	BOOST_MESSAGE("The full name is: " + fullName);
	BOOST_CHECK(fullName.find("FelixGBTLink") != std::string::npos);
	BOOST_CHECK(fullName.find(std::to_string(DISTRIBUTION_PARAMETER::EXPONENTIAL)) != std::string::npos);
	BOOST_CHECK(fullName.find(std::to_string(FelixPeriodPerThreadDistributionParameter::FELIX_MODE_LOW_LATENCY)) != std::string::npos);
}

BOOST_AUTO_TEST_CASE(getFullName_readingParamsHighThroughput_shouldReturnFelixAndParams)
{
	CmdLineModelParams.clear();
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::EXPONENTIAL);
	setCmdLineParameter("felix_dist_period_mu", 44);
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT);
	setCmdLineParameter("felix_dist_size_bytes", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_size_bytes_value", 66.5);
	setCmdLineParameter("felix_dist_buffer_bytes", 33.5);
	setCmdLineParameter("felix_dist_timeout", 22.123);
	setCmdLineParameter("felix_dist_out_size_bytes", 31);

	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter>();
	distribution->readParameters("felix_dist");

	std::string fullName = distribution->getFullName();

	BOOST_MESSAGE("The full name is: " + fullName);
	BOOST_CHECK(fullName.find("FelixGBTLink") != std::string::npos);
	BOOST_CHECK(fullName.find(std::to_string(DISTRIBUTION_PARAMETER::EXPONENTIAL)) != std::string::npos);
	BOOST_CHECK(fullName.find(std::to_string(FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT)) != std::string::npos);
}

BOOST_AUTO_TEST_CASE(nextValue_withLowLatency_shouldForwardInmediatlyAccordingToPeriodDistribution)
{
	CmdLineModelParams.clear();
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter_wrapper>();
	auto paramDistributionName = "felix_dist";

	// TODO: use a custom testing distribution --> copy from felixGeneratorTests
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_period_value", 0.1); // 10 packets per second
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_LOW_LATENCY);

	// test
	distribution->readParameters(paramDistributionName);

	// assert ==> should forward inmediatly, independent of buffer, size, etc
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0.1);
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0.1);
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0.1);
}

BOOST_AUTO_TEST_CASE(nextValue_withBigBufferAndHighThroughput_shouldUseTheTimeoutAsManyTimesAsSizeOverTime )
{
	CmdLineModelParams.clear();
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter_wrapper>();
	auto paramDistributionName = "felix_dist";

	setCmdLineParameter("felix_dist_size_bytes", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_size_bytes_value", 1); // 1B per packet
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_period_value", 1); // 1 packet per second
	setCmdLineParameter("felix_dist_buffer_bytes", std::numeric_limits<int>::max());  // buffer will not get filled
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT);
	setCmdLineParameter("felix_dist_timeout", 10);
	setCmdLineParameter("felix_dist_out_size_bytes", 2);  // each out packet of 2B

	// test
	distribution->readParameters(paramDistributionName);

	// assert ==> should mark 5 out packets of 2B each, total 10B in the 10s timeout
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 10); // first period should come with the time out
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 4 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 4 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 4 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 4 other times 0

	// start again
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 10); // first period should come with the time out
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 4 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 4 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 4 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 4 other times 0
}

BOOST_AUTO_TEST_CASE(nextValue_withBigBufferAndHighThroughput_shouldUseTheTimeoutAsManyTimesAsSizeOverTimeRoundingUp )
{
	CmdLineModelParams.clear();
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter_wrapper>();
	auto paramDistributionName = "felix_dist";

	setCmdLineParameter("felix_dist_size_bytes", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_size_bytes_value", 1); // 1B per packet
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_period_value", 1); // 1 packet per second
	setCmdLineParameter("felix_dist_buffer_bytes", std::numeric_limits<int>::max());  // buffer will not get filled
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT);
	setCmdLineParameter("felix_dist_timeout", 11);
	setCmdLineParameter("felix_dist_out_size_bytes", 2);  // each out packet of 2B

	// test
	distribution->readParameters(paramDistributionName);

	// assert ==> should mark 6 out packets of 2B each, total 12B (round up 11/2) in the 10s timeout
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 11); // first period should come with the time out
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0

	// start again
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 11); // first period should come with the time out
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 5 other times 0
}

BOOST_AUTO_TEST_CASE(nextValue_withHighThroughput_shouldUseBufferSizeAndRateAndSize)
{
	CmdLineModelParams.clear();
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter_wrapper>();
	auto paramDistributionName = "felix_dist";

	setCmdLineParameter("felix_dist_size_bytes", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_size_bytes_value", 1); // 1B per packet
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_period_value", 1); // 10 packets per second
	setCmdLineParameter("felix_dist_buffer_bytes", 6);  // buffer can fit 6 packets --> 0.6s to get filled
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT);
	setCmdLineParameter("felix_dist_timeout", 100); // big timeout wont be reached
	setCmdLineParameter("felix_dist_out_size_bytes", 2);  // each out packet of 2B  --> 3 packets per buffer

	// test
	distribution->readParameters(paramDistributionName);

	// assert ==> should mark 3 out packets of 2B each, total 6B in  0.5s
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 6); // first period should come with bufferSize/(rate*size)
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 2 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 2 other times 0

	// start again
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 6); // first period should come with bufferSize/(rate*size)
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 2 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 2 other times 0
}

BOOST_AUTO_TEST_CASE(nextValue_withHighThroughputAndBigBuffer_shouldRoundUp_alsoToMeasurePerformance)
{
	CmdLineModelParams.clear();
	auto distribution = std::make_shared<FelixPeriodPerThreadDistributionParameter_wrapper>();
	auto paramDistributionName = "felix_dist";

	int totalPacketsInBuffer = 10 * 1000 * 1000; // Lar will have 4kB messages at 100kHz => 1MB fits 250 messages
	int outSize = 3;
	setCmdLineParameter("felix_dist_size_bytes", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_size_bytes_value", 1); // 1B per packet
	setCmdLineParameter("felix_dist_period", DISTRIBUTION_PARAMETER::CONSTANT);
	setCmdLineParameter("felix_dist_period_value", 1); // 10 packets per second
	setCmdLineParameter("felix_dist_buffer_bytes", totalPacketsInBuffer);  // buffer can fit 100k packets --> 100k seg to get filled
	setCmdLineParameter("felix_dist_mode", FelixPeriodPerThreadDistributionParameter::FELIX_MODE_HIGH_THROUGHPUT);
	setCmdLineParameter("felix_dist_timeout", std::numeric_limits<double>::max()); // big timeout wont be reached
	setCmdLineParameter("felix_dist_out_size_bytes", outSize);  // each out packet of 2B  --> 3 packets per buffer

	// test
	distribution->readParameters(paramDistributionName);

	// assert ==> should mark 33334 out packets of 3B each, total 100kB/3 (rounded up) in  100k seg
	auto startTime = std::chrono::system_clock::now();
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), totalPacketsInBuffer); // first period should come with bufferSize/(rate*size)
	auto finishTime = std::chrono::system_clock::now();

	auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - startTime);
	BOOST_MESSAGE("Time to generate " + std::to_string(totalPacketsInBuffer) + "*2 random numbers: " + std::to_string(totalTime.count()) + " ms \n");

	int counter = 0;
	double lastValue;
	while((lastValue = distribution->nextValue()) == 0 ) counter++;

	int expectedTimes = (totalPacketsInBuffer + outSize - 1) / outSize;
	BOOST_REQUIRE_EQUAL(counter, expectedTimes - 1); // then, 33333 other times 0

	// start again
	BOOST_REQUIRE_EQUAL(lastValue, totalPacketsInBuffer); // first period should come with bufferSize/(rate*size)
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 2 other times 0
	BOOST_REQUIRE_EQUAL(distribution->nextValue(), 0); // then, 2 other times 0
}



/* TODO: check next value:
 *              - custom distribution: not always constant: copy from FelixMultiFlowTest
 * */


BOOST_AUTO_TEST_SUITE_END()


