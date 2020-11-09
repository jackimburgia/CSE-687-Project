/*
    BasicLogging.h

    This file contains the declaration and implementation of the BasicLogging class.
    Concrete class that derives from Logging abstract class. Logs PASS/FAIL results to console.

*/

#ifndef BasicLogging_h
#define BasicLogging_h

#include "TestResult.h"
#include "Logging.h"

#include <iostream>

using std::cout;
using std::endl;

/**
* Displays a Basic test result
**/
class BasicLogging: public Logging
{
public:

    BasicLogging();

    void DisplayResult(TestResult& result) override;
};

#endif /* BasicLogging_h */
