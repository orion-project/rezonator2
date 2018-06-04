#include "../core/Math.h"
#include "../core/Pump.h"
#include "../funcs/PumpCalculator.h"
#include "testing/OriTestBase.h"

// Test values are produced by script $PROJECT/calc/PumpCalculator.py

using namespace Z::Units;

namespace Z {
namespace Tests {
namespace PumpCalculatorTests {

TEST_METHOD(Waist_gauss)
{
    PumpParams_Waist p;
    p.waist()->setValue(150_mkm);
    p.distance()->setValue(110_mm);
    p.MI()->setValue(1);
    const double lambda = 980e-9;

    auto calc = PumpCalculator::T();
    ASSERT_IS_TRUE(calc->init(&p, lambda));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0002735517, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.1572955, 1e-7)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.002079625, 1e-9)

    // Output beam
    beam = calc->calc({ -1.4379022, 0.1681906, -10.1543000, 0.4922850 }, lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0002166898, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.1557644, 1e-7)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.002001918, 1e-9)
}

TEST_METHOD(Waist_hyper)
{
    PumpParams_Waist p;
    p.waist()->setValue(100_mkm);
    p.distance()->setValue(10_cm);
    p.MI()->setValue(3);
    const double lambda = 980e-9;

    auto calc = PumpCalculator::S();
    ASSERT_IS_TRUE(calc->init(&p, lambda));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0009411587, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.1011418, 1e-7)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.009358311, 1e-9)

    // Output beam
    beam = calc->calc({ -1.3899498, 0.1731843, -9.8480800, 0.5075960 }, lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0003488415, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, -0.09418758, 1e-8)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.004573193, 1e-9)
}

TEST_METHOD(Front_gauss)
{
    PumpParams_Front p;
    p.beamRadius()->setValue(400_mkm);
    p.frontRadius()->setValue(120_mm);
    p.MI()->setValue(1);
    const double lambda = 980e-9;

    auto calc = PumpCalculator::T();
    ASSERT_IS_TRUE(calc->init(&p, lambda));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0004, 1e-9)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.12, 1e-9)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.003423345, 1e-9)

    // Output beam
    beam = calc->calc({ -1.4379022, 0.1681906, -10.1543000, 0.4922850 }, lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0001319668, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.203642, 1e-6)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.002451023, 1e-9)
}

TEST_METHOD(Front_hyper)
{
    PumpParams_Front p;
    p.beamRadius()->setValue(0.3_mm);
    p.frontRadius()->setValue(11_cm);
    p.MI()->setValue(3);
    const double lambda = 980e-9;

    auto calc = PumpCalculator::T();
    ASSERT_IS_TRUE(calc->init(&p, lambda));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0003, 1e-9)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.11, 1e-9)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.004143537, 1e-9)

    // Output beam
    beam = calc->calc({ -1.3899498, 0.1731843, -9.8480800, 0.5075960 }, lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0005430641, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.3837397, 1e-7)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.002229871, 1e-9)
}

TEST_METHOD(RayVector)
{
    PumpParams_RayVector p;
    p.radius()->setValue(1.5_mm);
    p.angle()->setValue(5_deg);
    p.distance()->setValue(10_cm);

    auto calc = PumpCalculator::T();
    ASSERT_IS_TRUE(calc->init(&p, 0));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), 0);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.01024887, 1e-8)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.08726646, 1e-8)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.1175926, 1e-7)

    // Output beam
    beam = calc->calc({ -1.4379022, 0.1681906, -10.1543000, 0.4922850 }, 0);
    ASSERT_NEAR_DBL(beam.beamRadius, -5.946877e-05, 1e-10)
    ASSERT_NEAR_DBL(beam.halfAngle, -0.06111009, 1e-8)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.0009737475, 1e-10)
}

