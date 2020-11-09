/*
    Logging.h

    This file contains the declaration and implementation of the Logging class.
    Abstract base class. Derived classes must provide specific functionality
    related to a particular logging level.
*/

#ifndef Logging_h
#define Logging_h

#include "TestResult.h"
#include <string>

using std::string;

/**
* The amount of detail to be displayed in the log message
**/
enum class LoggingLevel
{
    BASIC,
    DETAILED,
    VERY_DETAILED
};

/**
* Abstraction of Logging a TestResult
**/
class Logging{
    
public:
    /**
    * Base constructor used to create new instances that derive from Logging
    *
    * @lvl[in] - the level of the logging message to be displayed
    **/
    Logging(LoggingLevel lvl);

    /**
    * Destructor is virtual to ensure base class destructors are called
    **/
    virtual ~Logging();

    /**
    * Displays the formatted results of the test
    *
    * @result[in] - test result to be displayed
    **/
    virtual void DisplayResult(TestResult & result) = 0;

    /**
    * Returns the logging level 
    **/
    LoggingLevel getLoggingLevel() const;
    
protected:
    /**
    * Returns the success / fail message
    *
    * @isSuccessful[in] - whether a test is successful
    **/
    string SuccessValue(bool isSuccessful);

private:
    LoggingLevel logLevel;
};


#endif /* Logging_h */
