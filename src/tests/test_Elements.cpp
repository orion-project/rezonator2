#include "../core/Elements.h"
#include "../core/ElementsCatalog.h"
#include "../core/Pump.h"
#include "../tests/TestUtils.h"
#include "../math/PumpCalculator.h"

#include "testing/OriTestBase.h"

#include <QSharedPointer>

namespace Z {
namespace Tests {
namespace ElementsTests {

#define ELEM(elem_type, expected_param_count) \
    QSharedPointer<Elem ## elem_type> elem(new Elem ## elem_type);\
    ASSERT_EQ_INT(elem->params().size(), expected_param_count)

#define SET_PARAM(name, value, unit) { \
    auto param_##name = elem->params().byAlias(#name);\
    ASSERT_IS_NOT_NULL(param_##name)\
    param_##name->setValue(Z::Value(value, Z::Units::unit())); }

#define ASSERT_RAW_PARAM(param, value)\
    ASSERT_NEAR_DBL(elem->param(), value, 1e-7)

#define ASSERT_MATRIX(ts, a, b, c, d)\
    ASSERT_NEAR_DBL(elem->M##ts().A.real(), a, 1e-7)\
    ASSERT_NEAR_DBL(elem->M##ts().B.real(), b, 1e-7)\
    ASSERT_NEAR_DBL(elem->M##ts().C.real(), c, 1e-7)\
    ASSERT_NEAR_DBL(elem->M##ts().D.real(), d, 1e-7)\
    ASSERT_EQ_DBL(elem->M##ts().A.imag(), 0)\
    ASSERT_EQ_DBL(elem->M##ts().B.imag(), 0)\
    ASSERT_EQ_DBL(elem->M##ts().C.imag(), 0)\
    ASSERT_EQ_DBL(elem->M##ts().D.imag(), 0)

#define ASSERT_MATRIX_C(ts, a, b, c, d) { \
    const double eps = 1e-7;\
    ASSERT_NEAR_DBL(elem->M##ts().A.real(), a.real(), eps)\
    ASSERT_NEAR_DBL(elem->M##ts().B.real(), b.real(), eps)\
    ASSERT_NEAR_DBL(elem->M##ts().C.real(), c.real(), eps)\
    ASSERT_NEAR_DBL(elem->M##ts().D.real(), d.real(), eps)\
    ASSERT_NEAR_DBL(elem->M##ts().A.imag(), a.imag(), eps)\
    ASSERT_NEAR_DBL(elem->M##ts().B.imag(), b.imag(), eps)\
    ASSERT_NEAR_DBL(elem->M##ts().C.imag(), c.imag(), eps)\
    ASSERT_NEAR_DBL(elem->M##ts().D.imag(), d.imag(), eps)\
}

#define _C Z::Complex

#define ASSERT_PARAM_VISIBLE(name, expected_visible) { \
    auto param_##name = elem->params().byAlias(#name);\
    ASSERT_IS_NOT_NULL(param_##name)\
    ASSERT_IS_TRUE(param_##name->visible() == expected_visible) }

//------------------------------------------------------------------------------

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(EmptyRange)
{
    ELEM(EmptyRange, 2)
    SET_PARAM(L, 88, mm)

    ASSERT_RAW_PARAM(lengthSI, 0.088)
    ASSERT_RAW_PARAM(axisLengthSI, 0.088)
    ASSERT_RAW_PARAM(opticalPathSI, 0.088)
    ASSERT_RAW_PARAM(ior, 1)

    ASSERT_IS_FALSE(elem->paramIor()->visible())

    ASSERT_MATRIX(t, 1, 0.088, 0, 1)
    ASSERT_MATRIX(s, 1, 0.088, 0, 1)

    elem->setSubRangeSI(0.0176000);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.0176000)
    ASSERT_MATRIX(t1, 1.0000000, 0.0176000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s1, 1.0000000, 0.0176000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(t2, 1.0000000, 0.0704000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0704000, 0.0000000, 1.0000000)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(MediumRange)
{
    ELEM(MediumRange, 2)
    SET_PARAM(L, 88, mm)
    SET_PARAM(n, 1.2, none)

    ASSERT_RAW_PARAM(ior, 1.2)
    ASSERT_RAW_PARAM(lengthSI, 0.088)
    ASSERT_RAW_PARAM(axisLengthSI, 0.088)
    ASSERT_RAW_PARAM(opticalPathSI, 0.1056000)

    ASSERT_MATRIX(t, 1, 0.088, 0, 1)
    ASSERT_MATRIX(s, 1, 0.088, 0, 1)

    elem->setSubRangeSI(0.0176000);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.0176000)
    ASSERT_MATRIX(t1, 1.0000000, 0.0176000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s1, 1.0000000, 0.0176000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(t2, 1.0000000, 0.0704000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0704000, 0.0000000, 1.0000000)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/ElemPlate.py
TEST_METHOD(Plate)
{
    ELEM(Plate, 2)
    SET_PARAM(L, 88, mm)
    SET_PARAM(n, 1.2, none)

    ASSERT_RAW_PARAM(ior, 1.2)
    ASSERT_RAW_PARAM(lengthSI, 0.088)
    ASSERT_RAW_PARAM(axisLengthSI, 0.088)
    ASSERT_RAW_PARAM(opticalPathSI, 0.1056000)

    ASSERT_MATRIX(t, 1.0000000, 0.0733333, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s, 1.0000000, 0.0733333, 0.0000000, 1.0000000)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.0176000);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.0176000)
    ASSERT_MATRIX(t1, 1.0000000, 0.0146667, 0.0000000, 0.8333333)
    ASSERT_MATRIX(s1, 1.0000000, 0.0146667, 0.0000000, 0.8333333)
    ASSERT_MATRIX(t2, 1.0000000, 0.0704000, 0.0000000, 1.2000000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0704000, 0.0000000, 1.2000000)
}

TEST_METHOD(FlatMirror)
{
    ELEM(FlatMirror, 0)
    elem->calcMatrix("");
    ASSERT_MATRIX_IS_UNITY(elem->Mt())
    ASSERT_MATRIX_IS_UNITY(elem->Ms())
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(CurveMirror)
{
    ELEM(CurveMirror, 2)

    SET_PARAM(R, 100, mm)
    SET_PARAM(Alpha, 15, deg)

    ASSERT_RAW_PARAM(radius, 0.1)
    ASSERT_RAW_PARAM(alpha, 0.2617994)
    ASSERT_MATRIX(t, 1, 0, -20.7055236, 1)
    ASSERT_MATRIX(s, 1, 0, -19.3185165, 1)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(ThinLens)
{
    ELEM(ThinLens, 2)
    SET_PARAM(F, 100, mm)
    SET_PARAM(Alpha, 15, deg)

    ASSERT_RAW_PARAM(focus, 0.1)
    ASSERT_RAW_PARAM(alpha, 0.2617994)
    ASSERT_MATRIX(t, 1, 0, -10.3527618, 1)
    ASSERT_MATRIX(s, 1, 0, -9.6592583, 1)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(CylinderLensT)
{
    ELEM(CylinderLensT, 2)
    SET_PARAM(F, 100, mm)
    SET_PARAM(Alpha, 15, deg)

    ASSERT_RAW_PARAM(focus, 0.1)
    ASSERT_RAW_PARAM(alpha, 0.2617994)
    ASSERT_MATRIX(t, 1, 0, -10.3527618, 1)
    ASSERT_MATRIX(s, 1, 0, 0, 1)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(CylinderLensS)
{
    ELEM(CylinderLensS, 2)
    SET_PARAM(F, 100, mm)
    SET_PARAM(Alpha, 15, deg)

    ASSERT_RAW_PARAM(focus, 0.1)
    ASSERT_RAW_PARAM(alpha, 0.2617994)
    ASSERT_MATRIX(t, 1, 0, 0, 1)
    ASSERT_MATRIX(s, 1, 0, -9.6592583, 1)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/ElemTiltedCrystal.py
TEST_METHOD(TiltedCrystal)
{
    ELEM(TiltedCrystal, 3)
    SET_PARAM(L, 4.5, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(Alpha, 15, deg)

    ASSERT_RAW_PARAM(lengthSI,      0.0045)
    ASSERT_RAW_PARAM(ior,           1.5)
    ASSERT_RAW_PARAM(alpha,         0.2617994)
    ASSERT_RAW_PARAM(axisLengthSI,  0.0045)
    ASSERT_RAW_PARAM(opticalPathSI, 0.0067500)

    ASSERT_MATRIX(t, 1.0000000, 0.0028849, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s, 1.0000000, 0.0030000, 0.0000000, 1.0000000)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.0009000);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.0009000)
    ASSERT_MATRIX(t1, 1.0197485, 0.0005884, 0.0000000, 0.6537559)
    ASSERT_MATRIX(s1, 1.0000000, 0.0006000, 0.0000000, 0.6666667)
    ASSERT_MATRIX(t2, 0.9806339, 0.0035303, 0.0000000, 1.5296228)
    ASSERT_MATRIX(s2, 1.0000000, 0.0036000, 0.0000000, 1.5000000)
}

// Calculation: $PROJECT/calc/ElemTiltedPlate.py
TEST_METHOD(TiltedPlate)
{
    ELEM(TiltedPlate, 3)
    SET_PARAM(L, 4.5, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(Alpha, 15, deg)

    ASSERT_RAW_PARAM(lengthSI,      0.0045)
    ASSERT_RAW_PARAM(ior,           1.5)
    ASSERT_RAW_PARAM(alpha,         0.2617994)
    ASSERT_RAW_PARAM(axisLengthSI,  0.0045685)
    ASSERT_RAW_PARAM(opticalPathSI, 0.0068528)

    ASSERT_MATRIX(t, 1, 0.0029289, 0, 1)
    ASSERT_MATRIX(s, 1, 0.0030457, 0, 1)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.0009137);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.0009137)
    ASSERT_MATRIX(t1, 1.0197485, 0.0005973, 0.0000000, 0.6537559)
    ASSERT_MATRIX(s1, 1.0000000, 0.0006091, 0.0000000, 0.6666667)
    ASSERT_MATRIX(t2, 0.9806339, 0.0035840, 0.0000000, 1.5296228)
    ASSERT_MATRIX(s2, 1.0000000, 0.0036548, 0.0000000, 1.5000000)
}

// Calculation: $PROJECT/calc/ElemBrewsterCrystal.py
TEST_METHOD(BrewsterCrystal)
{
    ELEM(BrewsterCrystal, 2)
    SET_PARAM(L, 4.5, mm)
    SET_PARAM(n, 1.5, none)

    ASSERT_RAW_PARAM(lengthSI,      0.0045)
    ASSERT_RAW_PARAM(ior,           1.5)
    ASSERT_RAW_PARAM(axisLengthSI,  0.0045)
    ASSERT_RAW_PARAM(opticalPathSI, 0.0067500)

    ASSERT_MATRIX(t, 1.0000000, 0.0013333, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s, 1.0000000, 0.0030000, 0.0000000, 1.0000000)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.0009000);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.0009000)
    ASSERT_MATRIX(t1, 1.5000000, 0.0004000, 0.0000000, 0.4444444)
    ASSERT_MATRIX(s1, 1.0000000, 0.0006000, 0.0000000, 0.6666667)
    ASSERT_MATRIX(t2, 0.6666667, 0.0024000, 0.0000000, 2.2500000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0036000, 0.0000000, 1.5000000)
}

// Calculation: $PROJECT/calc/ElemBrewsterPlate.py
TEST_METHOD(BrewsterPlate)
{
    ELEM(BrewsterPlate, 2)
    SET_PARAM(L, 4.5, mm)
    SET_PARAM(n, 1.5, none)

    ASSERT_RAW_PARAM(lengthSI,      0.0045)
    ASSERT_RAW_PARAM(ior,           1.5)
    ASSERT_RAW_PARAM(axisLengthSI,  0.0054083)
    ASSERT_RAW_PARAM(opticalPathSI, 0.0081125)

    ASSERT_MATRIX(t, 1.0000000, 0.0016025, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s, 1.0000000, 0.0036056, 0.0000000, 1.0000000)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.0010817);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.0010817)
    ASSERT_MATRIX(t1, 1.5000000, 0.0004807, 0.0000000, 0.4444444)
    ASSERT_MATRIX(s1, 1.0000000, 0.0007211, 0.0000000, 0.6666667)
    ASSERT_MATRIX(t2, 0.6666667, 0.0028844, 0.0000000, 2.2500000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0043267, 0.0000000, 1.5000000)
}

TEST_METHOD(Matrix_1)
{
    ELEM(Matrix, 8)
    elem->setMatrixT(1.1, 2.2, 3.3, 4.4);
    elem->setMatrixS(5.5, 6.6, 7.7, 8.8);

    ASSERT_MATRIX(t, 1.1, 2.2, 3.3, 4.4)
    ASSERT_MATRIX(s, 5.5, 6.6, 7.7, 8.8)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

TEST_METHOD(Matrix_2)
{
    ELEM(Matrix, 8)
    SET_PARAM(At, 1.1, none)
    SET_PARAM(Bt, 220, cm)
    SET_PARAM(Ct, 3.3, inv_m)
    SET_PARAM(Dt, 4.4, none)
    SET_PARAM(As, 5.5, none)
    SET_PARAM(Bs, 6600, mm)
    SET_PARAM(Cs, 7.7, inv_m)
    SET_PARAM(Ds, 8.8, none)

    ASSERT_MATRIX(t, 1.1, 2.2, 3.3, 4.4)
    ASSERT_MATRIX(s, 5.5, 6.6, 7.7, 8.8)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

TEST_METHOD(Matrix1_1)
{
    ELEM(Matrix1, 4)
    elem->setMatrix(1.1, 2.2, 3.3, 4.4);

    ASSERT_MATRIX(t, 1.1, 2.2, 3.3, 4.4)
    ASSERT_EQ_MATRIX(elem->Mt(), elem->Ms())
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

TEST_METHOD(Matrix1_2)
{
    ELEM(Matrix1, 4)
    SET_PARAM(A, 1.1, none)
    SET_PARAM(B, 220, cm)
    SET_PARAM(C, 3.3, inv_m)
    SET_PARAM(D, 4.4, none)

    ASSERT_MATRIX(t, 1.1, 2.2, 3.3, 4.4)
    ASSERT_EQ_MATRIX(elem->Mt(), elem->Ms())
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

TEST_METHOD(Point)
{
    ELEM(Point, 0)
    elem->calcMatrix("");
    ASSERT_MATRIX_IS_UNITY(elem->Mt())
    ASSERT_MATRIX_IS_UNITY(elem->Ms())
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/ElemThickLens.py
TEST_METHOD(ThickLens)
{
    ELEM(ThickLens, 4)
    SET_PARAM(L, 10, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(R1, -90, mm)
    SET_PARAM(R2, 150, mm)

    ASSERT_RAW_PARAM(ior, 1.5)
    ASSERT_RAW_PARAM(lengthSI, 0.01)
    ASSERT_RAW_PARAM(axisLengthSI, 0.01)
    ASSERT_RAW_PARAM(opticalPathSI, 0.015)

    ASSERT_MATRIX(t, 0.9629630, 0.0066667, -8.7654321, 0.9777778)
    ASSERT_MATRIX(s, 0.9629630, 0.0066667, -8.7654321, 0.9777778)
    ASSERT_MATRIX(t_inv, 0.9777778, 0.0066667, -8.7654321, 0.9629630)
    ASSERT_MATRIX(s_inv, 0.9777778, 0.0066667, -8.7654321, 0.9629630)

    elem->setSubRangeSI(0.002);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.002)
    ASSERT_MATRIX(t1, 0.9925926, 0.0013333, -3.7037037, 0.6666667)
    ASSERT_MATRIX(s1, 0.9925926, 0.0013333, -3.7037037, 0.6666667)
    ASSERT_MATRIX(t2, 1.0000000, 0.0080000, -3.3333333, 1.4733333)
    ASSERT_MATRIX(s2, 1.0000000, 0.0080000, -3.3333333, 1.4733333)
}

TEST_METHOD(ThickLens_flat1)
{
    ELEM(ThickLens, 4)
    SET_PARAM(L, 10, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(R1, qInf(), mm)
    SET_PARAM(R2, 150, mm)

    ASSERT_RAW_PARAM(ior, 1.5)
    ASSERT_RAW_PARAM(lengthSI, 0.01)
    ASSERT_RAW_PARAM(axisLengthSI, 0.01)
    ASSERT_RAW_PARAM(opticalPathSI, 0.015)

    ASSERT_MATRIX(t, 1.0000000, 0.0066667, -3.3333333, 0.9777778)
    ASSERT_MATRIX(s, 1.0000000, 0.0066667, -3.3333333, 0.9777778)
    ASSERT_MATRIX(t_inv, 0.9777778, 0.0066667, -3.3333333, 1.0000000)
    ASSERT_MATRIX(s_inv, 0.9777778, 0.0066667, -3.3333333, 1.0000000)

    elem->setSubRangeSI(0.002);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.002)
    ASSERT_MATRIX(t1, 1.0000000, 0.0013333, 0.0000000, 0.6666667)
    ASSERT_MATRIX(s1, 1.0000000, 0.0013333, 0.0000000, 0.6666667)
    ASSERT_MATRIX(t2, 1.0000000, 0.0080000, -3.3333333, 1.4733333)
    ASSERT_MATRIX(s2, 1.0000000, 0.0080000, -3.3333333, 1.4733333)
}

TEST_METHOD(ThickLens_flat2)
{
    ELEM(ThickLens, 4)
    SET_PARAM(L, 10, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(R1, -90, mm)
    SET_PARAM(R2, qInf(), mm)

    ASSERT_RAW_PARAM(ior, 1.5)
    ASSERT_RAW_PARAM(lengthSI, 0.01)
    ASSERT_RAW_PARAM(axisLengthSI, 0.01)
    ASSERT_RAW_PARAM(opticalPathSI, 0.015)

    ASSERT_MATRIX(t, 0.9629630, 0.0066667, -5.5555556, 1.0000000)
    ASSERT_MATRIX(s, 0.9629630, 0.0066667, -5.5555556, 1.0000000)
    ASSERT_MATRIX(t_inv, 1.0000000, 0.0066667, -5.5555556, 0.9629630)
    ASSERT_MATRIX(s_inv, 1.0000000, 0.0066667, -5.5555556, 0.9629630)

    elem->setSubRangeSI(0.002);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.002)
    ASSERT_MATRIX(t1, 0.9925926, 0.0013333, -3.7037037, 0.6666667)
    ASSERT_MATRIX(s1, 0.9925926, 0.0013333, -3.7037037, 0.6666667)
    ASSERT_MATRIX(t2, 1.0000000, 0.0080000, -0.0000000, 1.5000000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0080000, -0.0000000, 1.5000000)
}

TEST_METHOD(ThickLens_flat1_flat2)
{
    ELEM(ThickLens, 4)
    SET_PARAM(L, 10, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(R1, qInf(), mm)
    SET_PARAM(R2, qInf(), mm)

    ASSERT_RAW_PARAM(ior, 1.5)
    ASSERT_RAW_PARAM(lengthSI, 0.01)
    ASSERT_RAW_PARAM(axisLengthSI, 0.01)
    ASSERT_RAW_PARAM(opticalPathSI, 0.015)

    ASSERT_MATRIX(t, 1.0000000, 0.0066667, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s, 1.0000000, 0.0066667, 0.0000000, 1.0000000)
    ASSERT_MATRIX(t_inv, 1.0000000, 0.0066667, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s_inv, 1.0000000, 0.0066667, 0.0000000, 1.0000000)

    elem->setSubRangeSI(0.002);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.002)
    ASSERT_MATRIX(t1, 1.0000000, 0.0013333, 0.0000000, 0.6666667)
    ASSERT_MATRIX(s1, 1.0000000, 0.0013333, 0.0000000, 0.6666667)
    ASSERT_MATRIX(t2, 1.0000000, 0.0080000, -0.0000000, 1.5000000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0080000, -0.0000000, 1.5000000)
}


// Calculation: $PROJECT/calc/ElemGrinLens.py
TEST_METHOD(GrinLens)
{
    ELEM(GrinLens, 4)
    SET_PARAM(L, 10, mm)
    SET_PARAM(n, 1.5, none)

    ASSERT_RAW_PARAM(ior, 1.5)
    ASSERT_RAW_PARAM(lengthSI, 0.01)

    // positive
    SET_PARAM(n2t, 0.2, none)
    SET_PARAM(n2s, 0.3, none)
    ASSERT_RAW_PARAM(ior2t, 0.2)
    ASSERT_RAW_PARAM(ior2s, 0.3)

    ASSERT_MATRIX(t, 0.9999933, 0.0066667, -0.0020000, 0.9999933)
    ASSERT_MATRIX(s, 0.9999900, 0.0066666, -0.0030000, 0.9999900)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.003);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.003)
    ASSERT_MATRIX(t1, 0.9999994, 0.0020000, -0.0004000, 0.6666663)
    ASSERT_MATRIX(s1, 0.9999991, 0.0020000, -0.0006000, 0.6666661)
    ASSERT_MATRIX(t2, 0.9999967, 0.0070000, -0.0014000, 1.4999951)
    ASSERT_MATRIX(s2, 0.9999951, 0.0070000, -0.0021000, 1.4999927)

    // negative
    SET_PARAM(n2t, -0.2, none)
    SET_PARAM(n2s, -0.3, none)
    ASSERT_RAW_PARAM(ior2t, -0.2)
    ASSERT_RAW_PARAM(ior2s, -0.3)

    ASSERT_MATRIX(t, 1.0000067, 0.0066667, 0.0020000, 1.0000067)
    ASSERT_MATRIX(s, 1.0000100, 0.0066667, 0.0030000, 1.0000100)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.003);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.003)
    ASSERT_MATRIX(t1, 1.0000006, 0.0020000, 0.0004000, 0.6666671)
    ASSERT_MATRIX(s1, 1.0000009, 0.0020000, 0.0006000, 0.6666673)
    ASSERT_MATRIX(t2, 1.0000033, 0.0070000, 0.0014000, 1.5000049)
    ASSERT_MATRIX(s2, 1.0000049, 0.0070000, 0.0021000, 1.5000074)

    // zero
    SET_PARAM(n2t, 0, none)
    SET_PARAM(n2s, 0, none)
    ASSERT_RAW_PARAM(ior2t, 0)
    ASSERT_RAW_PARAM(ior2s, 0)

    ASSERT_MATRIX(t, 1, 0.01/1.5, 0, 1)
    ASSERT_MATRIX(s, 1, 0.01/1.5, 0, 1)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.003);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.003)
    ASSERT_MATRIX(t1, 1, 0.003/1.5, 0, 1.0/1.5)
    ASSERT_MATRIX(s1, 1, 0.003/1.5, 0, 1.0/1.5)
    ASSERT_MATRIX(t2, 1, 0.01-0.003, 0, 1.5)
    ASSERT_MATRIX(s2, 1, 0.01-0.003, 0, 1.5)
}

// Calculation: $PROJECT/calc/ElemGrinLens.py
TEST_METHOD(GrinMedium)
{
    ELEM(GrinMedium, 4)
    SET_PARAM(L, 10, mm)
    SET_PARAM(n, 1.5, none)

    ASSERT_RAW_PARAM(ior, 1.5)
    ASSERT_RAW_PARAM(lengthSI, 0.01)

    // positive
    SET_PARAM(n2t, 0.2, none)
    SET_PARAM(n2s, 0.3, none)
    ASSERT_RAW_PARAM(ior2t, 0.2)
    ASSERT_RAW_PARAM(ior2s, 0.3)

    ASSERT_MATRIX(t, 0.9999933, 0.0100000, -0.0013333, 0.9999933)
    ASSERT_MATRIX(s, 0.9999900, 0.0100000, -0.0020000, 0.9999900)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.003);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.003)
    ASSERT_MATRIX(t1, 0.9999994, 0.0030000, -0.0004000, 0.9999994)
    ASSERT_MATRIX(s1, 0.9999991, 0.0030000, -0.0006000, 0.9999991)
    ASSERT_MATRIX(t2, 0.9999967, 0.0070000, -0.0009333, 0.9999967)
    ASSERT_MATRIX(s2, 0.9999951, 0.0070000, -0.0014000, 0.9999951)

    // negative
    SET_PARAM(n2t, -0.2, none)
    SET_PARAM(n2s, -0.3, none)
    ASSERT_RAW_PARAM(ior2t, -0.2)
    ASSERT_RAW_PARAM(ior2s, -0.3)

    ASSERT_MATRIX(t, 1.0000067, 0.0100000, 0.0013333, 1.0000067)
    ASSERT_MATRIX(s, 1.0000100, 0.0100000, 0.0020000, 1.0000100)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.003);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.003)
    ASSERT_MATRIX(t1, 1.0000006, 0.0030000, 0.0004000, 1.0000006)
    ASSERT_MATRIX(s1, 1.0000009, 0.0030000, 0.0006000, 1.0000009)
    ASSERT_MATRIX(t2, 1.0000033, 0.0070000, 0.0009333, 1.0000033)
    ASSERT_MATRIX(s2, 1.0000049, 0.0070000, 0.0014000, 1.0000049)

    // zero
    SET_PARAM(n2t, 0, none)
    SET_PARAM(n2s, 0, none)
    ASSERT_RAW_PARAM(ior2t, 0)
    ASSERT_RAW_PARAM(ior2s, 0)

    ASSERT_MATRIX(t, 1, 0.01, 0, 1)
    ASSERT_MATRIX(s, 1, 0.01, 0, 1)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.003);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.003)
    ASSERT_MATRIX(t1, 1, 0.003, 0, 1)
    ASSERT_MATRIX(s1, 1, 0.003, 0, 1)
    ASSERT_MATRIX(t2, 1, 0.01-0.003, 0, 1)
    ASSERT_MATRIX(s2, 1, 0.01-0.003, 0, 1)
}

// Calculation: $PROJECT/calc/ElemGrinLens.py
TEST_METHOD(ThermoLens)
{
    ELEM(ThermoLens, 3)
    SET_PARAM(L, 100, mm)
    SET_PARAM(n, 1.7, none)

    ASSERT_RAW_PARAM(ior, 1.7)
    ASSERT_RAW_PARAM(lengthSI, 0.1)

    // positive
    SET_PARAM(F, 1.5, m) // n2=6.580425421766774
    ASSERT_RAW_PARAM(focus, 1.5)

    ASSERT_MATRIX(t, 0.9807082, 0.0584448, -0.6538055, 0.9807082)
    ASSERT_MATRIX(s, 0.9807082, 0.0584448, -0.6538055, 0.9807082)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.03);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.03)
    ASSERT_MATRIX(t1, 0.9982586, 0.0176368, -0.1160577, 0.5872110)
    ASSERT_MATRIX(s1, 0.9982586, 0.0176368, -0.1160577, 0.5872110)
    ASSERT_MATRIX(t2, 0.9905314, 0.0697789, -0.4591750, 1.6839034)
    ASSERT_MATRIX(s2, 0.9905314, 0.0697789, -0.4591750, 1.6839034)

    // negative
    SET_PARAM(F, -1.5, m) // n2=-6.754730954420761
    ASSERT_RAW_PARAM(focus, -1.5)

    ASSERT_MATRIX(t, 1.0199327, 0.0592139, 0.6799552, 1.0199327)
    ASSERT_MATRIX(s, 1.0199327, 0.0592139, 0.6799552, 1.0199327)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.03);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.03)
    ASSERT_MATRIX(t1, 1.0017885, 0.0176576, 0.1192722, 0.5892874)
    ASSERT_MATRIX(s1, 1.0017885, 0.0176576, 0.1192722, 0.5892874)
    ASSERT_MATRIX(t2, 1.0097506, 0.0702274, 0.4743670, 1.7165760)
    ASSERT_MATRIX(s2, 1.0097506, 0.0702274, 0.4743670, 1.7165760)
}

// Calculation: $PROJECT/calc/ElemGrinLens.py
TEST_METHOD(ThermoMedium)
{
    ELEM(ThermoMedium, 3)
    SET_PARAM(L, 100, mm)
    SET_PARAM(n, 1.7, none)

    ASSERT_RAW_PARAM(ior, 1.7)
    ASSERT_RAW_PARAM(lengthSI, 0.1)

    // positive
    SET_PARAM(F, 1.5, m) // n2=6.580425421766774
    ASSERT_RAW_PARAM(focus, 1.5)

    ASSERT_MATRIX(t, 0.9807082, 0.0993561, -0.3845914, 0.9807082)
    ASSERT_MATRIX(s, 0.9807082, 0.0993561, -0.3845914, 0.9807082)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.03);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.03)
    ASSERT_MATRIX(t1, 0.9982586, 0.0299826, -0.1160577, 0.9982586)
    ASSERT_MATRIX(s1, 0.9982586, 0.0299826, -0.1160577, 0.9982586)
    ASSERT_MATRIX(t2, 0.9905314, 0.0697789, -0.2701030, 0.9905314)
    ASSERT_MATRIX(s2, 0.9905314, 0.0697789, -0.2701030, 0.9905314)

    // negative
    SET_PARAM(F, -1.5, m) // n2=-6.754730954420761
    ASSERT_RAW_PARAM(focus, -1.5)

    ASSERT_MATRIX(t, 1.0199327, 0.1006635, 0.3999736, 1.0199327)
    ASSERT_MATRIX(s, 1.0199327, 0.1006635, 0.3999736, 1.0199327)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.03);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.03)
    ASSERT_MATRIX(t1, 1.0017885, 0.0300179, 0.1192722, 1.0017885)
    ASSERT_MATRIX(s1, 1.0017885, 0.0300179, 0.1192722, 1.0017885)
    ASSERT_MATRIX(t2, 1.0097506, 0.0702274, 0.2790394, 1.0097506)
    ASSERT_MATRIX(s2, 1.0097506, 0.0702274, 0.2790394, 1.0097506)
}

// Calculation: $PROJECT/calc/ElemGauss.py
TEST_METHOD(GaussAperture)
{
    ELEM(GaussAperture, 3)
    SET_PARAM(Lambda, 980, nm)
    SET_PARAM(alpha2t, 1e6, inv_m2)
    SET_PARAM(alpha2s, 2e6, inv_m2)

    ASSERT_PARAM_VISIBLE(Lambda, false)

    ASSERT_MATRIX_C(t, _C(1,0), _C(0,0), _C(0,-0.1559718), _C(1,0))
    ASSERT_MATRIX_C(s, _C(1,0), _C(0,0), _C(0,-0.3119437), _C(1,0))
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/ElemGauss.py
TEST_METHOD(GaussApertureLens)
{
    ELEM(GaussApertureLens, 5)
    SET_PARAM(Lambda, 980, nm)
    SET_PARAM(Ft, 50, cm)
    SET_PARAM(Fs, 60, cm)
    SET_PARAM(alpha2t, 1e6, inv_m2)
    SET_PARAM(alpha2s, 2e6, inv_m2)

    ASSERT_PARAM_VISIBLE(Lambda, false)

    ASSERT_MATRIX_C(t, _C(1,0), _C(0,0), _C(-2,-0.1559718), _C(1,0))
    ASSERT_MATRIX_C(s, _C(1,0), _C(0,0), _C(-1.6666667,-0.3119437), _C(1,0))
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/ElemGauss.py
TEST_METHOD(GaussDuctMedium)
{
    ELEM(GaussDuctMedium, 7)
    SET_PARAM(L, 10, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(Lambda, 980, nm)
    SET_PARAM(n2t, 0.2, inv_m2)
    SET_PARAM(n2s, 0.3, inv_m2)
    SET_PARAM(alpha2t, 1e8, inv_m3)
    SET_PARAM(alpha2s, 2e8, inv_m3)

    ASSERT_PARAM_VISIBLE(Lambda, false)

    ASSERT_MATRIX_C(t, _C(0.9999933,-0.0005199), _C(0.0100000,-0.0000017), _C(-0.0013514,-0.1039808), _C(0.9999933,-0.0005199))
    ASSERT_MATRIX_C(s, _C(0.9999898,-0.0010398), _C(0.0100000,-0.0000035), _C(-0.0020721,-0.2079611), _C(0.9999898,-0.0010398))
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.003);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.003)
    ASSERT_MATRIX_C(t1, _C(0.9999994,-0.0000468), _C(0.0030000,-0.0000000), _C(-0.0004005,-0.0311944), _C(0.9999994,-0.0000468))
    ASSERT_MATRIX_C(s1, _C(0.9999991,-0.0000936), _C(0.0030000,-0.0000001), _C(-0.0006019,-0.0623887), _C(0.9999991,-0.0000936))
    ASSERT_MATRIX_C(t2, _C(0.9999967,-0.0002548), _C(0.0070000,-0.0000006), _C(-0.0009395,-0.0727867), _C(0.9999967,-0.0002548))
    ASSERT_MATRIX_C(s2, _C(0.9999951,-0.0005095), _C(0.0070000,-0.0000012), _C(-0.0014247,-0.1455732), _C(0.9999951,-0.0005095))
}

// Calculation: $PROJECT/calc/ElemGauss.py
TEST_METHOD(GaussDuctSlab)
{
    ELEM(GaussDuctSlab, 7)
    SET_PARAM(L, 10, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(Lambda, 980, nm)
    SET_PARAM(n2t, 0.2, inv_m2)
    SET_PARAM(n2s, 0.3, inv_m2)
    SET_PARAM(alpha2t, 1e8, inv_m3)
    SET_PARAM(alpha2s, 2e8, inv_m3)

    ASSERT_PARAM_VISIBLE(Lambda, false)

    ASSERT_MATRIX_C(t, _C(0.9999933,-0.0005199), _C(0.0066667,-0.0000012), _C(-0.0020270,-0.1559711), _C(0.9999933,-0.0005199))
    ASSERT_MATRIX_C(s, _C(0.9999898,-0.0010398), _C(0.0066666,-0.0000023), _C(-0.0031081,-0.3119416), _C(0.9999898,-0.0010398))
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())

    elem->setSubRangeSI(0.003);
    ASSERT_EQ_DBL(elem->subRangeSI(), 0.003)
    ASSERT_MATRIX_C(t1, _C(0.9999994,-0.0000468), _C(0.0020000,-0.0000000), _C(-0.0004005,-0.0311944), _C(0.6666663,-0.0000312))
    ASSERT_MATRIX_C(s1, _C(0.9999991,-0.0000936), _C(0.0020000,-0.0000001), _C(-0.0006019,-0.0623887), _C(0.6666661,-0.0000624))
    ASSERT_MATRIX_C(t2, _C(0.9999967,-0.0002548), _C(0.0070000,-0.0000006), _C(-0.0014093,-0.1091801), _C(1.4999951,-0.0003821))
    ASSERT_MATRIX_C(s2, _C(0.9999951,-0.0005095), _C(0.0070000,-0.0000012), _C(-0.0021371,-0.2183599), _C(1.4999926,-0.0007643))
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(NormalInterface)
{
    ELEM(NormalInterface, 2)
    SET_PARAM(n1, 1.6, none)
    SET_PARAM(n2, 3.7, none)

    ASSERT_MATRIX(t, 1.0000000, 0.0000000, 0.0000000, 0.4324324)
    ASSERT_MATRIX(s, 1.0000000, 0.0000000, 0.0000000, 0.4324324)
    ASSERT_MATRIX(t_inv, 1.0000000, 0.0000000, 0.0000000, 2.3125000)
    ASSERT_MATRIX(s_inv, 1.0000000, 0.0000000, 0.0000000, 2.3125000)
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(BrewsterInterface)
{
    ELEM(BrewsterInterface, 2)
    SET_PARAM(n1, 1.6, none)
    SET_PARAM(n2, 3.7, none)

    ASSERT_MATRIX(t, 2.3125000, 0.0000000, 0.0000000, 0.1869978)
    ASSERT_MATRIX(s, 1.0000000, 0.0000000, 0.0000000, 0.4324324)
    ASSERT_MATRIX(t_inv, 0.4324324, 0.0000000, 0.0000000, 5.3476562)
    ASSERT_MATRIX(s_inv, 1.0000000, 0.0000000, 0.0000000, 2.3125000)
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(TiltedInterface)
{
    ELEM(TiltedInterface, 3)
    SET_PARAM(n1, 1.6, none)
    SET_PARAM(n2, 3.7, none)
    SET_PARAM(Alpha, 15, deg)

    ASSERT_MATRIX(t, 1.0287716, 0.0000000, 0.0000000, 0.4203386)
    ASSERT_MATRIX(s, 1.0000000, 0.0000000, 0.0000000, 0.4324324)
    ASSERT_MATRIX(t_inv, 0.9720331, 0.0000000, 0.0000000, 2.3790342)
    ASSERT_MATRIX(s_inv, 1.0000000, 0.0000000, 0.0000000, 2.3125000)

    SET_PARAM(Alpha, -15, deg)
    ASSERT_MATRIX(t, 1.2057365, 0.0000000, 0.0000000, 0.3586459)
    ASSERT_MATRIX(s, 1.0000000, 0.0000000, 0.0000000, 0.4324324)
    ASSERT_MATRIX(t_inv, 0.8293686, 0.0000000, 0.0000000, 2.7882656)
    ASSERT_MATRIX(s_inv, 1.0000000, 0.0000000, 0.0000000, 2.3125000)
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(SphericalInterface)
{
    ELEM(SphericalInterface, 3)
    SET_PARAM(n1, 1.7, none)
    SET_PARAM(n2, 3.8, none)
    SET_PARAM(R, 90, cm)

    ASSERT_MATRIX(t, 1.0000000, 0.0000000, 0.6140351, 0.4473684)
    ASSERT_MATRIX(s, 1.0000000, 0.0000000, 0.6140351, 0.4473684)
    ASSERT_MATRIX(t_inv, 1.0000000, 0.0000000, 1.3725490, 2.2352941)
    ASSERT_MATRIX(s_inv, 1.0000000, 0.0000000, 1.3725490, 2.2352941)
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(AxiconMirror)
{
    ELEM(AxiconMirror, 2)
    SET_PARAM(Theta, 5.5, deg)
    SET_PARAM(Alpha, 11, deg)

    PumpParams_RayVector pump;
    pump.radius()->setValue({11, 12, Z::Units::mm()});
    pump.angle()->setValue(0);
    pump.distance()->setValue(0);

    PumpCalculator pumpCalc(&pump, 0);
    ElementDynamic::CalcParams p;
    Z::Matrix unity;
    p.pumpCalc = &pumpCalc;
    p.Mt = &unity;
    p.Ms = &unity;
    elem->calcDynamicMatrix(p);

    ASSERT_MATRIX(t_dyn, 1.0000000, 0.0000000, -17.7799605, 1.0000000)
    ASSERT_MATRIX(s_dyn, 1.0000000, 0.0000000, -15.7049075, 1.0000000)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

// Calculation: $PROJECT/calc/Elements.py
TEST_METHOD(AxiconLens)
{
    ELEM(AxiconLens, 3)
    SET_PARAM(Theta, 5.5, deg)
    SET_PARAM(Alpha, 11, deg)
    SET_PARAM(n, 1.33, none)

    PumpParams_RayVector pump;
    pump.radius()->setValue({13, 14, Z::Units::mm()});
    pump.angle()->setValue(0);
    pump.distance()->setValue(0);

    PumpCalculator pumpCalc(&pump, 0);
    ElementDynamic::CalcParams p;
    Z::Matrix unity;
    p.pumpCalc = &pumpCalc;
    p.Mt = &unity;
    p.Ms = &unity;
    elem->calcDynamicMatrix(p);

    ASSERT_MATRIX(t_dyn, 1.0000000, 0.0000000, -2.4942520, 1.0000000)
    ASSERT_MATRIX(s_dyn, 1.0000000, 0.0000000, -2.2317667, 1.0000000)
    ASSERT_EQ_MATRIX(elem->Mt_inv(), elem->Mt())
    ASSERT_EQ_MATRIX(elem->Ms_inv(), elem->Ms())
}

//------------------------------------------------------------------------------

TEST_GROUP("Elements",
           ADD_TEST(EmptyRange),
           ADD_TEST(MediumRange),
           ADD_TEST(Plate),
           ADD_TEST(FlatMirror),
           ADD_TEST(CurveMirror),
           ADD_TEST(ThinLens),
           ADD_TEST(CylinderLensT),
           ADD_TEST(CylinderLensS),
           ADD_TEST(TiltedCrystal),
           ADD_TEST(TiltedPlate),
           ADD_TEST(BrewsterCrystal),
           ADD_TEST(BrewsterPlate),
           ADD_TEST(Matrix_1),
           ADD_TEST(Matrix_2),
           ADD_TEST(Matrix1_1),
           ADD_TEST(Matrix1_2),
           ADD_TEST(Point),
           ADD_TEST(GrinLens),
           ADD_TEST(GrinMedium),
           ADD_TEST(ThermoLens),
           ADD_TEST(ThermoMedium),
           ADD_TEST(GaussAperture),
           ADD_TEST(GaussApertureLens),
           ADD_TEST(GaussDuctMedium),
           ADD_TEST(GaussDuctSlab),
           ADD_TEST(ThickLens),
           ADD_TEST(ThickLens_flat1),
           ADD_TEST(ThickLens_flat2),
           ADD_TEST(ThickLens_flat1_flat2),
           ADD_TEST(NormalInterface),
           ADD_TEST(BrewsterInterface),
           ADD_TEST(TiltedInterface),
           ADD_TEST(SphericalInterface),
           ADD_TEST(AxiconMirror),
           ADD_TEST(AxiconLens),
           )

} // namespace ElementsTests
} // namespace Tests
} // namespace Z