TEST_METHOD(TwoSections)
{
    PumpParams_TwoSections p;
    p.radius1()->setValue(1.5_mm);
    p.radius2()->setValue(2.5_mm);
    p.distance()->setValue(11_cm);

    auto calc = PumpCalculator::S();
    ASSERT_IS_TRUE(calc->init(&p, 0));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), 0);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0025, 1e-8)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.009090659, 1e-8)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.2750114, 1e-7)

    // Output beam
    beam = calc->calc({ -1.3899498, 0.1731843, -9.8480800, 0.5075960 }, 0);
    ASSERT_NEAR_DBL(beam.beamRadius, -0.001900515, 1e-9)
    ASSERT_NEAR_DBL(beam.halfAngle, -0.02000582, 1e-8)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.09500446, 1e-8)
}

TEST_METHOD(Complex_gauss)
{
    PumpParams_Complex p;
    p.real()->setValue(0.1_m);
    p.imag()->setValue(-0.0320571_m);
    p.MI()->setValue(1);
    const double lambda = 980e-9;

    auto calc = PumpCalculator::T();
    ASSERT_IS_TRUE(calc->init(&p, lambda));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0003275802, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.1102766, 1e-7)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.003119435, 1e-9)

    // Output beam
    beam = calc->calc({ -1.4379022, 0.1681906, -10.1543000, 0.4922850 }, lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0001626936, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 1.214461, 1e-6)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.001922044, 1e-9)
}

TEST_METHOD(Complex_hyper)
{
    PumpParams_Complex p;
    p.real()->setValue(0.101014_m);
    p.imag()->setValue(-0.003598_m);
    p.MI()->setValue(3);
    const double lambda = 980e-9;

    auto calc = PumpCalculator::T();
    ASSERT_IS_TRUE(calc->init(&p, lambda));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0009411621, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.1011422, 1e-7)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.009358314, 1e-9)

    // Output beam
    beam = calc->calc({ -1.3899498, 0.1731843, -9.8480800, 0.5075960 }, lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0003488377, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, -0.09418615, 1e-8)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.004573224, 1e-9)
}

TEST_METHOD(InvComplex_gauss)
{
    PumpParams_InvComplex p;
    p.real()->setValue(9.06811_m);
    p.imag()->setValue(2.90697_m);
    p.MI()->setValue(1);
    const double lambda = 980e-9;

    auto calc = PumpCalculator::T();
    ASSERT_IS_TRUE(calc->init(&p, lambda));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0003275803, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.1102766, 1e-7)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.003119437, 1e-9)

    // Output beam
    beam = calc->calc({ -1.4379022, 0.1681906, -10.1543000, 0.4922850 }, lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0001626935, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 1.214473, 1e-6)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.001922044, 1e-9)
}

TEST_METHOD(InvComplex_hyper)
{
    PumpParams_InvComplex p;
    p.real()->setValue(9.88711_m);
    p.imag()->setValue(0.352168_m);
    p.MI()->setValue(3);
    const double lambda = 980e-9;

    auto calc = PumpCalculator::T();
    ASSERT_IS_TRUE(calc->init(&p, lambda));

    // Beam at schema entrance
    BeamResult beam = calc->calc(Matrix(), lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0009411593, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, 0.1011418, 1e-7)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.009358321, 1e-9)

    // Output beam
    beam = calc->calc({ -1.3899498, 0.1731843, -9.8480800, 0.5075960 }, lambda);
    ASSERT_NEAR_DBL(beam.beamRadius, 0.0003488423, 1e-10)
    ASSERT_NEAR_DBL(beam.frontRadius, -0.09418767, 1e-8)
    ASSERT_NEAR_DBL(beam.halfAngle, 0.004573194, 1e-9)
}

//------------------------------------------------------------------------------

TEST_GROUP("Pump Calculator",
           ADD_TEST(Waist_gauss),
           ADD_TEST(Waist_hyper),
           ADD_TEST(Front_gauss),
           ADD_TEST(Front_hyper),
           ADD_TEST(RayVector),
           ADD_TEST(TwoSections),
           ADD_TEST(Complex_gauss),
           ADD_TEST(Complex_hyper),
           ADD_TEST(InvComplex_gauss),
           ADD_TEST(InvComplex_hyper),
           )

} // namespace PumpCalculatorTests
} // namespace Tests
} // namespace Z
