/*
 * main.cpp
 *
 *  Created on: Jun 17, 2016
 *      Author: mbonaven
 */

double tf; // required by PowerDEVS to compile :-( TODO: remove

// TODO:
// - check why we need to include the .cpp files instead of the .h

/**** Boost UTF includes ****/
//#define BOOST_TEST_STATIC_LINK
#define BOOST_TEST_MODULE "C++ Unit Tests for PowerDEVS"
#include <boost/test/included/unit_test.hpp>

/** DEVS test framework **/
#include "PowerDEVSTestFramework/CommonDEVSTestUtilities.h"
#include "PowerDEVSTestFramework/FunctionalTestRootSimulator.cpp"
#include "PowerDEVSTestFramework/FunctionalTesterModel.cpp"


/********  Add your new tests here: #include "yourTest.cpp"   ************/
#include "SampleLoggerTest.cpp"
#include "WRRSchedulerTest.cpp"
#include "RouterTest.cpp"
#include "FelixServerTest.cpp"
#include "FelixPeriodPerThreadDistributionParameterTest.cpp"

// #include "yourTest.cpp"




// TODO: evaluate using some unit test framework:
// googleTest: https://github.com/google/googletest
// boost: http://www.boost.org/doc/libs/1_46_0/libs/test/doc/html/utf.html
//        - boost assets refer to: http://www.boost.org/doc/libs/1_46_1/libs/test/doc/html/utf/testing-tools/reference.html
// kind-of complete list: https://en.wikipedia.org/wiki/List_of_unit_testing_frameworks#C.2B.2B


