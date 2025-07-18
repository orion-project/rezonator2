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
    Z::Parameter tgt;
    Z::Formula f(&tgt);
    f.setCode("2+2");
    f.calculate();
    ASSERT_IS_TRUE(f.ok());
    ASSERT_EQ_DBL(tgt.value().value(), 4);
    ASSERT_EQ_INT(tgt.valueDriver(), ParamValueDriver::Formula);
}

TEST_METHOD(calculate_with_deps)
{
    Z::Parameter tgt;
    Z::Parameter dep1(Z::Dims::none(), "p1");
    Z::Parameter dep2(Z::Dims::none(), "p2");
    dep1.setValue(2);
    dep2.setValue(3);

    Z::Formula f(&tgt);
    f.addDep(&dep1);
    f.addDep(&dep2);
    f.setCode("p1+p2");
    f.calculate();
    ASSERT_IS_TRUE(f.ok());
    ASSERT_EQ_DBL(tgt.value().value(), 5);
    
    ASSERT_IS_TRUE(dep1.listeners().contains(&f));
    ASSERT_IS_TRUE(dep2.listeners().contains(&f));
    
    dep1.setValue(3);
    ASSERT_EQ_DBL(tgt.value().value(), 6);

    dep2.setValue(4);
    ASSERT_EQ_DBL(tgt.value().value(), 7);
}

TEST_METHOD(destructor_must_unlisten_deps)
{
    Z::Parameter tgt;
    Z::Parameter dep1(Z::Dims::none(), "p1");
    Z::Parameter dep2(Z::Dims::none(), "p2");
    
    auto f = new Z::Formula(&tgt);
    f->addDep(&dep1);
    f->addDep(&dep2);
    f->setCode("p1+p2");
    f->calculate();
    ASSERT_IS_TRUE(f->ok());
    delete f;
    
    ASSERT_IS_FALSE(dep1.listeners().contains(f));
    ASSERT_IS_FALSE(dep2.listeners().contains(f));
    ASSERT_EQ_INT(tgt.valueDriver(), ParamValueDriver::None);
}

TEST_METHOD(calculate_with_units)
{
    Z::Parameter tgt(Z::Dims::linear(), "");
    Z::Parameter dep1(Z::Dims::linear(), "p1");
    Z::Parameter dep2(Z::Dims::linear(), "p2");
    tgt.setValue(1_m);
    dep1.setValue(150_cm);
    dep2.setValue(5_mm);

    Z::Formula f(&tgt);
    f.addDep(&dep1);
    f.addDep(&dep2);
    f.setCode("p1+p2");
    f.calculate();

    ASSERT_IS_TRUE(f.ok());
    ASSERT_EQ_ZVALUE(tgt.value(), 1.505_m);
}

TEST_METHOD(calculate_sets_error_when_empty)
{
    Z::Parameter tgt;
    Z::Formula f(&tgt);

    ASSERT_IS_TRUE(f.ok())
    ASSERT_IS_FALSE(tgt.failed())

    f.calculate();
    ASSERT_IS_FALSE(f.ok())
    ASSERT_IS_TRUE(tgt.failed())
    ASSERT_IS_TRUE(tgt.error().contains("empty"))
}

TEST_METHOD(calculate_sets_error_when_bad_code)
{
    Z::Parameter tgt;
    Z::Formula f(&tgt);

    ASSERT_IS_TRUE(f.ok())
    ASSERT_IS_FALSE(tgt.failed())

    f.setCode("2+p1");
    f.calculate();

    ASSERT_IS_FALSE(f.ok())
    ASSERT_IS_TRUE(tgt.failed())
}

TEST_METHOD(calculate_sets_error_when_dep_failed)
{
    Z::Parameter tgt;
    Z::Parameter dep("p1");
    Z::Formula f(&tgt);
    f.addDep(&dep);

    ASSERT_IS_TRUE(f.ok())
    ASSERT_IS_FALSE(tgt.failed())

    f.setCode("2+p1");
    f.calculate();
    ASSERT_IS_TRUE(f.ok())
    ASSERT_IS_FALSE(tgt.failed())
    
    dep.setValue(1);
    ASSERT_IS_TRUE(f.ok())
    ASSERT_IS_FALSE(tgt.failed())

    dep.setError("dep-error");
    ASSERT_IS_FALSE(f.ok())
    ASSERT_IS_TRUE(tgt.failed())
    ASSERT_IS_TRUE(tgt.error().contains("p1"))
    ASSERT_IS_TRUE(tgt.error().contains("dep-error"))
}

