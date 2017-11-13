#include "testing/OriTestBase.h"
#include "../core/Elements.h"
#include "../core/ElementsCatalog.h"
#include "TestUtils.h"

#include <QSharedPointer>

namespace Z {
namespace Test {
namespace ElementsTests {

#define ELEM(elem_type, expected_param_count) \
    QSharedPointer<Elem ## elem_type> elem(new Elem ## elem_type);\
    ASSERT_EQ_INT(elem->params().size(), expected_param_count)

#define SET_PARAM(name, value, unit)\
    auto param_##name = elem->params().byAlias(#name);\
    ASSERT_IS_NOT_NULL(param_##name)\
    param_##name->setValue(Z::Value(value, Z::Units::unit()));

#define ASSERT_Mt(a, b, c, d) ASSERT_MATRIX_IS(elem->Mt(), a, b, c, d)
#define ASSERT_Ms(a, b, c, d) ASSERT_MATRIX_IS(elem->Ms(), a, b, c, d)

#define ASSERT_MULT_M1_M2(L)\
    const double L1 = L*0.2;\
    const double L2 = L*0.8;\
    elem->setSubRangeSI(L1);\
    auto mt = elem->Mt2() * elem->Mt1();\
    auto ms = elem->Ms2() * elem->Ms1();\
    ASSERT_EQ_MATRIX(mt, elem->Mt())\
    ASSERT_EQ_MATRIX(ms, elem->Ms())\
    Q_UNUSED(L2)

////////////////////////////////////////////////////////////////////////////////

TEST_METHOD(EmptyRange)
{
    ELEM(EmptyRange, 1)
    SET_PARAM(L, 88, mm)

    const double L = 0.088;
    ASSERT_EQ_DBL(elem->lengthSI(), L)
    ASSERT_EQ_DBL(elem->axisLengthSI(), L)
    ASSERT_EQ_DBL(elem->opticalPathSI(), L)

    ASSERT_MATRIX_IS(elem->Mt(), 1, L, 0, 1)
    ASSERT_EQ_MATRIX(elem->Ms(), elem->Mt())

    ASSERT_MULT_M1_M2(L)
    ASSERT_MATRIX_IS(elem->Mt1(), 1, L1, 0, 1)
    ASSERT_MATRIX_IS(elem->Mt2(), 1, L2, 0, 1)
    ASSERT_EQ_MATRIX(elem->Ms1(), elem->Mt1())
    ASSERT_EQ_MATRIX(elem->Ms2(), elem->Mt2())
}

TEST_METHOD(MediumRange)
{
    ELEM(MediumRange, 2)
    SET_PARAM(L, 88, mm)
    SET_PARAM(n, 1.2, none)

    const double L = 0.088;
    const double n = 1.2;
    ASSERT_EQ_DBL(elem->lengthSI(), L)
    ASSERT_EQ_DBL(elem->ior(), n)
    ASSERT_EQ_DBL(elem->axisLengthSI(), L)
    ASSERT_EQ_DBL(elem->opticalPathSI(), L*n)

    ASSERT_MATRIX_IS(elem->Mt(), 1, L, 0, 1)
    ASSERT_EQ_MATRIX(elem->Ms(), elem->Mt())

    ASSERT_MULT_M1_M2(L)
    ASSERT_MATRIX_IS(elem->Mt1(), 1, L1, 0, 1)
    ASSERT_MATRIX_IS(elem->Mt2(), 1, L2, 0, 1)
    ASSERT_EQ_MATRIX(elem->Ms1(), elem->Mt1())
    ASSERT_EQ_MATRIX(elem->Ms2(), elem->Mt2())
}

TEST_METHOD(Plate)
{
    ELEM(Plate, 2)
    SET_PARAM(L, 88, mm)
    SET_PARAM(n, 1.2, none)

    const double L = 0.088;
    const double n = 1.2;
    ASSERT_EQ_DBL(elem->lengthSI(), L)
    ASSERT_EQ_DBL(elem->ior(), n)
    ASSERT_EQ_DBL(elem->axisLengthSI(), L)
    ASSERT_EQ_DBL(elem->opticalPathSI(), L*n)

    ASSERT_MATRIX_IS(elem->Mt(), 1, L/n, 0, 1)
    ASSERT_EQ_MATRIX(elem->Ms(), elem->Mt())

    ASSERT_MULT_M1_M2(L)
    ASSERT_MATRIX_IS(elem->Mt1(), 1, L1/n, 0, 1/n)
    ASSERT_MATRIX_IS(elem->Mt2(), 1, L2, 0, n)
    ASSERT_EQ_MATRIX(elem->Ms1(), elem->Mt1())
    ASSERT_EQ_MATRIX(elem->Ms2(), elem->Mt2())
}

TEST_METHOD(FlatMirror)
{
    ELEM(FlatMirror, 0)
    elem->calcMatrix();
    ASSERT_MATRIX_IS_UNITY(elem->Mt())
    ASSERT_MATRIX_IS_UNITY(elem->Ms())
}

TEST_METHOD(CurveMirror)
{
    ELEM(CurveMirror, 2)

    SET_PARAM(R, 100, mm)
    SET_PARAM(Alpha, 15, deg)

    const double R = 0.1;
    const double alpha = DEG_TO_RAD(15);
    ASSERT_Mt(1, 0, -2/(R*cos(alpha)), 1)
    ASSERT_Ms(1, 0, -2*cos(alpha)/R, 1)
}

TEST_METHOD(ThinLens)
{
    ELEM(ThinLens, 2)

    SET_PARAM(F, 100, mm)
    SET_PARAM(Alpha, 15, deg)

    const double F = 0.1;
    const double alpha = DEG_TO_RAD(15);
    ASSERT_Mt(1, 0, -1/(F*cos(alpha)), 1)
    ASSERT_Ms(1, 0, -cos(alpha)/F, 1)
}

TEST_METHOD(CylinderLensT)
{
    ELEM(CylinderLensT, 2)

    SET_PARAM(F, 100, mm)
    SET_PARAM(Alpha, 15, deg)

    const double F = 0.1;
    const double alpha = DEG_TO_RAD(15);
    ASSERT_Mt(1, 0, -1/(F*cos(alpha)), 1)
    ASSERT_Ms(1, 0, 0, 1)
}

TEST_METHOD(CylinderLensS)
{
    ELEM(CylinderLensS, 2)

    SET_PARAM(F, 100, mm)
    SET_PARAM(Alpha, 15, deg)

    const double F = 0.1;
    const double alpha = DEG_TO_RAD(15);
    ASSERT_Mt(1, 0, 0, 1)
    ASSERT_Ms(1, 0, -cos(alpha)/F, 1)
}

TEST_METHOD(TiltedCrystal)
{
    ELEM(TiltedCrystal, 3)
    SET_PARAM(L, 4.5, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(Alpha, 15, deg)

    const double L = 0.0045;
    const double a = DEG_TO_RAD(15);
    const double n = 1.5;
    ASSERT_EQ_DBL(elem->lengthSI(), L)
    ASSERT_EQ_DBL(elem->ior(), n)
    ASSERT_EQ_DBL(elem->alpha(), a)
    ASSERT_EQ_DBL(elem->axisLengthSI(), L)
    ASSERT_EQ_DBL(elem->opticalPathSI(), L*n)

    // B = n * L * cos(a)^2 / ( n^2 - sin(a)^2 )
    ASSERT_MATRIX_IS(elem->Mt(), 1, n * L * SQR(cos(a)) / ( n*n - SQR(sin(a)) ), 0, 1)
    ASSERT_MATRIX_IS(elem->Ms(), 1, L/n, 0, 1)

    ASSERT_MULT_M1_M2(L)
}

TEST_METHOD(TiltedPlate)
{
    ELEM(TiltedPlate, 3)
    SET_PARAM(L, 4.5, mm)
    SET_PARAM(n, 1.5, none)
    SET_PARAM(Alpha, 15, deg)

    const double L = 0.0045;
    const double a = DEG_TO_RAD(15);
    const double n = 1.5;
    const double L_eff = L/cos( asin( sin(a) / n ) );
    ASSERT_EQ_DBL(elem->lengthSI(), L)
    ASSERT_EQ_DBL(elem->ior(), n)
    ASSERT_EQ_DBL(elem->alpha(), a)
    ASSERT_EQ_DBL(elem->axisLengthSI(), L_eff)
    ASSERT_EQ_DBL(elem->opticalPathSI(), L_eff*n)

    double s = n*n - SQR(sin(a));
    ASSERT_MATRIX_IS(elem->Mt(), 1, L * n*n * (1 - SQR(sin(a))) / sqrt(s*s*s), 0, 1)
    ASSERT_MATRIX_IS(elem->Ms(), 1, L/sqrt(s), 0, 1)

    ASSERT_MULT_M1_M2(L_eff)
}

TEST_METHOD(BrewsterCrystal)
{
    ELEM(BrewsterCrystal, 2)
    SET_PARAM(L, 4.5, mm)
    SET_PARAM(n, 1.5, none)

    const double L = 0.0045;
    const double n = 1.5;
    ASSERT_EQ_DBL(elem->ior(), n)
    ASSERT_EQ_DBL(elem->axisLengthSI(), L)
    ASSERT_EQ_DBL(elem->opticalPathSI(), L*n)

    ASSERT_MATRIX_IS(elem->Mt(), 1, L/(n*n*n), 0, 1)
    ASSERT_MATRIX_IS(elem->Ms(), 1, L/n, 0, 1)

    ASSERT_MULT_M1_M2(L)
}

TEST_METHOD(BrewsterPlate)
{
    ELEM(BrewsterPlate, 2)
    SET_PARAM(L, 4.5, mm)
    SET_PARAM(n, 1.5, none)

    const double L = 0.0045;
    const double n = 1.5;
    const double L_eff = L * sqrt(n * n + 1) / n;
    ASSERT_EQ_DBL(elem->ior(), n)
    ASSERT_EQ_DBL(elem->axisLengthSI(), L_eff)
    ASSERT_EQ_DBL(elem->opticalPathSI(), L_eff*n)

    ASSERT_MATRIX_IS(elem->Mt(), 1, L*sqrt(n*n + 1)/(n*n*n*n), 0, 1)
    ASSERT_MATRIX_IS(elem->Ms(), 1, L*sqrt(n*n + 1)/(n*n), 0, 1)

    ASSERT_MULT_M1_M2(L_eff)
}

TEST_METHOD(Matrix1)
{
    ELEM(Matrix, 8)

    elem->setMatrixT(1.1, 2.2, 3.3, 4.4);
    ASSERT_Mt(1.1, 2.2, 3.3, 4.4)

    elem->setMatrixS(5.5, 6.6, 7.7, 8.8);
    ASSERT_Ms(5.5, 6.6, 7.7, 8.8)
}

TEST_METHOD(Matrix2)
{
    ELEM(Matrix, 8)

    SET_PARAM(At, 1.1, none)
    SET_PARAM(Bt, 2.2, none)
    SET_PARAM(Ct, 3.3, none)
    SET_PARAM(Dt, 4.4, none)
    ASSERT_Mt(1.1, 2.2, 3.3, 4.4)

    SET_PARAM(As, 5.5, none)
    SET_PARAM(Bs, 6.6, none)
    SET_PARAM(Cs, 7.7, none)
    SET_PARAM(Ds, 8.8, none)
    ASSERT_Ms(5.5, 6.6, 7.7, 8.8)
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

////////////////////////////////////////////////////////////////////////////////

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
           )

} // namespace ElementsTests
} // namespace Test
} // namespace Z

