/*
	ITest.h

	This file contains the declaration and implementation of the ITest class.
	Abstract base class. Derived classes of this interface provide an 
	infrastructure for passing tests to the harness.

*/

#pragma once

#include <string>
using std::string;

/**
* Abstracion of a test used to return 
**/
class ITest
{
public:
	/**
	* Base constructor used to create new instances that derive from ITest
	*
	* @tstName[in] - name of the test
	* @errMsg[in] - error message for the test
	**/
	ITest(string tstName, string errMsg) : testName(tstName), errorMessage(errMsg) {}

	/**
	* Destructor is virtual to ensure base class destructors are called
	**/
	virtual ~ITest() {  }

	/**
	* Suppress copying for now
	**/
	ITest(const ITest&) = delete;
	ITest& operator=(const ITest&) = delete;

	/**
	* Returns error message for this test
	**/
	string getErrorMessage() const { return errorMessage; }

	/**
	* Returns name of the test
	**/
	string getTestName() const { return testName; }

	/**
	* Runs this test
	* Virtual function to be overriden by derived classes.
	**/
	virtual bool run() = 0;
private:
	string errorMessage;
	string testName;
};