#include "../core/Formula.h"
#include "../tests/TestUtils.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace FormulaTests {

TEST_METHOD(isValidVariableName)
{
    ASSERT_IS_TRUE(Z::FormulaUtils::isValidVariableName("a"))
    ASSERT_IS_TRUE(Z::FormulaUtils::isValidVariableName("a1"))
    ASSERT_IS_FALSE(Z::FormulaUtils::isValidVariableName("a+1"))
    ASSERT_IS_FALSE(Z::FormulaUtils::isValidVariableName("a*1"))
    ASSERT_IS_TRUE(Z::FormulaUtils::isValidVariableName("a_1"))
    ASSERT_IS_FALSE(Z::FormulaUtils::isValidVariableName("1"))
    ASSERT_IS_TRUE(Z::FormulaUtils::isValidVariableName("_1"))
}

TEST_METHOD(calculate)
{
    Z::Parameter p;
    Z::Formula f(&p);
    f.setCode("2+2");
    f.calculate();
    ASSERT_IS_TRUE(f.ok());
    ASSERT_EQ_DBL(p.value().value(), 4);
    ASSERT_EQ_INT(p.valueDriver(), ParamValueDriver::Formula);
}

TEST_METHOD(calculate_with_deps)
{
    Z::Parameter p;
    Z::Parameter p1(Z::Dims::none(), "p1");
    Z::Parameter p2(Z::Dims::none(), "p2");
    p1.setValue(2);
    p2.setValue(3);

    Z::Formula f(&p);
    f.addDep(&p1);
    f.addDep(&p2);
    f.setCode("p1+p2");
    f.calculate();
    ASSERT_IS_TRUE(f.ok());
    ASSERT_EQ_DBL(p.value().value(), 5);
    
    ASSERT_IS_TRUE(p1.listeners().contains(&f));
    ASSERT_IS_TRUE(p2.listeners().contains(&f));
    
    p1.setValue(3);
    ASSERT_EQ_DBL(p.value().value(), 6);

    p2.setValue(4);
    ASSERT_EQ_DBL(p.value().value(), 7);
}

TEST_METHOD(destructor_must_unlisten_deps)
{
    Z::Parameter p;
    Z::Parameter p1(Z::Dims::none(), "p1");
    Z::Parameter p2(Z::Dims::none(), "p2");
    
    auto f = new Z::Formula(&p);
    f->addDep(&p1);
    f->addDep(&p2);
    f->setCode("p1+p2");
    f->calculate();
    ASSERT_IS_TRUE(f->ok());
    delete f;
    
    ASSERT_IS_FALSE(p1.listeners().contains(f));
    ASSERT_IS_FALSE(p2.listeners().contains(f));
    ASSERT_EQ_INT(p.valueDriver(), ParamValueDriver::None);
}

TEST_METHOD(calculate_with_units)
{
    Z::Parameter p(Z::Dims::linear(), "");
    Z::Parameter p1(Z::Dims::linear(), "p1");
    Z::Parameter p2(Z::Dims::linear(), "p2");
    p.setValue(1_m);
    p1.setValue(150_cm);
    p2.setValue(5_mm);

    Z::Formula f(&p);
    f.addDep(&p1);
    f.addDep(&p2);
    f.setCode("p1+p2");
    f.calculate();

    ASSERT_IS_TRUE(f.ok());
    ASSERT_EQ_ZVALUE(p.value(), 1.505_m);
}

TEST_CASE_METHOD(renameDependency, QString code, QString expectedCode, bool expectedFound)
{
    Z::Parameter tgt;
    Z::Formula f(&tgt);
    Z::Parameter dep("a_1");
    f.addDep(&dep);
    f.setCode(code);
    bool ok = f.renameDependency(&dep, "a_2");
    if (expectedFound)
        ASSERT_IS_TRUE(ok)
    else
        ASSERT_IS_FALSE(ok)
    ASSERT_EQ_STR(f.code(), expectedCode)
}

TEST_CASE(renameDependency_none, renameDependency, "a+b+c", "a+b+c", false)
TEST_CASE(renameDependency_beg, renameDependency, "a_1+b+c", "a_2+b+c", true)
TEST_CASE(renameDependency_mid, renameDependency, "b+ a_1 +c", "b+ a_2 +c", true)
TEST_CASE(renameDependency_end, renameDependency, "b+c +a_1", "b+c +a_2", true)
TEST_CASE(renameDependency_several, renameDependency, "a=a_1 ans=a+b + a_1^2 + c", "a=a_2 ans=a+b + a_2^2 + c", true)
TEST_CASE(renameDependency_func, renameDependency, "a=sin(a_1) ans=a-abs(-a_1)", "a=sin(a_2) ans=a-abs(-a_2)", true)

//------------------------------------------------------------------------------

TEST_GROUP("Formula",
    ADD_TEST(isValidVariableName),
    ADD_TEST(calculate),
    ADD_TEST(calculate_with_deps),
    ADD_TEST(destructor_must_unlisten_deps),
    ADD_TEST(calculate_with_units),
    ADD_TEST(renameDependency_none),
    ADD_TEST(renameDependency_beg),
    ADD_TEST(renameDependency_mid),
    ADD_TEST(renameDependency_end),
    ADD_TEST(renameDependency_several),
    ADD_TEST(renameDependency_func),
)

} // namespace FormulaTests
} // namespace Tests
} // namespace Z
