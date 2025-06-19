#include "../core/Math.h"
#include "../math/AbcdBeamCalculator.h"
#include "../tests/TestUtils.h"

#include "testing/OriTestBase.h"

// Expected values for these tests calculated by `test_files/test_plot_funcs.rez`
// with help of func 'Round-trip Matrix' and 'Beam Parameters at Element'.

namespace Z {
namespace Tests {
namespace AbcdBeamCalculatorTests {

#define CALCULATOR \
    AbcdBeamCalculator calc(1e-6);

#define ASSERT_IS_STABLE(m) {\
    double p = (m.A.real() + m.D.real()) / 2; \
    ASSERT_IS_TRUE(p >= -1 && p <= 1) \
}

#define ASSERT_IS_UNSTABLE(m) {\
    double p = (m.A.real() + m.D.real()) / 2; \
    ASSERT_IS_TRUE(p < -1 || p > 1) \
}

//------------------------------------------------------------------------------

TEST_METHOD(real_beamRadius__must_return_nan_when_unstable)
{
    CALCULATOR
    Z::Matrix m(-0.577181785, -0.511906815, 24.6309496, 20.1128159);
    ASSERT_IS_UNSTABLE(m)
    ASSERT_IS_TRUE(std::isnan(calc.beamRadius(m, 1)))
}

TEST_METHOD(real_beamRadius)
{
    CALCULATOR
    Z::Matrix m(1.22740581, 0.933155617, -5.17573851, -3.12021454);
    ASSERT_IS_STABLE(m)
    ASSERT_NEAR_DBL(calc.beamRadius(m, 1), 958.984241e-6, 1e-10)
}

TEST_METHOD(real_frontRadius__can_return_inf)
{
    CALCULATOR

    double r1 = calc.frontRadius({1, 1, 0, 1}, 1);
    TEST_LOG_VALUE(r1)
    ASSERT_IS_TRUE(std::isinf(r1))
    ASSERT_IS_TRUE(r1 > 0)

    double r2 = calc.frontRadius({1, -1, 0, 1}, 1);
    TEST_LOG_VALUE(r2)
    ASSERT_IS_TRUE(std::isinf(r2))
    ASSERT_IS_TRUE(r2 < 0)
}

TEST_METHOD(real_frontRadius)
{
    CALCULATOR
    Z::Matrix m(1.22740581, 0.933155617, -5.17573851, -3.12021454);
    ASSERT_IS_STABLE(m)
    ASSERT_NEAR_DBL(calc.frontRadius(m, 1), -0.429271897, 1e-9)
}

TEST_METHOD(real_halfAngle__must_return_nan_when_unstable)
{
    CALCULATOR
    Z::Matrix m(-0.577181785, -0.511906815, 24.6309496, 20.1128159);
    ASSERT_IS_UNSTABLE(m)
    ASSERT_IS_TRUE(std::isnan(calc.halfAngle(m, 1)))
}

TEST_METHOD(real_halfAngle)
{
    CALCULATOR
    Z::Matrix m(1.22740581, 0.933155617, -5.17573851, -3.12021454);
    ASSERT_IS_STABLE(m)
    ASSERT_NEAR_DBL(calc.halfAngle(m, 1), (0.129402667_deg).toSi(), 1e-10)
}

TEST_METHOD(real_calc_ts)
{
    CALCULATOR
    Z::Matrix mt(1.22740581, 0.933155617, -5.17573851, -3.12021454);
    Z::Matrix ms(1.07321435, 0.805228946, -1.57668319, -0.251199535);
    ASSERT_IS_STABLE(mt)
    ASSERT_IS_STABLE(ms)
    ASSERT_NEAR_TS(calc.beamRadius(mt, ms, 1), 958.984241e-6, 530.243033e-6, 1e-10)
    ASSERT_NEAR_TS(calc.frontRadius(mt, ms, 1), -0.429271897, -1.21597781, 1e-8)
    ASSERT_NEAR_TS(calc.halfAngle(mt, ms, 1), (0.129402667_deg).toSi(), (0.0425118626_deg).toSi(), 1e-10)
}

//------------------------------------------------------------------------------

TEST_GROUP("AbcdBeamCalculator",
           ADD_TEST(real_beamRadius__must_return_nan_when_unstable),
           ADD_TEST(real_beamRadius),
           ADD_TEST(real_frontRadius__can_return_inf),
           ADD_TEST(real_frontRadius),
           ADD_TEST(real_halfAngle__must_return_nan_when_unstable),
           ADD_TEST(real_halfAngle),
           ADD_TEST(real_calc_ts),
           // TODO: add tests for calculation inside a medium
           )

} // namespace AbcdBeamCalculatorTests
} // namespace Tests
} // namespace Z
