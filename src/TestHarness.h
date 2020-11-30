/*
	TestHarness.h

	This file contains the declaration of the TestHarness class.
	Fixture that is responsible for running a series of tests and invoking the logging mechanism.
*/

#pragma once

#include "ITest.h"
#include "TestResult.h"
#include "Logging.h"
#include <vector>
#include "Sockets.h"
#include "Message.h"
#include "Comm.h"

using std::vector;

/**
* Test harness used to run all tests and display results
**/
class TestHarness
{
public:
	/**
	* Constructor to create a new TestHarness instance
	*
	* @log[in] - pointer to logging abstraction to be used for logging
	**/
	TestHarness(Logging* log);

	/**
	* Constructor to create a new TestHarness instance
	*
	* @log[in] - pointer to logging abstraction to be used for logging
	* @tests[in] - vectors of tests to be run in parralel
	**/
	TestHarness(Logging* log, vector<ITest*> tests);

	/**
	* Destructor
	**/
	~TestHarness();

	/**
	* Runs all of the tests added to the test harness
	**/
	void runTestSequence();

	/**
	* Adds a new test to the test harness
	*
	* @test[in] - the new test to be added
	**/
	void addTest(ITest* test);

private:
	/**
	* Logs a new test result message
	*
	* @result[in] - data associated with a test result
	**/
	void log(TestResult result);

	/**
	* Creates a new child thread
	*
	* @port[in] - port number for the child thread communication end point
	**/
	void childThread(int port);

	/**
	* Runs a single test
	*
	* @test[in] - test to be run
	**/
	void runTest(ITest* test);

	// Collection of tests that are part of this test harness
	vector<ITest*> tests;
	// Pointer to logging abstraction that was injected in
	Logging* logging;

	// the ready queue of threads that are available to run tests
	BlockingQueue<int> ready;
	// the queue of tests that need to be run
	BlockingQueue<int> testIds;

	// temporary - used to store times for sleeping to demonstrate
	//	tests beuing run in parallel
	BlockingQueue<int> sleepTimes;
};