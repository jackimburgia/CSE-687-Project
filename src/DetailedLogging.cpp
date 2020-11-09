#include "DetailedLogging.h"

DetailedLogging::DetailedLogging() : Logging(LoggingLevel::DETAILED) {}

void DetailedLogging::DisplayResult(TestResult& result)
{
    // Log test name
    cout << result.getTestName() << endl;
    // Log PASS/FAIL to console.
    cout << SuccessValue(result.getIsSuccessful()) <<endl;
    // Log test specific message to console.
    cout << result.getMessage() << endl;
    
}