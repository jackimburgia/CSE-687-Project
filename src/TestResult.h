/*
    TestResult.h

    This file contains the declaration of the TestHarness class.
    Provides a container to store test execution data. Also, provides
    an interface to communicate result data to the logging mechanism.
*/

#ifndef TestResult_h
#define TestResult_h

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <chrono>

using std::string;

// Typedefs for simplified chrono library use.
typedef std::chrono::system_clock::time_point t_point;
typedef std::chrono::duration<double> duration;

/**
* Stores the data for a test result
**/
class TestResult {

public:
    /**
    * Constructor used to create a new instance of a test result
    **/
    TestResult();

    /**
    * Records the start time of the test
    **/
    void recordStartTime();

    /**
    * Records the end time of the test
    **/
    void recordEndTime();

    /**
    * Calculates the execution time of the test
    **/
    inline void calculateExecutionTime();
    
    /**
    * Setter for the message
    **/
    void setMessage(const string);

    /**
    * Getter for the message
    **/
    string getMessage() const;    
    
    /**
    * Setter for the test name
    **/
    void setTestName(const string tstName);

    /**
    * Getter for the test name
    **/
    string getTestName() const;
    
    /**
    * Setter for whether test is successul
    **/
    void setIsSuccessful(const bool);

    /**
    * Getter for whether test is successul
    **/
    bool getIsSuccessful() const;

    /**
    *  Getter for start time
    **/
    string getFunctionStartDateTime() const;

    /**
    * Getter for end time
    **/
    string getFunctionEndDateTime() const;

    /**
    * Getter for execution time
    **/
    double getFunctionExecutionTime() const;
    
private:
    bool successful;
    string message;
    string testName;
    t_point startTime;
    t_point endTime;
    duration executionTime;
    string functionStartDateTime;
    string functionEndDateTime;
};

#endif /* TestResult_h */
