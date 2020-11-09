/*
	LambdaTest.h

	This file contains the declaration and implementation of the LambdaTest class.
	Derives from ITest interface class. Provides an infrastructure for passing tests to the
	test harness via lambda function.
*/

#pragma once

#include "ITest.h"
#include <functional>

/**
* Runs a lamdba or functor as a test
**/
class LambdaTest : public ITest
{
public:
	LambdaTest(const std::function<bool()> getR, string tstName, const string errMsg);

	bool run() override;
private:
	// function used to run the test
	std::function<bool()> getRes;
};