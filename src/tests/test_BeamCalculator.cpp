#include "testing/OriTestBase.h"
#include "../funcs/BeamCalculator.h"

// Test values are produced by script $PROJECT/calc/BeamCalculator.py

namespace Z {
namespace Tests {
namespace BeamCalculatorTests {

TEST_METHOD(calcVector)
{
    // TODO
}

TEST_METHOD(calcGauss)
{
    // TODO
}

//------------------------------------------------------------------------------

TEST_GROUP("Beam Calculator",
           ADD_TEST(calcVector),
           ADD_TEST(calcGauss),
           )

} // namespace BeamCalculatorTests
} // namespace Tests
} // namespace Z
