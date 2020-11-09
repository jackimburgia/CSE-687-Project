#include "FunctionPointerTest.h"

FunctionPointerTest::FunctionPointerTest(bool (*getR)(), string tstName, const string errMsg) : ITest(tstName, errMsg), getRes(getR) {}

bool FunctionPointerTest::run() { return getRes(); }