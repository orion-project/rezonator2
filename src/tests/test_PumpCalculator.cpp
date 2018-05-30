#include "testing/OriTestBase.h"
#include "../funcs/PumpCalculator.h"
#include "../core/Pump.h"

// Test values are produced by script $PROJECT/calc/PumpCalculator.py

using namespace Z::Units;

namespace Z {
namespace Tests {
namespace PumpCalculatorTests {

#define SET_PARAM(param_name, valueT, valueS, unit)\
    p.param_name()->setValue(ValueTS(valueT, valueS, unit()));
/*
TEST_METHOD(calcFront_Waist)
{
    PumpParams_Waist p;
    SET_PARAM(waist, 100, 150, mkm)
    SET_PARAM(distance, 100, 110, mm)
    SET_PARAM(MI, 2, 1, none)
    auto q = PumpCalculator::calcFront(&p, 980e-9);
    ASSERT_NEAR_DBL(q.T.real(), 0.101914557344, 1e-12)
    ASSERT_NEAR_DBL(q.T.imag(), -0.00816770471033, 1e-12)
    ASSERT_NEAR_DBL(q.S.real(), 0.11, 1e-12)
    ASSERT_NEAR_DBL(q.S.imag(), -0.072128402761, 1e-12)
}

TEST_METHOD(calcFront_Front)
{
    PumpParams_Front p;
    SET_PARAM(beamRadius, 300, 400, mkm)
    SET_PARAM(frontRadius, 110, 120, mm)
    auto q = PumpCalculator::calcFront(&p, 980e-9);
    ASSERT_NEAR_DBL(q.T.real(), 0.0960394529947, 1e-12)
    ASSERT_NEAR_DBL(q.T.imag(), -0.0366164348059, 1e-12)
    ASSERT_NEAR_DBL(q.S.real(), 0.113772520423, 1e-12)
    ASSERT_NEAR_DBL(q.S.imag(), -0.0266179647495, 1e-12)
}

TEST_METHOD(calcFront_RayVector)
{
    PumpParams_RayVector p;
    SET_PARAM(radius, 1, 1.5, mm)
    SET_PARAM(angle, 5, 10, deg)
    SET_PARAM(distance, 10, 9, cm)
    auto v = PumpCalculator::calcFront(&p);
    ASSERT_NEAR_DBL(v.T.Y, 0.009748866, 1e-9)
    ASSERT_NEAR_DBL(v.T.V, 0.08726646, 1e-8)
    ASSERT_NEAR_DBL(v.S.Y, 0.01736943, 1e-8)
    ASSERT_NEAR_DBL(v.S.V, 0.1745329, 1e-7)
}

TEST_METHOD(calcFront_TwoSections)
{
    PumpParams_TwoSections p;
    SET_PARAM(radius1, 1, 1.5, mm)
    SET_PARAM(radius2, 2, 2.5, mm)
    SET_PARAM(distance, 10, 11, cm)
    auto v = PumpCalculator::calcFront(&p);
    ASSERT_NEAR_DBL(v.T.Y, 0.002, 1e-4)
    ASSERT_NEAR_DBL(v.T.V, 0.009999667, 1e-9)
    ASSERT_NEAR_DBL(v.S.Y, 0.0025, 1e-5)
    ASSERT_NEAR_DBL(v.S.V, 0.009090659, 1e-9)
}

TEST_METHOD(calcFront_Complex)
{
    PumpParams_Complex p;
    SET_PARAM(real, 110, 120, mm)
    SET_PARAM(imag, 30, 40, mm)
    auto q = PumpCalculator::calcFront(&p);
    ASSERT_NEAR_DBL(q.T.real(), 0.11, 1e-12)
    ASSERT_NEAR_DBL(q.T.imag(), 0.03, 1e-12)
    ASSERT_NEAR_DBL(q.S.real(), 0.12, 1e-12)
    ASSERT_NEAR_DBL(q.S.imag(), 0.04, 1e-12)
}

TEST_METHOD(calcFront_InvComplex)
{
    PumpParams_InvComplex p;
    SET_PARAM(real, 0.011, 0.012, mm)
    SET_PARAM(imag, -0.30, -0.40, cm)
    auto q = PumpCalculator::calcFront(&p);
    ASSERT_NEAR_DBL(q.T.real(), 0.0107737512243, 1e-12)
    ASSERT_NEAR_DBL(q.T.imag(), 0.0293829578844, 1e-12)
    ASSERT_NEAR_DBL(q.S.real(), 0.00688073394495, 1e-12)
    ASSERT_NEAR_DBL(q.S.imag(), 0.0229357798165, 1e-12)
}
*/
//------------------------------------------------------------------------------

TEST_GROUP("Pump Calculator",
//           ADD_TEST(calcFront_Waist),
//           ADD_TEST(calcFront_Front),
//           ADD_TEST(calcFront_RayVector),
//           ADD_TEST(calcFront_TwoSections),
//           ADD_TEST(calcFront_Complex),
//           ADD_TEST(calcFront_InvComplex),
           )

} // namespace PumpCalculatorTests
} // namespace Tests
} // namespace Z
