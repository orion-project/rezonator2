#include "testing/OriTestBase.h"
#include "../core/LuaHelper.h"

namespace Z {
namespace Tests {
namespace LuaHelperTests {

#define ASSERT_LUA(val, e1, e2) { \
    auto expr1 = QString(e1).arg(val); \
    auto expr2 = QString(e2).arg(val); \
    TEST_LOG(expr1 + QString(" == ") + expr2) \
    auto r1 = lua.calculate(expr1); \
    ASSERT_IS_TRUE(r1.ok()) \
    auto r2 = lua.calculate(expr2); \
    ASSERT_IS_TRUE(r2.ok()) \
    ASSERT_EQ_DBL(r1.value(), r2.value()) \
}

TEST_METHOD(global_funcs)
{
    Z::Lua lua;
    ASSERT_LUA(0.33, "sin(%1)", "math.sin(%1)");
    ASSERT_LUA(0.33, "sinh(%1)", "(math.exp(%1) - math.exp(-%1)) / 2");
    ASSERT_LUA(0.33, "asin(%1)", "math.asin(%1)");

    ASSERT_LUA(0.33, "cos(%1)", "math.cos(%1)");
    ASSERT_LUA(0.33, "cosh(%1)", "(math.exp(%1) + math.exp(-%1)) / 2");
    ASSERT_LUA(0.33, "acos(%1)", "math.acos(%1)");

    ASSERT_LUA(0.33, "tan(%1)", "math.sin(%1) / math.cos(%1)");
    ASSERT_LUA(0.33, "tanh(%1)", "(math.exp(%1) - math.exp(-%1)) / (math.exp(%1) + math.exp(-%1))");
    ASSERT_LUA(0.33, "atan(%1)", "math.atan(%1)");

    ASSERT_LUA(0.33, "tan(%1)", "math.tan(%1)");
    ASSERT_LUA(0.33, "tanh(%1)", "(math.exp(%1) - math.exp(-%1)) / (math.exp(%1) + math.exp(-%1))");
    ASSERT_LUA(0.33, "atan(%1)", "math.atan(%1)");

    ASSERT_LUA(0.33, "cot(%1)", "math.cos(%1) / math.sin(%1)");
    ASSERT_LUA(0.33, "coth(%1)", "(math.exp(%1) + math.exp(-%1)) / (math.exp(%1) - math.exp(-%1))");
    ASSERT_LUA(0.33, "acot(%1)", "math.atan(1 / %1)");

    ASSERT_LUA(0.33, "sec(%1)", "1 / cos(%1)");
    ASSERT_LUA(0.33, "sech(%1)", "2 / (math.exp(%1) + math.exp(-%1))");
    ASSERT_LUA(0.33, "csc(%1)", "1 / sin(%1)");
    ASSERT_LUA(0.33, "csch(%1)", "2 / (math.exp(%1) - math.exp(-%1))");

    ASSERT_LUA(0.33, "abs(%1)", "math.abs(%1)");
    ASSERT_LUA(0.33, "floor(%1)", "math.floor(%1)");
    ASSERT_LUA(0.33, "ceil(%1)", "math.ceil(%1)");

    ASSERT_LUA(0.33, "exp(%1)", "math.exp(%1)");
    ASSERT_LUA(0.33, "ln(%1)", "math.log(%1, math.exp(1))");
    ASSERT_LUA(0.33, "lg(%1)", "math.log(%1, 10)");

    ASSERT_LUA(0.33, "sqrt(%1)", "math.sqrt(%1)");

    ASSERT_LUA(0.33, "deg2rad(%1)", "math.rad(%1)");
    ASSERT_LUA(0.33, "rad2deg(%1)", "math.deg(%1)");
}

//------------------------------------------------------------------------------

TEST_GROUP("Lua Helper",
    ADD_TEST(global_funcs),
)

} // namespace LuaHelperTests
} // namespace Tests
} // namespace Z
