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
    ASSERT_Z_VALUE_AND_UNIT(v1, 3.14, Z::Units::mkm())

    Z::Value v2(v1);
    ASSERT_Z_VALUE_AND_UNIT(v2, 3.14, Z::Units::mkm())
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

#define _(val) Z::Value(val,
#define _m Z::Units::m())
#define _cm Z::Units::cm())

TEST_METHOD(Value_equality)
{
    Z::Value v(_(3.14)_m);
    ASSERT_IS_TRUE(v == 3.14)
    ASSERT_IS_TRUE(v == _(3.14)_m)
    ASSERT_IS_TRUE(v == _(314)_cm)

    ASSERT_IS_TRUE(v != 314)
    ASSERT_IS_TRUE(v != _(314)_m)
    ASSERT_IS_TRUE(v != _(3.14)_cm)
}

TEST_METHOD(Value_compare)
{
    ASSERT_IS_TRUE(_(3.14)_m > _(3.14)_cm)

    ASSERT_IS_TRUE(_(3.14)_m > _(3.14)_cm)
    ASSERT_IS_TRUE(_(3.14)_m >= _(3.14)_cm)

    ASSERT_IS_TRUE(_(3.14)_cm < _(3.14)_m)
    ASSERT_IS_TRUE(_(3.14)_cm <= _(3.14)_m)

    ASSERT_IS_TRUE(_(3.14)_cm >= _(3.14)_cm)

    // For statistics: comparison of 3.14cm fails but 3.141cm passes on Windows MinGW 5.3 x32
    ASSERT_IS_TRUE(_(3.141)_cm <= _(3.141)_cm)
}

#undef _
#undef _m
#undef _cm

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

