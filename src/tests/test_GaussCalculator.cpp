#include "testing/OriTestBase.h"
#include "../funcs/GaussCalculator.h"

#ifdef Q_OS_MAC
#include <cmath>
#endif

// Test values are produced by script $PROJECT/calc/GaussCalculator.py

namespace Z {
namespace Tests {
namespace GaussCalculatorTests {

#define _(p, value, order)\
{\
    const double epsilon = std::pow(10.0, -order);\
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

TEST_METHOD(change_R)
{
    GaussCalculator c;
    c.setLambda(640e-9);
    c.setW0(100e-6);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setR(0.12);
    c.calc();

    _(M2, 1.097627, 6)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.04472136, 8)
    _(Vs, 0.002236068, 9)
    _(z, 0.1, 7)
    _(w, 0.000244949, 9)
    _(R, 0.12, 7)
    _(reQ1, 8.333333, 6)
    _(imQ1, 3.395305, 6)
    _(reQ, 0.1029155, 7)
    _(imQ, -0.04193156, 8)

    c.setLock(GaussCalculator::Lock::Front);
    c.setR(0.11);
    c.calc();

    _(M2, 0.8466979, 7)
    _(lambda, 6.4e-07, 12)
    _(w0, 7.385489e-05, 11)
    _(z0, 0.03162278, 8)
    _(Vs, 0.002335497, 9)
    _(z, 0.1, 7)
    _(w, 0.000244949, 9)
    _(R, 0.11, 7)
    _(reQ1, 9.090909, 6)
    _(imQ1, 3.395305, 6)
    _(reQ, 0.09653441, 8)
    _(imQ, -0.03605402, 8)
}

TEST_METHOD(change_Q)
{
    GaussCalculator c;
    c.setLambda(640e-9);
    c.setW0(100e-6);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setReQ(0.1);
    c.setImQ(-0.036);
    c.calc();

    _(M2, 1.196226, 6)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.0410352, 7)
    _(Vs, 0.002436932, 9)
    _(z, 0.09528854, 8)
    _(w, 0.0002528286, 10)
    _(R, 0.11296, 5)
    _(reQ1, 8.852691, 6)
    _(imQ1, 3.186969, 6)
    _(reQ, 0.1, 7)
    _(imQ, -0.036, 8)

    c.setLock(GaussCalculator::Lock::Front);
    c.setReQ(0.9);
    c.setImQ(-0.040);
    c.calc();

    _(M2, 65.45781, 5)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.001922674, 9)
    _(z0, 0.2772169, 7)
    _(Vs, 0.006935631, 9)
    _(z, 0.09528854, 8)
    _(w, 0.002033088, 9)
    _(R, 0.9017778, 7)
    _(reQ1, 1.108921, 6)
    _(imQ1, 0.04928536, 8)
    _(reQ, 0.9, 7)
    _(imQ, -0.040, 8)
}

TEST_METHOD(change_Q1_re)
{
    GaussCalculator c;
    c.setLambda(640e-9);
    c.setW0(100e-6);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setReQ1(9);
    c.calc();

    _(M2, 1.472622, 6)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.03333333, 8)
    _(Vs, 0.003, 4)
    _(z, 0.1, 7)
    _(w, 0.0003162278, 10)
    _(R, 0.1111111, 7)
    _(reQ1, 9, 6)
    _(imQ1, 2.037183, 6)
    _(reQ, 0.1056957, 7)
    _(imQ, -0.02392461, 8)

    c.setLock(GaussCalculator::Lock::Front);
    c.setReQ1(8);
    c.calc();

    _(M2, 1.963495, 6)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001414214, 10)
    _(z0, 0.05, 3)
    _(Vs, 0.002828427, 9)
    _(z, 0.1, 7)
    _(w, 0.0003162278, 10)
    _(R, 0.125, 4)
    _(reQ1, 8, 6)
    _(imQ1, 2.037183, 6)
    _(reQ, 0.1173879, 7)
    _(imQ, -0.02989259, 8)
}

TEST_METHOD(change_Q1_im)
{
    GaussCalculator c;
    c.setLambda(640e-9);
    c.setW0(100e-6);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setImQ1(0.05);
    c.calc();

    _(M2, 9.896151, 6)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.00496025, 8)
    _(Vs, 0.02016027, 8)
    _(z, 0.1, 7)
    _(w, 0.002018506, 9)
    _(R, 0.100246, 6)
    _(reQ1, 9.975456, 6)
    _(imQ1, 0.05, 7)
    _(reQ, 0.1002435, 7)
    _(imQ, -0.0005024508, 10)

    c.setLock(GaussCalculator::Lock::Front);
    c.setImQ1(0.45);
    c.calc();

    _(M2, 1.099572, 6)
    _(lambda, 6.4e-07, 12)
    _(w0, 3.333333e-05, 10)
    _(z0, 0.00496025, 8)
    _(Vs, 0.006720091, 9)
    _(z, 0.1, 7)
    _(w, 0.0006728353, 10)
    _(R, 0.100246, 6)
    _(reQ1, 9.975456, 6)
    _(imQ1, 0.45, 7)
    _(reQ, 0.1000425, 7)
    _(imQ, -0.004512987, 9)
}

TEST_METHOD(change_Lambda)
{
    GaussCalculator c;
    c.setW0(100e-6);
    c.setM2(2);
    c.setZ(0.1);

    c.setLock(GaussCalculator::Lock::Waist);
    c.setLambda(980e-9);
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
    c.setLambda(640e-9);
    c.calc();

    _(M2, 3.0625, 4)
    _(lambda, 6.4e-07, 12)
    _(w0, 0.0001, 7)
    _(z0, 0.01602853, 8)
    _(Vs, 0.006238874, 9)
    _(z, 0.1, 7)
    _(w, 0.0006318508, 9)
    _(R, 0.1025691, 7)
    _(reQ1, 9.749521, 6)
    _(imQ1, 0.5102711, 7)
    _(reQ, 0.1022889, 7)
    _(imQ, -0.005353606, 9)}

//------------------------------------------------------------------------------

TEST_GROUP("BeamCalculator",
           ADD_TEST(change_W0),
           ADD_TEST(change_Z0_far),
           ADD_TEST(change_Z0_near),
           ADD_TEST(change_Vs),
           ADD_TEST(change_Z),
           ADD_TEST(change_M2),
           ADD_TEST(change_W),
           ADD_TEST(change_R),
           ADD_TEST(change_Q),
           ADD_TEST(change_Q1_re),
           ADD_TEST(change_Q1_im),
           ADD_TEST(change_Lambda),
           )

} // namespace GaussCalculatorTests
} // namespace Tests
} // namespace Z