TEST_CASE_METHOD(renameDep, QString code, QString expectedCode, bool expectedFound)
{
    Z::Parameter tgt;
    Z::Formula f(&tgt);
    Z::Parameter dep("a_1");
    f.addDep(&dep);
    f.setCode(code);
    bool ok = f.renameDep(&dep, "a_2");
    if (expectedFound)
        ASSERT_IS_TRUE(ok)
    else
        ASSERT_IS_FALSE(ok)
    ASSERT_EQ_STR(f.code(), expectedCode)
}

TEST_CASE(renameDep_none, renameDep, "a+b+c", "a+b+c", false)
TEST_CASE(renameDep_beg, renameDep, "a_1+b+c", "a_2+b+c", true)
TEST_CASE(renameDep_mid, renameDep, "b+ a_1 +c", "b+ a_2 +c", true)
TEST_CASE(renameDep_end, renameDep, "b+c +a_1", "b+c +a_2", true)
TEST_CASE(renameDep_several, renameDep, "a=a_1 ans=a+b + a_1^2 + c", "a=a_2 ans=a+b + a_2^2 + c", true)
TEST_CASE(renameDep_func, renameDep, "a=sin(a_1) ans=a-abs(-a_1)", "a=sin(a_2) ans=a-abs(-a_2)", true)

TEST_CASE_METHOD(findDeps, QString code, QStringList expectedDeps)
{
    Parameter pa("a");
    Parameter pb("b");
    Parameter pc("c");
    Parameter pd("d");
    Parameters globals { &pa, &pb, &pc, &pd };
    
    Parameter tgt;
    Formula f(&tgt);
    f.addDep(&pa);
    f.addDep(&pb);
    f.setCode(code);
    f.findDeps(globals);
    QStringList foundDeps;
    for (auto d : f.deps())
        foundDeps << d->alias();
    ASSERT_EQ_INT(foundDeps.size(), expectedDeps.size())
    for (auto d : f.deps())
        ASSERT_IS_TRUE(expectedDeps.contains(d->alias()))
    for (const QString &d : expectedDeps) {
        ASSERT_IS_TRUE(foundDeps.contains(d))
    }
}

TEST_CASE(findDeps_keep_old, findDeps, "a+b", {"a", "b"})
TEST_CASE(findDeps_keep_non_existent, findDeps, "a+b+e", {"a", "b"})
TEST_CASE(findDeps_remove_one, findDeps, "a+2", {"a"})
TEST_CASE(findDeps_remove_all, findDeps, "2+2", {})
TEST_CASE(findDeps_add_one, findDeps, "a+b+c", {"a", "b", "c"});
TEST_CASE(findDeps_add_several, findDeps, "a+b+c+d", {"a", "b", "c", "d"});
TEST_CASE(findDeps_replace_one, findDeps, "a+c", {"a", "c"});
TEST_CASE(findDeps_replace_several, findDeps, "c+d", {"c", "d"});

//------------------------------------------------------------------------------

TEST_GROUP("Formula",
    ADD_TEST(isValidVariableName),
    ADD_TEST(calculate),
    ADD_TEST(calculate_with_deps),
    ADD_TEST(destructor_must_unlisten_deps),
    ADD_TEST(calculate_with_units),
    ADD_TEST(calculate_sets_error_when_empty),
    ADD_TEST(calculate_sets_error_when_bad_code),
    ADD_TEST(calculate_sets_error_when_dep_failed),
    ADD_TEST(renameDep_none),
    ADD_TEST(renameDep_beg),
    ADD_TEST(renameDep_mid),
    ADD_TEST(renameDep_end),
    ADD_TEST(renameDep_several),
    ADD_TEST(renameDep_func),
    ADD_TEST(findDeps_keep_old),
    ADD_TEST(findDeps_keep_non_existent),
    ADD_TEST(findDeps_remove_one),
    ADD_TEST(findDeps_remove_all),
    ADD_TEST(findDeps_add_one),
    ADD_TEST(findDeps_add_several),
    ADD_TEST(findDeps_replace_one),
    ADD_TEST(findDeps_replace_several),
)

} // namespace FormulaTests
} // namespace Tests
} // namespace Z
