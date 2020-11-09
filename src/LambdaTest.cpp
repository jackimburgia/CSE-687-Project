#include "LambdaTest.h"

LambdaTest::LambdaTest(const std::function<bool()> getR, string tstName, const string errMsg) : ITest(tstName, errMsg), getRes(getR) {};

bool LambdaTest::run() { return getRes(); }