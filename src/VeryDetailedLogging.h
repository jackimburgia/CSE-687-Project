/*
    VeryDetailedLogging.h

    This file contains the declaration and implementation of the VeryDetailedLogging class.
    Concrete class that derives from Logging abstract class.
    Logs PASS/FAIL, test specific message, start and end date/time, and execution duration to console.

*/

#ifndef VeryDetailedLogging_h
#define VeryDetailedLogging_h

#include "TestResult.h"
#include "Logging.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

/**
* Displays a very detailed test result
**/
class VeryDetailedLogging : public Logging
{
public:
    VeryDetailedLogging();

    void DisplayResult(TestResult& result) override;
};

#endif /* VeryDetailedLogging_h */
