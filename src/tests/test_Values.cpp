#include "testing/OriTestBase.h"
#include "../core/Values.h"
#include "../core/Units.h"
#include "TestUtils.h"

namespace Z {
namespace Tests {
namespace ValuesTests {

#define ASSERT_VALUE_T_S(value, expectedT, expectedS)\
    ASSERT_EQ_DBL(value.T, expectedT)\
    ASSERT_EQ_DBL(value.S, expectedS)

//------------------------------------------------------------------------------

TEST_METHOD(Value_ctor)
{
    Z::Value v1(3.14, Z::Units::mkm());
    ASSERT_EQ_DBL(v1.value(), 3.14)
    ASSERT_EQ_UNIT(v1.unit(), Z::Units::mkm())

    Z::Value v2(v1);
    ASSERT_EQ_DBL(v1.value(), 3.14)
    ASSERT_EQ_UNIT(v1.unit(), Z::Units::mkm())
}

//------------------------------------------------------------------------------

class TestUnit : public Z::_Unit_
{
public:
    double processedValue = 0;
    QString alias() const override { return ""; }
    QString name() const override { return ""; }
    double toSi(const double& value) const override
    {
        const_cast<TestUnit*>(this)->processedValue = value; return value;
    }
    double fromSi(const double&) const override { return 0; }
    Unit siUnit() const override { return nullptr; }
};

TEST_METHOD(Value_toSi)
{
    TestUnit unit;
    Z::Value v(3.14, &unit);
    v.toSi();
    ASSERT_EQ_DBL(unit.processedValue, v.value());
}

TEST_METHOD(Value_equality)
{
    Z::Value v(3.14_m);
    ASSERT_IS_TRUE(v == 3.14)
    ASSERT_IS_TRUE(v == 3.14_m)
    ASSERT_IS_TRUE(v == 314_cm)

    ASSERT_IS_TRUE(v != 314)
    ASSERT_IS_TRUE(v != 314_m)
    ASSERT_IS_TRUE(v != 3.14_cm)
}

TEST_METHOD(Value_compare)
{
    ASSERT_IS_TRUE(3.14_m > 3.14_cm)

    ASSERT_IS_TRUE(3.14_m > 3.14_cm)
    ASSERT_IS_TRUE(3.14_m >= 3.14_cm)

    ASSERT_IS_TRUE(3.14_cm < 3.14_m)
    ASSERT_IS_TRUE(3.14_cm <= 3.14_m)

    ASSERT_IS_TRUE(3.14_cm >= 3.14_cm)

    // For statistics: comparison of 3.14cm fails but 3.141cm passes on Windows MinGW 5.3 x32
    ASSERT_IS_TRUE(3.141_cm <= 3.141_cm)
}

//------------------------------------------------------------------------------

TEST_METHOD(ValueTS_constructors)
{
    Z::PairTS<int> v0;
    ASSERT_VALUE_T_S(v0, 0, 0)

    Z::PairTS<int> v1(10, 20);
    ASSERT_VALUE_T_S(v1, 10, 20)

    Z::PairTS<int> v2(v1);
    ASSERT_VALUE_T_S(v2, 10, 20)

    Z::PairTS<int> v3(&v1);
    ASSERT_VALUE_T_S(v3, 10, 20)
}

TEST_METHOD(ValueTS_assign)
{
    Z::PairTS<int> v1;
    v1 = 10;
    ASSERT_VALUE_T_S(v1, 10, 10)

    Z::PairTS<int> v2;
    v2.set(10, 20);
    ASSERT_VALUE_T_S(v2, 10, 20)
}

//------------------------------------------------------------------------------

TEST_GROUP("Values",
    ADD_TEST(Value_ctor),
    ADD_TEST(Value_toSi),
    ADD_TEST(Value_equality),
    ADD_TEST(Value_compare),
    ADD_TEST(ValueTS_constructors),
    ADD_TEST(ValueTS_assign),
)

} // namespace ValuesTests
} // namespace Tests
} // namespace Z

