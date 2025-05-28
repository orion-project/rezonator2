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

//------------------------------------------------------------------------------

TEST_GROUP("Formula",
    ADD_TEST(isValidVariableName),
    ADD_TEST(calculate),
    ADD_TEST(calculate_with_deps),
    ADD_TEST(destructor_must_unlisten_deps),
    ADD_TEST(calculate_with_units),
)

} // namespace FormulaTests
} // namespace Tests
} // namespace Z
