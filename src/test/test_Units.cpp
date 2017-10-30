#include "testing/OriTestBase.h"
#include "../core/Units.h"
#include "TestUtils.h"

using namespace Z::Units;

namespace Z {
namespace Test {
namespace UnitsTests {

#define SAME_VALUE(a, b) {                              \
    double left = a;                                    \
    double right = b;                                   \
    TEST_LOG_SEPARATOR                                  \
    TEST_LOG(QString("%1 == %2").arg(#a).arg(#b))       \
    TEST_LOG_DOUBLE(left)                               \
    TEST_LOG_DOUBLE(right)                              \
    TEST_LOG("")                                        \
    ASSERT_EQ_DBL(left, right)                          \
}

TEST_METHOD(linear_toSi)
{
    SAME_VALUE(Z::Units::Ao()->toSi(3.14), 3.14e-10)
    SAME_VALUE(Z::Units::nm()->toSi(3.14), 3.14e-9)
    SAME_VALUE(Z::Units::mkm()->toSi(3.14), 3.14e-6)
    SAME_VALUE(Z::Units::mm()->toSi(3.14), 3.14e-3)
    SAME_VALUE(Z::Units::cm()->toSi(3.14), 3.14e-2)
    SAME_VALUE(Z::Units::m()->toSi(3.14), 3.14)
}

TEST_METHOD(linear_fromSi)
{
    SAME_VALUE(Z::Units::Ao()->fromSi(3.14), 3.14e+10)
    SAME_VALUE(Z::Units::nm()->fromSi(3.14), 3.14e+9)
    SAME_VALUE(Z::Units::mkm()->fromSi(3.14), 3.14e+6)
    SAME_VALUE(Z::Units::mm()->fromSi(3.14), 3.14e+3)
    SAME_VALUE(Z::Units::cm()->fromSi(3.14), 3.14e+2)
    SAME_VALUE(Z::Units::m()->fromSi(3.14), 3.14)
}

TEST_METHOD(algular_toSi)
{
    SAME_VALUE(Z::Units::mrad()->toSi(3.14), 3.14e-3)
    SAME_VALUE(Z::Units::rad()->toSi(3.14), 3.14)
    SAME_VALUE(Z::Units::amin()->toSi(3.14), DEG_TO_RAD(3.14/60.0))
    SAME_VALUE(Z::Units::deg()->toSi(3.14), DEG_TO_RAD(3.14))
}

TEST_METHOD(algular_fromSi)
{
    SAME_VALUE(Z::Units::mrad()->fromSi(3.14), 3.14e+3)
    SAME_VALUE(Z::Units::rad()->fromSi(3.14), 3.14)
    SAME_VALUE(Z::Units::amin()->fromSi(3.14), RAD_TO_DEG(3.14)*60.0)
    SAME_VALUE(Z::Units::deg()->fromSi(3.14), RAD_TO_DEG(3.14))
}

//------------------------------------------------------------------------------

TEST_METHOD(linear_unitByAlias)
{
    auto dim = Z::Dims::linear();
    for (Z::Unit u : Z::Dims::linear()->units())  ASSERT_EQ_PTR(dim->unitByAlias(u->alias()), u);
    for (Z::Unit u : Z::Dims::angular()->units()) ASSERT_IS_NULL(dim->unitByAlias(u->alias()));
    for (Z::Unit u : Z::Dims::none()->units())    ASSERT_IS_NULL(dim->unitByAlias(u->alias()));
}

TEST_METHOD(angular_unitByAlias)
{
    auto dim = Z::Dims::angular();
    for (Z::Unit u : Z::Dims::linear()->units())  ASSERT_IS_NULL(dim->unitByAlias(u->alias()));
    for (Z::Unit u : Z::Dims::angular()->units()) ASSERT_EQ_PTR(dim->unitByAlias(u->alias()), u);
    for (Z::Unit u : Z::Dims::none()->units())    ASSERT_IS_NULL(dim->unitByAlias(u->alias()));
}

TEST_METHOD(none_unitByAlias)
{
    auto dim = Z::Dims::none();
    for (Z::Unit u : Z::Dims::linear()->units())  ASSERT_IS_NULL(dim->unitByAlias(u->alias()));
    for (Z::Unit u : Z::Dims::angular()->units()) ASSERT_IS_NULL(dim->unitByAlias(u->alias()));
    for (Z::Unit u : Z::Dims::none()->units())    ASSERT_EQ_PTR(dim->unitByAlias(u->alias()), u);
}

//------------------------------------------------------------------------------

#define TEST_SIMPLIFY(value, expected_value, expected_prefix)         \
{                                                                     \
    TEST_LOG_SEPARATOR                                                \
    double value_pos = value;                                         \
    auto prefix_pos = Z::Units::simplify(value_pos);                  \
    TEST_LOG(QString("%1 -> %2 (%3)").arg(double(value), 0, 'g', 15)  \
        .arg(value_pos, 0, 'g', 15).arg(prefix_pos))                  \
    ASSERT_EQ_INT(prefix_pos, expected_prefix)                        \
    ASSERT_EQ_DBL(value_pos, expected_value)                          \
    double value_neg = -value;                                        \
    auto prefix_neg = Z::Units::simplify(value_neg);                  \
    TEST_LOG(QString("%1 -> %2 (%3)").arg(double(-value), 0, 'g', 15) \
        .arg(value_neg, 0, 'g', 15).arg(prefix_neg))                  \
    ASSERT_EQ_INT(prefix_neg, expected_prefix)                        \
    ASSERT_EQ_DBL(value_neg, -expected_value)                         \
}

TEST_METHOD(simplify)
{
    TEST_SIMPLIFY(1, 1, Z::Units::Prefix_None);
    TEST_SIMPLIFY(10, 10, Z::Units::Prefix_None);
    TEST_SIMPLIFY(100, 100, Z::Units::Prefix_None);
    TEST_SIMPLIFY(1000, 1, Z::Units::Prefix_Kilo);
    TEST_SIMPLIFY(10000, 10, Z::Units::Prefix_Kilo);
    TEST_SIMPLIFY(100000, 100, Z::Units::Prefix_Kilo);
    TEST_SIMPLIFY(1000000, 1, Z::Units::Prefix_Mega);
    TEST_SIMPLIFY(10000000, 10, Z::Units::Prefix_Mega);
    TEST_SIMPLIFY(100000000, 100, Z::Units::Prefix_Mega);
    TEST_SIMPLIFY(1000000000, 1, Z::Units::Prefix_Giga);
    TEST_SIMPLIFY(10000000000, 10, Z::Units::Prefix_Giga);
    TEST_SIMPLIFY(100000000000, 100, Z::Units::Prefix_Giga);
    TEST_SIMPLIFY(1000000000000, 1, Z::Units::Prefix_Tera);
    TEST_SIMPLIFY(10000000000000, 10, Z::Units::Prefix_Tera);
    TEST_SIMPLIFY(100000000000000, 100, Z::Units::Prefix_Tera);
    TEST_SIMPLIFY(1000000000000000, 1000, Z::Units::Prefix_Tera);
}

//------------------------------------------------------------------------------

TEST_METHOD(prefix_name_tr)
{
    for (auto p : ENUM_VALUES(Z::Units::Prefix))
    {
        auto s = Z::Units::prefixNameTr(p);
        auto inner_name = ENUM_ITEM_NAME(Z::Units::Prefix, p);
        TEST_LOG(QString("%1(%2): %3").arg(p).arg(inner_name).arg(s));
        ASSERT_IS_FALSE(inner_name.isEmpty())
        if (p == Z::Units::Prefix_None)
            ASSERT_IS_TRUE(s.isEmpty())
        else
            ASSERT_IS_FALSE(s.isEmpty())
    }
}

TEST_METHOD(guessDim)
{
    for (Z::Unit u : Z::Dims::linear()->units())
        ASSERT_EQ_PTR(Z::Units::guessDim(u), Z::Dims::linear());
    for (Z::Unit u : Z::Dims::angular()->units())
        ASSERT_EQ_PTR(Z::Units::guessDim(u), Z::Dims::angular());
    for (Z::Unit u : Z::Dims::none()->units())
        ASSERT_EQ_PTR(Z::Units::guessDim(u), Z::Dims::none());
}

TEST_METHOD(findByAlias)
{
    for (Z::Dim dim : Z::Dims::dims())
        for (Z::Unit unit : dim->units())
        {
            auto foundUnit = Z::Units::findByAlias(unit->alias());
            ASSERT_EQ_PTR(foundUnit, unit)
        }
    ASSERT_IS_NULL(Z::Units::findByAlias("there is not such unit"))
}

//------------------------------------------------------------------------------

TEST_GROUP("Units",
    ADD_TEST(linear_toSi),
    ADD_TEST(linear_fromSi),
    ADD_TEST(algular_toSi),
    ADD_TEST(algular_fromSi),
    ADD_TEST(linear_unitByAlias),
    ADD_TEST(angular_unitByAlias),
    ADD_TEST(none_unitByAlias),
    ADD_TEST(simplify),
    ADD_TEST(prefix_name_tr),
    ADD_TEST(guessDim),
    ADD_TEST(findByAlias)
)

} // namespace UnitsTests
} // namespace Test
} // namespace Z
