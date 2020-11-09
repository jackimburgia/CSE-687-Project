#include "BasicLogging.h"

BasicLogging::BasicLogging() : Logging(LoggingLevel::BASIC) {}

void BasicLogging::DisplayResult(TestResult& result)
{
    // Log test name
    cout << result.getTestName() << endl;
    // Log PASS/FAIL to console.
    cout << SuccessValue(result.getIsSuccessful()) << " - " << endl;
}   