#include "testing/OriTestBase.h"
#include "../funcs/GaussCalculator.h"

namespace Z {
namespace Tests {
namespace GaussCalculatorTests {

#define _(p, value) ASSERT_NEAR_DBL(c.p(), value, 1e-7)
#define __(p, value, epsilon) ASSERT_NEAR_DBL(c.p(), value, epsilon)

TEST_METHOD(change_W0)
{
    GaussCalculator c;
    c.setZone(GaussCalculator::Zone::Far);
    c.setM2(2);
    c.setLambda(980e-9);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setW0(100e-6);
    c.calc();

    _(M2, 2)
    __(lambda, 9.8e-07, 1e-12)
    _(w0, 0.0001)
    _(z0, 0.01602853)
    _(Vs, 0.006238874)
    _(z, 0.1)
    _(w, 0.0006318508)
    _(R, 0.1025691)
    __(reQ1, 9.749521, 1e-6)
    _(imQ1, 0.7813527)
    _(reQ, 0.1019146)
    _(imQ, -0.008167705)

    c.setLock(GaussCalculator::Lock::Front);
    c.setW0(75e-6);
    c.calc();

    __(M2, 1.49164, 1e-5)
    __(lambda, 9.8e-07, 1e-12)
    _(w0, 7.5e-05)
    _(z0, 0.01208877)
    _(Vs, 0.006204104)
    _(z, 0.101124)
    _(w, 0.0006318508)
    _(R, 0.1025691)
    __(reQ1, 9.749521, 1e-6)
    _(imQ1, 0.7813527)
    _(reQ, 0.1019146)
    _(imQ, -0.008167705)
}

TEST_METHOD(change_Z0)
{
    GaussCalculator c;
    c.setZone(GaussCalculator::Zone::Far);
    c.setLambda(1.1e-6);
    c.setW0(100e-6);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setZ0(0.012);
    c.calc();

    __(M2, 2.379994, 1e-5)
    __(lambda, 1.1e-06, 1e-12)
    _(w0, 0.0001)
    _(z0, 0.012)
    _(Vs, 0.008333333)
    _(z, 0.1)
    _(w, 0.0008393119)
    _(R, 0.10144)
    __(reQ1, 9.858044, 1e-6)
    _(imQ1, 0.4970454)
    _(reQ, 0.1011828)
    _(imQ, -0.005101664)

    c.setLock(GaussCalculator::Lock::Front);
    c.setZ0(0.016);

    c.setZone(GaussCalculator::Zone::Far);
    c.calc();

    __(M2, 3.210236, 1e-6)
    __(lambda, 1.1e-06, 1e-12)
    _(w0, 0.0001341065)
    _(z0, 0.016)
    _(Vs, 0.008381658)
    _(z, 0.09885022)
    _(w, 0.0008393119)
    _(R, 0.10144)
    __(reQ1, 9.858044, 1e-6)
    _(imQ1, 0.4970454)
    _(reQ, 0.1011828)
    _(imQ, -0.005101664)

    c.setZone(GaussCalculator::Zone::Near);
    c.calc();

    __(M2, 122.5328, 1e-4)
    __(lambda, 1.1e-06, 1e-12)
    _(w0, 0.0008285287)
    _(z0, 0.016)
    _(Vs, 0.05178305)
    _(z, 0.002589777)
    _(w, 0.0008393119)
    _(R, 0.10144)
    __(reQ1, 9.858044, 1e-6)
    _(imQ1, 0.4970454)
    _(reQ, 0.1011828)
    _(imQ, -0.005101664)
}

TEST_METHOD(change_Vs)
{
    GaussCalculator c;
    c.setZone(GaussCalculator::Zone::Far);
    c.setLambda(640e-9);
    c.setW0(100e-6);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setVs(0.07);
    c.calc();

    __(M2, 34.36117, 1e-5)
    __(lambda, 6.4e-07, 1e-12)
    _(w0, 0.0001)
    _(z0, 0.001428571)
    _(Vs, 0.07)
    _(z, 0.1)
    _(w, 0.007000714)
    _(R, 0.1000204)
    __(reQ1, 9.99796, 1e-6)
    _(imQ1, 0.004156669)
    _(reQ, 0.1000204)
    __(imQ, -4.158365e-05, 1e-9)

    c.setLock(GaussCalculator::Lock::Front);
    c.setVs(0.08);
    c.calc();

    __(M2, 1331.382, 1e-3)
    __(lambda, 6.4e-07, 1e-12)
    _(w0, 0.003390336)
    _(z0, 0.04237919)
    _(Vs, 0.08)
    _(z, 0.0765625)
    _(w, 0.007000714)
    _(R, 0.1000204)
    __(reQ1, 9.99796, 1e-6)
    _(imQ1, 0.004156669)
    _(reQ, 0.1000204)
    __(imQ, -4.158365e-05, 1e-9)
}

//------------------------------------------------------------------------------

TEST_GROUP("BeamCalculator",
           ADD_TEST(change_W0),
           ADD_TEST(change_Z0),
           ADD_TEST(change_Vs),
           )

} // namespace GaussCalculatorTests
} // namespace Tests
} // namespace Z
