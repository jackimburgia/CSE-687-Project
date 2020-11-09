#include "Logging.h"

Logging::Logging(LoggingLevel lvl) : logLevel(lvl) {}

Logging::~Logging() { }

LoggingLevel Logging::getLoggingLevel() const { return logLevel; }

string Logging::SuccessValue(bool isSuccessful)
{
    return isSuccessful ? "PASS" : "FAIL";
}