#include "testing/OriTestBase.h"
#include "../core/Values.h"
#include "../core/Units.h"
#include "TestUtils.h"

namespace Z {
namespace Tests {
namespace ValuesTests {

using namespace Units;

#define ASSERT_VALUE_UNIT(z_value, expected_value, expected_unit)\
    ASSERT_EQ_DBL(z_value.value(), expected_value)\
    ASSERT_EQ_UNIT(z_value.unit(), expected_unit())

#define ASSERT_VALUE_T_S(value, expectedT, expectedS)\
    ASSERT_EQ_DBL(value.T, expectedT)\
    ASSERT_EQ_DBL(value.S, expectedS)

//------------------------------------------------------------------------------

TEST_METHOD(Value_ctor_value_unit)
{
    Value v1(3.14, mkm());
    ASSERT_VALUE_UNIT(v1, 3.14, mkm)
}

TEST_METHOD(Value_ctor_copy)
{
    Value v1(3.14, mkm());
    Value v2(v1);
    ASSERT_VALUE_UNIT(v1, 3.14, mkm)
}

TEST_METHOD(Value_ctor_from_numbers)
{
    Value v1(3.14);
    ASSERT_VALUE_UNIT(v1, 3.14, none)

    Value v2(4);
    ASSERT_VALUE_UNIT(v2, 4, none)
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
    Value v(3.14, &unit);
    v.toSi();
    ASSERT_EQ_DBL(unit.processedValue, v.value());
}

TEST_METHOD(Value_literals_int)
{
    Value v1 = 2_nm;
    ASSERT_EQ_ZVALUE(v1, Value(2, nm()))

    Value v2 = 3_mkm;
    ASSERT_EQ_ZVALUE(v2, Value(3, mkm()))

    Value v3 = 4_deg;
    ASSERT_EQ_ZVALUE(v3, Value(4, deg()))
}

TEST_METHOD(Value_literals_double)
{
    Value v1 = 2.1_nm;
    ASSERT_EQ_ZVALUE(v1, Value(2.1, nm()))

    Value v2 = 3.31_mkm;
    ASSERT_EQ_ZVALUE(v2, Value(3.31, mkm()))

    Value v3 = 4.48_deg;
    ASSERT_EQ_ZVALUE(v3, Value(4.48, deg()))
}

TEST_METHOD(Value_equality)
{
    Value v(3.14_m);
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
    PairTS<int> v0;
    ASSERT_VALUE_T_S(v0, 0, 0)

    PairTS<int> v1(10, 20);
    ASSERT_VALUE_T_S(v1, 10, 20)

    PairTS<int> v2(v1);
    ASSERT_VALUE_T_S(v2, 10, 20)

    PairTS<int> v3(&v1);
    ASSERT_VALUE_T_S(v3, 10, 20)
}

TEST_METHOD(ValueTS_assign)
{
    PairTS<int> v1;
    v1 = 10;
    ASSERT_VALUE_T_S(v1, 10, 10)

    PairTS<int> v2;
    v2.set(10, 20);
    ASSERT_VALUE_T_S(v2, 10, 20)
}

//------------------------------------------------------------------------------

TEST_GROUP("Values",
    ADD_TEST(Value_ctor_value_unit),
    ADD_TEST(Value_ctor_copy),
    ADD_TEST(Value_ctor_from_numbers),
    ADD_TEST(Value_toSi),
    ADD_TEST(Value_literals_int),
    ADD_TEST(Value_literals_double),
    ADD_TEST(Value_equality),
    ADD_TEST(Value_compare),
    ADD_TEST(ValueTS_constructors),
    ADD_TEST(ValueTS_assign),
)

} // namespace ValuesTests
} // namespace Tests
} // namespace Z

