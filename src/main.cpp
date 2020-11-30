/*
    main.cpp

    Responsible for instantiating the tests, logging, and harness.
*/

#include "TestHarness.h"
#include "Logging.h"
#include "BasicLogging.h"
#include "DetailedLogging.h"
#include "VeryDetailedLogging.h"
#include "LambdaTest.h"
#include "FunctionPointerTest.h"

#ifdef _WIN32
#include "Windows.h"
#endif

namespace mainFunctions {

    // Function that returns whether the test value is greater than 100
    bool is_greater_than_100() {
        int val1 = 76;
        bool isGreaterThan = val1 > 100;
        return isGreaterThan;
    }

    // Function that returns whether the test value is greater than 50
    bool is_greater_than_50() {
        int val1 = 76;
        bool isGreaterThan = val1 > 50;
        return isGreaterThan;
    }

    // Functor that returns whether a test value is betweem 10 and 30
    class is_between_10_and_30_Functor {
    public:
        bool operator()() {
            int val1 = 34;
            bool isBetween = val1 >= 10 && val1 <= 30;
            return isBetween;
        }
    };

    // Functor that returns whether a test value is betweem 5 and 80
    class is_between_5_and_80_Functor {
    public:
        bool operator()() {
            int val1 = 34;
            bool isBetween = val1 >= 5 && val1 <= 80;
            return isBetween;
        }
    };
}

using namespace mainFunctions;


int main()
{
    // Create series of test functions.

    // 1. Lambda test - will FAIL
    std::function<bool()> are_values_equal_1 = []() {
        int val1 = 11;
        int val2 = 33;

#ifdef _WIN32
        //Sleep(5000);
#endif
        bool isEqual = val1 == val2;


        return isEqual;
    };
    LambdaTest test1(are_values_equal_1, "Are values equal", "Lambda test failed! val1 and val2 are not equal!");

    // 2. Function pointer test - will FAIL
    bool (*is_greater_than_100_Ptr)() = is_greater_than_100;
    FunctionPointerTest test2(is_greater_than_100_Ptr, "Is greater than 100", "Function pointer test failed! val1 is not greater than 100!");


    // 3. Functor test - will FAIL
    is_between_10_and_30_Functor is_between_10_and_30;
    LambdaTest test3(is_between_10_and_30, "Is between 10 and 30", "Functor test failed! val1 is not between 10 and 30!");


    // 4. Lamdba test - will PASS
    std::function<bool()> are_values_equal_2 = []() {
        int val1 = 78;
        int val2 = 78;

        bool isEqual = val1 == val2;
        return isEqual;
    };
    LambdaTest test4(are_values_equal_2, "Are vaues equal", "Lambda test failed! val1 and val2 are not equal!");


    // 5. Function pointer test - will PASS
    bool (*is_greater_than_50_Ptr)() = is_greater_than_50;
    FunctionPointerTest test5(is_greater_than_50_Ptr, "Is greater than 50", "Function pointer test failed! val1 is not greater than 50!");


    // 6. Functor test - will PASS
    is_between_5_and_80_Functor is_between_5_and_80;
    LambdaTest test6(is_between_5_and_80, "Is between 5 and 50", "Functor test failed! val1 is not between 5 and 80!");


    // 7. Test will generate an exception - WILL FAIL
    std::function<bool()> division_test = []() {
        throw 20;
        int num1 = 350;
        int num2 = 0;
        int result = num1 / num2;
        bool isDivisionOk = result == 55;
        return isDivisionOk;
    };
    LambdaTest test7(division_test, "Division test", "Lambda test failed! result does not equal 55!");

    // create the logging
    LoggingLevel logLvl = LoggingLevel::VERY_DETAILED;
    Logging* logging = nullptr;

    switch (logLvl)
    {
    case LoggingLevel::DETAILED:
        logging = new DetailedLogging;
        break;
    case LoggingLevel::VERY_DETAILED:
        logging = new VeryDetailedLogging;
        break;
    case LoggingLevel::BASIC:
    default:
        logging = new BasicLogging;
        break;
    }


    // create a vector of tests to be performed
    vector<ITest*> tests;
    tests.push_back(&test1);
    tests.push_back(&test2);
    tests.push_back(&test3);
    tests.push_back(&test4);
    tests.push_back(&test5);
    tests.push_back(&test6);
    tests.push_back(&test7);


    // create the test harness
    // use dependency injection to inject the logging
    TestHarness testHarness(logging, tests);

    return 0;
}
