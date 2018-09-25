#include "testing/OriTestBase.h"
#include "../core/Elements.h"
#include "../core/ElementsCatalog.h"
#include "TestUtils.h"

#include <QSharedPointer>

namespace Z {
namespace Tests {
namespace ElementsTests {

#define ELEM(elem_type, expected_param_count) \
    QSharedPointer<Elem ## elem_type> elem(new Elem ## elem_type);\
    ASSERT_EQ_INT(elem->params().size(), expected_param_count)

#define SET_PARAM(name, value, unit)\
    auto param_##name = elem->params().byAlias(#name);\
    ASSERT_IS_NOT_NULL(param_##name)\
    param_##name->setValue(Z::Value(value, Z::Units::unit()));

#define ASSERT_RAW_PARAM(param, value)\
    ASSERT_NEAR_DBL(elem->param(), value, 1e-7)

#define ASSERT_MATRIX(ts, a, b, c, d)\
    ASSERT_NEAR_DBL(elem->M##ts().A, a, 1e-7)\
    ASSERT_NEAR_DBL(elem->M##ts().B, b, 1e-7)\
    ASSERT_NEAR_DBL(elem->M##ts().C, c, 1e-7)\
    ASSERT_NEAR_DBL(elem->M##ts().D, d, 1e-7)

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
    ASSERT_MATRIX(t1, 1.0000000, 0.0176000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s1, 1.0000000, 0.0176000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(t2, 1.0000000, 0.0704000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0704000, 0.0000000, 1.0000000)
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
    ASSERT_MATRIX(t1, 1.0000000, 0.0176000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s1, 1.0000000, 0.0176000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(t2, 1.0000000, 0.0704000, 0.0000000, 1.0000000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0704000, 0.0000000, 1.0000000)
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

    elem->setSubRangeSI(0.0176000);
    ASSERT_MATRIX(t1, 1.0000000, 0.0146667, 0.0000000, 0.8333333)
    ASSERT_MATRIX(s1, 1.0000000, 0.0146667, 0.0000000, 0.8333333)
    ASSERT_MATRIX(t2, 1.0000000, 0.0704000, 0.0000000, 1.2000000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0704000, 0.0000000, 1.2000000)
}

TEST_METHOD(FlatMirror)
{
    ELEM(FlatMirror, 0)
    elem->calcMatrix();
    ASSERT_MATRIX_IS_UNITY(elem->Mt())
    ASSERT_MATRIX_IS_UNITY(elem->Ms())
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

    elem->setSubRangeSI(0.0009000);
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

    elem->setSubRangeSI(0.0009137);
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

    elem->setSubRangeSI(0.0009000);
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

    elem->setSubRangeSI(0.0010817);
    ASSERT_MATRIX(t1, 1.5000000, 0.0004807, 0.0000000, 0.4444444)
    ASSERT_MATRIX(s1, 1.0000000, 0.0007211, 0.0000000, 0.6666667)
    ASSERT_MATRIX(t2, 0.6666667, 0.0028844, 0.0000000, 2.2500000)
    ASSERT_MATRIX(s2, 1.0000000, 0.0043267, 0.0000000, 1.5000000)
}

TEST_METHOD(Matrix1)
{
    ELEM(Matrix, 8)
    elem->setMatrixT(1.1, 2.2, 3.3, 4.4);
    elem->setMatrixS(5.5, 6.6, 7.7, 8.8);

    ASSERT_MATRIX(t, 1.1, 2.2, 3.3, 4.4)
    ASSERT_MATRIX(s, 5.5, 6.6, 7.7, 8.8)
}

TEST_METHOD(Matrix2)
{
    ELEM(Matrix, 8)
    SET_PARAM(At, 1.1, none)
    SET_PARAM(Bt, 2.2, none)
    SET_PARAM(Ct, 3.3, none)
    SET_PARAM(Dt, 4.4, none)
    SET_PARAM(As, 5.5, none)
    SET_PARAM(Bs, 6.6, none)
    SET_PARAM(Cs, 7.7, none)
    SET_PARAM(Ds, 8.8, none)

    ASSERT_MATRIX(t, 1.1, 2.2, 3.3, 4.4)
    ASSERT_MATRIX(s, 5.5, 6.6, 7.7, 8.8)
}

TEST_METHOD(Point)
{
    ELEM(Point, 0)
    elem->calcMatrix();
    ASSERT_MATRIX_IS_UNITY(elem->Mt())
    ASSERT_MATRIX_IS_UNITY(elem->Ms())
}

/* TODO
TEST_METHOD(grin_lens)
{

} */

// Calculation: $PROJECT/calc/ThickLens.py
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

    elem->setSubRangeSI(0.002);
    ASSERT_MATRIX(t1, 0.9925926, 0.0013333, -3.7037037, 0.6666667)
    ASSERT_MATRIX(s1, 0.9925926, 0.0013333, -3.7037037, 0.6666667)
    ASSERT_MATRIX(t2, 1.0000000, 0.0080000, -3.3333333, 1.4733333)
    ASSERT_MATRIX(s2, 1.0000000, 0.0080000, -3.3333333, 1.4733333)
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
           ADD_TEST(Matrix1),
           ADD_TEST(Matrix2),
           ADD_TEST(Point),
           // TODO ADD_TEST(grin_lens),
           ADD_TEST(ThickLens)
           )

} // namespace ElementsTests
} // namespace Tests
} // namespace Z

