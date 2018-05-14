#include "testing/OriTestBase.h"
#include "../funcs/PumpCalculator.h"

// Test values are produced by script $PROJECT/calc/PumpCalculator.py

namespace Z {
namespace Tests {
namespace PumpCalculatorTests {

TEST_METHOD(calcFront_Waist)
{
    // TODO
}

TEST_METHOD(calcFront_Front)
{
    // TODO
}

TEST_METHOD(calcFront_RayVector)
{
    // TODO
}

TEST_METHOD(calcFront_TwoSections)
{
    // TODO
}

TEST_METHOD(calcFront_Complex)
{
    // TODO
}

TEST_METHOD(calcFront_InvComplex)
{
    // TODO
}

//------------------------------------------------------------------------------

TEST_GROUP("Pump Calculator",
           ADD_TEST(calcFront_Waist),
           ADD_TEST(calcFront_Front),
           ADD_TEST(calcFront_RayVector),
           ADD_TEST(calcFront_TwoSections),
           ADD_TEST(calcFront_Complex),
           ADD_TEST(calcFront_InvComplex),
           )

} // namespace PumpCalculatorTests
} // namespace Tests
} // namespace Z
