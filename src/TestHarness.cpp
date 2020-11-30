/*
	TestHarness.cpp

	This file contains the implementation of the TestHarness class.
	Fixture that is responsible for running a series of tests and invoking the logging mechanism.
*/

#include "TestHarness.h"
#include <iostream>


using std::thread;
using namespace Sockets;
using namespace MsgPassingCommunication;
using std::cout;
using std::endl;

TestHarness::TestHarness(Logging* log) : logging(log) {}

TestHarness::TestHarness(Logging* log, vector<ITest*> tst) : logging(log), tests(tst) {

	// for demonstration, create a queue of ints that will be used 
	//	make each test sleep to demonstrate that the tests are running
	//	in parallel
	for (auto x : { 4,1,2,3,5,4,3,3,3,3,1,6 })sleepTimes.enQ(x);

	SocketSystem ss;
	EndPoint queueManagerEP("localhost", 9191);

	// create the queue manager thread that will listen for messages
	//	and add items to the appropriate queue
	thread queueManager([&]() {
		Comm queueManagerComm(queueManagerEP, "listener");
		queueManagerComm.start();

		while (true)
		{
			auto msg = queueManagerComm.getMessage();

			if (msg.name() == "ready") // from child threads
			{
				int endPointId = std::stoi(msg.body());
				ready.enQ(endPointId);
			}
			else if (msg.name() == "testrequest") // from 
			{
				int testId = std::stoi(msg.body());
				testIds.enQ(testId);
			}
		}
		});


	// creates messages for all of the tests that have been requested
	thread testManager([&]() {
		EndPoint testManagerEP("localhost", 9193);
		Comm testManagerComm(testManagerEP, "server");
		testManagerComm.start();

		for (int x = 0; x < tests.size(); x++) {
			Message msg;
			msg.to(queueManagerEP);
			msg.from(testManagerEP);
			msg.name("testrequest");
			msg.body(std::to_string(x));
			testManagerComm.postMessage(msg);
		}
		});

	// Dequeues threads and tests and sends
	thread testDispatcher([&]() {
		EndPoint testDispatcherEP("localhost", 9192);
		Comm testDispatcherComm(testDispatcherEP, "server");
		testDispatcherComm.start();

		while (true) {
			int threadId = ready.deQ(); // portid
			int testId = testIds.deQ();

			// send message to to the thread to run test
			EndPoint toEP("localhost", threadId);

			Message msg;
			msg.to(toEP);
			msg.from(testDispatcherEP);
			msg.name("runtest");
			msg.body(std::to_string(testId));
			testDispatcherComm.postMessage(msg);
		}
		});


	// create the child threads that will run the tests
	thread child1(&TestHarness::childThread, this, 9194);
	thread child2(&TestHarness::childThread, this, 9195);


	queueManager.join();
	testManager.join();
	testDispatcher.join();
	child1.join();
	child2.join();

}

// creates a child thread that runs tests
void TestHarness::childThread(int port) {
	EndPoint queueManagerEP("localhost", 9191);
	EndPoint childEP("localhost", port);
	Comm childComm(childEP, "server");
	childComm.start();

	Message msg;
	msg.to(queueManagerEP);
	msg.from(childEP);
	msg.name("ready");
	msg.body(std::to_string(port));
	childComm.postMessage(msg);

	while (true)
	{
		msg = childComm.getMessage();

		// get the test id that this thread should run
		int testId = std::stoi(msg.body());
		// run the test
		auto test = tests[testId];
		runTest(test);

		// Send a message back to the queue manager that the thread is ready 
		msg.to(queueManagerEP);
		msg.from(childEP);
		msg.name("ready");
		msg.body(std::to_string(port));
		childComm.postMessage(msg);
	}
}


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


		// TODO - remove; make the test sleep a random amount of time
		int x = sleepTimes.deQ();
		cout << "Run test, Sleep for " << x << endl;
		std::this_thread::sleep_for(std::chrono::seconds(x));


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