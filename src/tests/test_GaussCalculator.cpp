#include "testing/OriTestBase.h"
#include "../funcs/GaussCalculator.h"

namespace Z {
namespace Tests {
namespace GaussCalculatorTests {

#define _(p, value, order)\
{\
    constexpr double epsilon = std::pow(10, -order);\
    ASSERT_NEAR_DBL(c.p(), value, epsilon)\
}

TEST_METHOD(change_W0)
{
    GaussCalculator c;
    c.setM2(2);
    c.setLambda(980e-9);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setW0(100e-6);
    c.calc();

    _(M2, 2, 7)
    _(lambda, 9.8e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.01602853, 8)
    _(Vs, 0.006238874, 9)
    _(z, 0.1, 7)
    _(w, 0.0006318508, 9)
    _(R, 0.1025691, 7)
    _(reQ1, 9.749521, 6)
    _(imQ1, 0.7813527, 7)
    _(reQ, 0.1019146, 7)
    _(imQ, -0.008167705, 9)

    c.setLock(GaussCalculator::Lock::Front);
    c.setW0(75e-6);
    c.calc();

    _(M2, 1.49164, 5)
    _(lambda, 9.8e-07, 12)
    _(w0, 7.5e-05, 7)
    _(z0, 0.01208877, 8)
    _(Vs, 0.006204104, 9)
    _(z, 0.101124, 6)
    _(w, 0.0006318508, 10)
    _(R, 0.1025691, 7)
    _(reQ1, 9.749521, 6)
    _(imQ1, 0.7813527, 7)
    _(reQ, 0.1019146, 7)
    _(imQ, -0.008167705, 9)
}

TEST_METHOD(change_Z0_far)
{
    GaussCalculator c;
    c.setZone(GaussCalculator::Zone::Far);
    c.setLambda(1.1e-6);
    c.setW0(100e-6);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setZ0(0.012);
    c.calc();

    _(M2, 2.379994, 5)
    _(lambda, 1.1e-06, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.012, 7)
    _(Vs, 0.008333333, 9)
    _(z, 0.1, 7)
    _(w, 0.0008393119, 10)
    _(R, 0.10144, 5)
    _(reQ1, 9.858044, 6)
    _(imQ1, 0.4970454, 7)
    _(reQ, 0.1011828, 7)
    _(imQ, -0.005101664, 9)

    c.setLock(GaussCalculator::Lock::Front);
    c.setZ0(0.016);
    c.calc();

    _(M2, 3.210236, 6)
    _(lambda, 1.1e-06, 12)
    _(w0, 0.0001341065, 10)
    _(z0, 0.016, 7)
    _(Vs, 0.008381658, 9)
    _(z, 0.09885022, 8)
    _(w, 0.0008393119, 10)
    _(R, 0.10144, 5)
    _(reQ1, 9.858044, 6)
    _(imQ1, 0.4970454, 7)
    _(reQ, 0.1011828, 7)
    _(imQ, -0.005101664, 9)
}

TEST_METHOD(change_Z0_near)
{
    GaussCalculator c;
    c.setZone(GaussCalculator::Zone::Near);
    c.setLambda(1.1e-6);
    c.setW0(100e-6);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setZ0(0.012);
    c.calc();

    _(M2, 2.379994, 5)
    _(lambda, 1.1e-06, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.012, 7)
    _(Vs, 0.008333333, 9)
    _(z, 0.1, 7)
    _(w, 0.0008393119, 10)
    _(R, 0.10144, 5)
    _(reQ1, 9.858044, 6)
    _(imQ1, 0.4970454, 7)
    _(reQ, 0.1011828, 7)
    _(imQ, -0.005101664, 9)

    c.setLock(GaussCalculator::Lock::Front);
    c.setZ0(0.016);
    c.calc();

    _(M2, 122.5328, 4)
    _(lambda, 1.1e-06, 12)
    _(w0, 0.0008285287, 10)
    _(z0, 0.016, 7)
    _(Vs, 0.05178305, 8)
    _(z, 0.002589777, 9)
    _(w, 0.0008393119, 10)
    _(R, 0.10144, 5)
    _(reQ1, 9.858044, 6)
    _(imQ1, 0.4970454, 7)
    _(reQ, 0.1011828, 7)
    _(imQ, -0.005101664, 9)
}

TEST_METHOD(change_Vs)
{
    GaussCalculator c;
    c.setLambda(640e-9);
    c.setW0(100e-6);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setVs(0.07);
    c.calc();

    _(M2, 34.36117, 5)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.001428571, 9)
    _(Vs, 0.07, 7)
    _(z, 0.1, 7)
    _(w, 0.007000714, 9)
    _(R, 0.1000204, 7)
    _(reQ1, 9.99796, 6)
    _(imQ1, 0.004156669, 9)
    _(reQ, 0.1000204, 7)
    _(imQ, -4.158365e-05, 11)

