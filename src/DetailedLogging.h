/*
    DetailedLogging.h

    This file contains the declaration and implementation of the DetailedLogging class.
    Concrete class that derives from Logging abstract class. Logs PASS/FAIL results and 
    test specific message to console.

*/

#ifndef DetailedLogging_h
#define DetailedLogging_h

#include "TestResult.h"
#include "Logging.h"

#include <iostream>

using std::cout;
using std::endl;

/**
* Displays a Detailed test result
**/
class DetailedLogging : public Logging
{

public:
    DetailedLogging();

    void DisplayResult(TestResult& result) override;
};

#endif /* DetailedLogging_h */
