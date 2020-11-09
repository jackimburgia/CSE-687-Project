/*
    TestResult.cpp

    This file contains the impementation of the TestResult class.
    Provides a container to store test execution data. Also, provides
    an interface to communicate result data to the logging mechanism.
*/

#include "TestResult.h"
#include <string>
#include <ctime>

using std::string;
using std::to_string;

#define MONTH_OFFSET 1
#define YEAR_OFFSET 1900

TestResult::TestResult() :
    testName(""), message(""), successful(false), startTime(std::chrono::system_clock::now()), endTime(std::chrono::system_clock::now()), executionTime(0), functionStartDateTime(""), functionEndDateTime("") {}

void TestResult::recordStartTime()
{
    // Grab system clock time.
    startTime = std::chrono::system_clock::now();
    // Convert to time_t type so we can use ctime library facilities.
    time_t startTime_t = std::chrono::system_clock::to_time_t(endTime);

    // Convert to date/time string.
    functionStartDateTime = std::ctime(&startTime_t);
}

void TestResult::recordEndTime()
{
    // Grab system clock time.
    endTime = std::chrono::system_clock::now();
    // Convert to time_t type so we can use ctime library facilities.
    time_t endTime_t = std::chrono::system_clock::to_time_t(endTime);

    // Convert to date/time string.
    functionEndDateTime = std::ctime(&endTime_t);

    // Calculate execution duration.
    calculateExecutionTime();
}

void TestResult::setMessage(const string msg)
{
    // message mutator
    message = msg;
}

string TestResult::getMessage() const
{
    // message accessor
    return message;
}

void TestResult::setTestName(const string name)
{
    // test name mutator
    testName = name;
}

string TestResult::getTestName() const
{
    // test name accessor
    return testName;
}

void TestResult::setIsSuccessful(const bool isSucc)
{
    // Function Pass/Fail mutator
    successful = isSucc;
}

bool TestResult::getIsSuccessful() const
{
    // Function Pass/Fail accessor
    return successful;
}

string TestResult::getFunctionStartDateTime() const
{
    // Function Start Date and Time accessor
    return functionStartDateTime;
}

string TestResult::getFunctionEndDateTime() const
{
    // Function End Date and Time accessor
    return functionEndDateTime;
}

inline void TestResult::calculateExecutionTime()
{
    //Calculate function execution time
    executionTime = endTime - startTime;
}

double TestResult::getFunctionExecutionTime() const
{
    // Function execution time accessor
    return executionTime.count();
}
