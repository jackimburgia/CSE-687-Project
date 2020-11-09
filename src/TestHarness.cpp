/*
	TestHarness.cpp

	This file contains the implementation of the TestHarness class.
	Fixture that is responsible for running a series of tests and invoking the logging mechanism.
*/

#include "TestHarness.h"
#include <iostream>

TestHarness::~TestHarness() {
	// Delete logging since it was created with the new operator
	delete logging;
}

void TestHarness::runTestSequence()
{
	// Iterate through the test container.
	for (auto test : tests)
		runTest(test);
}

void TestHarness::addTest(ITest* test)
{
	tests.push_back(test);
}


void TestHarness::log(TestResult result)
{
	// Appropriate log level functionality is invoked polymorphically.
	logging->DisplayResult(result);
}

void TestHarness::runTest(ITest* test)
{
	TestResult result;

	try
	{
		// Set the test name
		result.setTestName(test->getTestName());

		// Record start time and date.
		result.recordStartTime();
		// Run test and record results.
		result.setIsSuccessful(test->run());
		// Record end time and date.
		result.recordEndTime();

		// Set message depending on PASS/FAIL.
		if (result.getIsSuccessful())
			result.setMessage("Test successful");
		else
			result.setMessage(test->getErrorMessage());

		// Log result to console.
		log(result);
	}
	catch (std::exception &e)
	{
		// Exception has been thrown. Set test as unsuccessful, set exception message, and log results to console.
		result.setIsSuccessful(false);
		result.setMessage("Test threw exception: ");
		log(result);
	}
	catch (...) {
		// Exception has been thrown. Set test as unsuccessful, set exception message, and log results to console.
		result.setIsSuccessful(false);
		result.setMessage("Test threw default exception: ");
		log(result);
	}

	std::cout << std::endl;
}