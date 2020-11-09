/*
	FunctionPointerTest.h

	This file contains the declaration and implementation of the FunctionPointerTest class.
	Derives from ITest interface class. Provides an infrastructure for passing tests to the
	test harness via function pointer.
*/

#pragma once

#include "ITest.h"

/**
* Runs a function pointer as a test
**/
class FunctionPointerTest : public ITest
{
public:
	FunctionPointerTest(bool (*getR)(), string tstName, const string errMsg);

	// Overrides abstract base class function.
	bool run() override;
private:
	// function pointer to run as test
	bool (*getRes)();
};