    c.setLock(GaussCalculator::Lock::Front);
    c.setVs(0.08);
    c.calc();

    _(M2, 1331.382, 3)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.003390336, 9)
    _(z0, 0.04237919, 8)
    _(Vs, 0.08, 7)
    _(z, 0.0765625, 7)
    _(w, 0.007000714, 9)
    _(R, 0.1000204, 7)
    _(reQ1, 9.99796, 6)
    _(imQ1, 0.004156669, 9)
    _(reQ, 0.1000204, 7)
    _(imQ, -4.158365e-05, 9)
}

TEST_METHOD(change_Z)
{
    GaussCalculator c;
    c.setLambda(640e-9);
    c.setM2(2);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setZ(0.1);
    c.calc();

    _(M2, 2, 5)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.02454369, 8)
    _(Vs, 0.004074367, 9)
    _(z, 0.1, 7)
    _(w, 0.0004195291, 10)
    _(R, 0.1060239, 7)
    _(reQ1, 9.431833, 6)
    _(imQ1, 1.15746, 5)
    _(reQ, 0.1044509, 7)
    _(imQ, -0.01281806, 8)

    c.setLock(GaussCalculator::Lock::Front);
    c.setZ(0.09);
    c.calc();

    _(M2, 3.438376, 6)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001630965, 10)
    _(z0, 0.0379757, 7)
    _(Vs, 0.004294759, 9)
    _(z, 0.09, 7)
    _(w, 0.0004195291, 10)
    _(R, 0.1060239, 7)
    _(reQ1, 9.431833, 6)
    _(imQ1, 1.15746, 5)
    _(reQ, 0.1044509, 7)
    _(imQ, -0.01281806, 8)
}

TEST_METHOD(change_M2)
{
    GaussCalculator c;
    c.setLambda(980e-9);
    c.setW0(100e-6);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setM2(2);
    c.calc();

    _(M2, 2, 7)
    _(lambda, 9.8e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.01602853, 8)
    _(Vs, 0.006238874, 9)
    _(z, 0.1, 7)
    _(w, 0.0006318508, 10)
    _(R, 0.1025691, 7)
    _(reQ1, 9.749521, 6)
    _(imQ1, 0.7813527, 7)
    _(reQ, 0.1019146, 7)
    _(imQ, -0.008167705, 9)

    c.setLock(GaussCalculator::Lock::Front);
    c.setM2(1.7);
    c.calc();

    _(M2, 1.7, 7)
    _(lambda, 9.8e-07, 12)
    _(w0, 8.529696e-05, 11)
    _(z0, 0.01371962, 8)
    _(Vs, 0.006217153, 8)
    _(z, 0.1006999, 7)
    _(w, 0.0006318508, 10)
    _(R, 0.1025691, 7)
    _(reQ1, 9.749521, 6)
    _(imQ1, 0.7813527, 7)
    _(reQ, 0.1019146, 7)
    _(imQ, -0.008167705, 9)
}

TEST_METHOD(change_W)
{
    GaussCalculator c;
    c.setLambda(640e-9);
    c.setW0(100e-6);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setW(600e-6);
    c.calc();

    _(M2, 2.904049, 6)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.01690309, 8)
    _(Vs, 0.00591608, 8)
    _(z, 0.1, 7)
    _(w, 0.0006, 7)
    _(R, 0.1028571, 7)
    _(reQ1, 9.722222, 6)
    _(imQ1, 0.5658842, 7)
    _(reQ, 0.1025099, 7)
    _(imQ, -0.00596661, 8)

    c.setLock(GaussCalculator::Lock::Front);
    c.setW(700e-6);
    c.calc();

    _(M2, 3.952733, 6)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001166667, 10)
    _(z0, 0.01690309, 8)
    _(Vs, 0.006902093, 9)
    _(z, 0.1, 7)
    _(w, 0.0007, 7)
    _(R, 0.1028571, 7)
    _(reQ1, 9.722222, 6)
    _(imQ1, 0.4157517, 7)
    _(reQ, 0.1026694, 7)
    _(imQ, -0.004390454, 9)
}

//------------------------------------------------------------------------------

TEST_GROUP("BeamCalculator",
           ADD_TEST(change_W0),
           ADD_TEST(change_Z0_far),
           ADD_TEST(change_Z0_near),
           ADD_TEST(change_Vs),
           ADD_TEST(change_Z),
           ADD_TEST(change_M2),
           ADD_TEST(change_W),
           )

} // namespace GaussCalculatorTests
} // namespace Tests
} // namespace Z
