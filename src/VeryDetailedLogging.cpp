#include "VeryDetailedLogging.h"

VeryDetailedLogging::VeryDetailedLogging() : Logging(LoggingLevel::VERY_DETAILED) {}

void VeryDetailedLogging::DisplayResult(TestResult& result) 
{
    // Log test name
    cout << result.getTestName() << endl;
    // Log PASS/FAIL.
    cout << SuccessValue(result.getIsSuccessful()) << endl;
    // Log test specific message.
    cout << result.getMessage() << endl;
    // Log start date and time.
    cout << "Function Start Date and Time: " << result.getFunctionStartDateTime();
    // Log end date and time.
    cout << "Function End Date and Time: " << result.getFunctionEndDateTime();
    // Log execution duration.
    cout << "Total Function Run Time: " << std::fixed << result.getFunctionExecutionTime() << std::setprecision(5);
    cout << " sec(s)" << endl;
}