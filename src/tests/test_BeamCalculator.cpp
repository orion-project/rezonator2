#include "testing/OriTestBase.h"
#include "../funcs/BeamCalculator.h"

// Test values are produced by script $PROJECT/calc/BeamCalculator.py

namespace Z {
namespace Tests {
namespace BeamCalculatorTests {

TEST_METHOD(calcVector)
{
    RayVector ray(0.01, 0.05);
    Matrix matrix(-1.4379022, 0.1681906, -10.1543000, 0.4922850);
    auto beam = BeamCalculator::calcVector(ray, matrix);
    ASSERT_NEAR_DBL(beam.beamRadius, -0.005969492, 1e-9)
    ASSERT_NEAR_DBL(beam.halfAngle, -0.07692875, 1e-8)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.07767426, 1e-8)
}

TEST_METHOD(calcGauss)
{
    Complex q(0.1019146, -0.008167705);
    const double lambda = 980e-9;
    const double MI = 2;
    Matrix matrix(-1.3899498, 0.1731843, -9.8480800, 0.5075960);
    auto beam = BeamCalculator::calcGauss(q, matrix, lambda, MI);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0002070907, 1e-10)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.004056169, 1e-9)
    ASSERT_NEAR_DBL(beam.frontRadius, -0.07624884, 1e-8)
}

//------------------------------------------------------------------------------

TEST_GROUP("Beam Calculator",
   ADD_TEST(calcVector),
   ADD_TEST(calcGauss),
)

} // namespace BeamCalculatorTests
} // namespace Tests
} // namespace Z
