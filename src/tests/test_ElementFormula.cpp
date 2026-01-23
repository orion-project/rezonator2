#include "../core/ElementFormula.h"
#include "../tests/TestUtils.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace ElementFormulaTests {

namespace {
class TestParam : public Z::Parameter
{
public:
    TestParam(Ori::Testing::TestBase *test, const QString& name): Z::Parameter(Z::Dims::none(), name), test(test) {}
    ~TestParam() override {
        TEST_LOG("~TestParam() " + alias())
        SET_TEST_DATA(QString("param destructor %1").arg(alias()), true);
    }
    Ori::Testing::TestBase *test;
};
}

#define ASSERT_PARAM_DESTRUCTED(param_alias) \
    ASSERT_IS_TRUE(test->data()[QString("param destructor %1").arg(param_alias)].toBool())

#define ADD_PARAM(name, value) \
    auto p_##name = new TestParam(test, #name);\
    p_##name->setValue(value);\
    elem.addParam(p_##name);

#define ASSERT_MATRIX(ts, a, b, c, d)\
    TEST_LOG(elem.failReason())\
    ASSERT_IS_FALSE(elem.failed())\
    ASSERT_EQ_DBL(elem.M##ts().A.real(), a)\
    ASSERT_EQ_DBL(elem.M##ts().B.real(), b)\
    ASSERT_EQ_DBL(elem.M##ts().C.real(), c)\
    ASSERT_EQ_DBL(elem.M##ts().D.real(), d)\
    ASSERT_EQ_DBL(elem.M##ts().A.imag(), 0)\
    ASSERT_EQ_DBL(elem.M##ts().B.imag(), 0)\
    ASSERT_EQ_DBL(elem.M##ts().C.imag(), 0)\
    ASSERT_EQ_DBL(elem.M##ts().D.imag(), 0)

#define CALC_TEST_MATRIX(code)\
    elem.setFormula(code);\
    elem.calcMatrix("test");

#define ASSERT_ERROR(err) {}\
    TEST_LOG(elem.failReason())\
    ASSERT_IS_TRUE(elem.failed())\
    QString expected_error(err);\
    if (!expected_error.isEmpty())\
        ASSERT_EQ_STR(elem.failReason(), expected_error)


TEST_METHOD(can_calculate_matrix)
{
    ElemFormula elem;

    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ADD_PARAM(c, 3)
    ADD_PARAM(d, 4)

    CALC_TEST_MATRIX("At=a; Bt=b; Ct=c; Dt=d; As=10*a; Bs=10*b; Cs=10*c; Ds=10*d;")

    ASSERT_MATRIX(t, 1, 2, 3, 4)
    ASSERT_MATRIX(s, 10, 20, 30, 40)
}

TEST_METHOD(matrix_must_be_unity_when_empty_formula)
{
    ElemFormula elem;
    CALC_TEST_MATRIX("")
    ASSERT_ERROR("Formula is empty")
    ASSERT_MATRIX_IS_UNITY(elem.Mt())
    ASSERT_MATRIX_IS_UNITY(elem.Ms())
}

TEST_METHOD(matrix_must_be_unity_when_invalid_formula)
{
    ElemFormula elem;
    CALC_TEST_MATRIX("A=1; B=2; C=3; D=unknown_func(4);")
    ASSERT_ERROR("Unknown function 'unknown_func'")
    ASSERT_MATRIX_IS_UNITY(elem.Mt())
    ASSERT_MATRIX_IS_UNITY(elem.Ms())
}

TEST_CASE_METHOD(matrix_must_be_unity_when_no_abcd, bool hasTS, const char* missedName)
{
    ElemFormula elem;
    QString code("A=1; B=2; C=3; D=4; At=5; Bt=6; Ct=7; Dt=8; As=9; Bs=10; Cs=11; Ds=12;");
    code.replace(missedName, "dummy");
    CALC_TEST_MATRIX(code)
    ASSERT_ERROR(QString("Formula doesn't contain an expression for '%1' or it is not a number").arg(missedName))
    ASSERT_MATRIX_IS_UNITY(elem.Mt())
    ASSERT_MATRIX_IS_UNITY(elem.Ms())
}
TEST_CASE(matrix_must_be_unity_when_no_at, matrix_must_be_unity_when_no_abcd, true, "At")
TEST_CASE(matrix_must_be_unity_when_no_bt, matrix_must_be_unity_when_no_abcd, true, "Bt")
TEST_CASE(matrix_must_be_unity_when_no_ct, matrix_must_be_unity_when_no_abcd, true, "Ct")
TEST_CASE(matrix_must_be_unity_when_no_dt, matrix_must_be_unity_when_no_abcd, true, "Dt")
TEST_CASE(matrix_must_be_unity_when_no_as, matrix_must_be_unity_when_no_abcd, true, "As")
TEST_CASE(matrix_must_be_unity_when_no_bs, matrix_must_be_unity_when_no_abcd, true, "Bs")
TEST_CASE(matrix_must_be_unity_when_no_cs, matrix_must_be_unity_when_no_abcd, true, "Cs")
TEST_CASE(matrix_must_be_unity_when_no_ds, matrix_must_be_unity_when_no_abcd, true, "Ds")

TEST_METHOD(params_in_formula_must_be_in_si_units)
{
    ElemFormula elem;
    auto p = new Z::Parameter(Z::Dims::linear(), "a");
    p->setValue(10_cm);
    elem.addParam(p);
    CALC_TEST_MATRIX("At=a; Bt=a; Ct=a; Dt=a; As=a; Bs=a; Cs=a; Ds=a")
    ASSERT_MATRIX(t, 0.1, 0.1, 0.1, 0.1)
}

//------------------------------------------------------------------------------

#define ASSERT_PARAMS(expected_names) {\
    QStringList strs; \
    for (auto p : elem.params()) strs << p->alias();\
    QString existed_names = strs.join(", ");\
    ASSERT_EQ_STR(existed_names, expected_names)\
}

TEST_METHOD(addParam__must_do_nothing_if_param_already_added)
{
    ElemFormula elem;
    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ASSERT_PARAMS("a, b")

    elem.addParam(p_a);
    ASSERT_PARAMS("a, b")
}

TEST_METHOD(removeParam__must_destruct)
{
    ElemFormula elem;
    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ASSERT_PARAMS("a, b")

    elem.removeParam(p_a);
    ASSERT_PARAM_DESTRUCTED("a")
    ASSERT_PARAMS("b")
}

TEST_METHOD(moveParamUp)
{
    ElemFormula elem;
    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ADD_PARAM(c, 3)
    ADD_PARAM(d, 4)

    elem.moveParamUp(p_b);
    ASSERT_PARAMS("b, a, c, d")

    elem.moveParamUp(p_b);
    ASSERT_PARAMS("a, c, d, b")

    elem.moveParamUp(p_b);
    ASSERT_PARAMS("a, c, b, d")
}

TEST_METHOD(moveParamDown)
{
    ElemFormula elem;
    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ADD_PARAM(c, 3)
    ADD_PARAM(d, 4)

    elem.moveParamDown(p_c);
    ASSERT_PARAMS("a, b, d, c")

    elem.moveParamDown(p_c);
    ASSERT_PARAMS("c, a, b, d")

    elem.moveParamDown(p_c);
    ASSERT_PARAMS("a, c, b, d")
}

//------------------------------------------------------------------------------

TEST_METHOD(assign__must_copy_params_and_props)
{
    ElemFormula elem;
    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    elem.setFormula("some code");

    ElemFormula elem1;
    elem1.assign(&elem);
    ASSERT_EQ_STR(elem1.formula(), elem.formula())
    ASSERT_EQ_INT(elem1.params().size(), elem.params().size())
    for (int i = 0; i < elem1.params().size(); i++)
    {
        auto p = elem.params().at(i);
        auto p1 = elem1.params().at(i);
        ASSERT_EQ_STR(p1->alias(), p->alias())
        ASSERT_IS_TRUE(p1->value() == p->value())
    }
}

TEST_METHOD(assign__must_destruct_old_params)
{
    ElemFormula elem;
    ADD_PARAM(a, 1)
    ADD_PARAM(b, 2)
    ASSERT_PARAMS("a, b")

    ElemFormula elem1;
    elem.assign(&elem1);
    ASSERT_PARAM_DESTRUCTED("a")
    ASSERT_PARAM_DESTRUCTED("b")
    ASSERT_PARAMS("")
}

//------------------------------------------------------------------------------

TEST_GROUP("ElementFormula",
           ADD_TEST(can_calculate_matrix),
           ADD_TEST(matrix_must_be_unity_when_empty_formula),
           ADD_TEST(matrix_must_be_unity_when_invalid_formula),
           ADD_TEST(matrix_must_be_unity_when_no_at),
           ADD_TEST(matrix_must_be_unity_when_no_bt),
           ADD_TEST(matrix_must_be_unity_when_no_ct),
           ADD_TEST(matrix_must_be_unity_when_no_dt),
           ADD_TEST(matrix_must_be_unity_when_no_as),
           ADD_TEST(matrix_must_be_unity_when_no_bs),
           ADD_TEST(matrix_must_be_unity_when_no_cs),
           ADD_TEST(matrix_must_be_unity_when_no_ds),
           ADD_TEST(params_in_formula_must_be_in_si_units),
           ADD_TEST(addParam__must_do_nothing_if_param_already_added),
           ADD_TEST(removeParam__must_destruct),
           ADD_TEST(moveParamUp),
           ADD_TEST(moveParamDown),
           ADD_TEST(assign__must_copy_params_and_props),
           ADD_TEST(assign__must_destruct_old_params),
           )

} // namespace ElementFormulaTests
} // namespace Tests
} // namespace Z
