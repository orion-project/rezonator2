#include "../core/LuaHelper.h"

#include "testing/OriTestBase.h"

namespace Z {
namespace Tests {
namespace LuaHelperTests {

#define OPEN_LUA(lua_var)\
    Z::Lua lua_var;\
    {\
        auto res = lua_var.open();\
        ASSERT_EQ_STR(res, "")\
    }

#define ASSERT_LUA_CALC(lua, code, expected_value) {\
    auto res = lua.calculate(code); \
    ASSERT_IS_TRUE(res.ok()) \
    ASSERT_EQ_DBL(res.value(), expected_value) \
}

#define ASSERT_LUA_EXEC(lua, result_var, expected_value) {\
    QString res = lua.execute(); \
    ASSERT_EQ_STR(res, "") \
    auto val = lua.getGlobalVar(result_var); \
    ASSERT_IS_TRUE(val.ok()) \
    ASSERT_EQ_DBL(val.value(), expected_value) \
}

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

#define ASSERT_LUA_ERROR(lua, code, expected_error) {\
    auto res = lua.calculate(code);\
    ASSERT_IS_FALSE(res.ok())\
    TEST_LOG(res.error())\
    ASSERT_EQ_STR(res.error(), expected_error)\
}

TEST_METHOD(global_funcs)
{
    OPEN_LUA(lua)

    ASSERT_LUA(0.33, "sin(%1)", "math.sin(%1)")
    ASSERT_LUA(0.33, "sinh(%1)", "(math.exp(%1) - math.exp(-%1)) / 2")
    ASSERT_LUA(0.33, "asin(%1)", "math.asin(%1)")

    ASSERT_LUA(0.33, "cos(%1)", "math.cos(%1)")
    ASSERT_LUA(0.33, "cosh(%1)", "(math.exp(%1) + math.exp(-%1)) / 2")
    ASSERT_LUA(0.33, "acos(%1)", "math.acos(%1)")

    ASSERT_LUA(0.33, "tan(%1)", "math.sin(%1) / math.cos(%1)")
    ASSERT_LUA(0.33, "tanh(%1)", "(math.exp(%1) - math.exp(-%1)) / (math.exp(%1) + math.exp(-%1))")
    ASSERT_LUA(0.33, "atan(%1)", "math.atan(%1)")

    ASSERT_LUA(0.33, "tan(%1)", "math.tan(%1)")
    ASSERT_LUA(0.33, "tanh(%1)", "(math.exp(%1) - math.exp(-%1)) / (math.exp(%1) + math.exp(-%1))")
    ASSERT_LUA(0.33, "atan(%1)", "math.atan(%1)")

    ASSERT_LUA(0.33, "cot(%1)", "math.cos(%1) / math.sin(%1)")
    ASSERT_LUA(0.33, "coth(%1)", "(math.exp(%1) + math.exp(-%1)) / (math.exp(%1) - math.exp(-%1))")
    ASSERT_LUA(0.33, "acot(%1)", "math.atan(1 / %1)")

    ASSERT_LUA(0.33, "sec(%1)", "1 / cos(%1)")
    ASSERT_LUA(0.33, "sech(%1)", "2 / (math.exp(%1) + math.exp(-%1))")
    ASSERT_LUA(0.33, "csc(%1)", "1 / sin(%1)")
    ASSERT_LUA(0.33, "csch(%1)", "2 / (math.exp(%1) - math.exp(-%1))")

    ASSERT_LUA(0.33, "abs(-%1)", "math.abs(-%1)")
    ASSERT_LUA(0.33, "floor(%1)", "math.floor(%1)")
    ASSERT_LUA(0.33, "ceil(%1)", "math.ceil(%1)")

    ASSERT_LUA(0.33, "exp(%1)", "math.exp(%1)")
    ASSERT_LUA(0.33, "ln(%1)", "math.log(%1, math.exp(1))")
    ASSERT_LUA(0.33, "lg(%1)", "math.log(%1, 10)")

    ASSERT_LUA(0.33, "sqrt(%1)", "math.sqrt(%1)")

    ASSERT_LUA(0.33, "deg2rad(%1)", "math.rad(%1)")
    ASSERT_LUA(0.33, "rad2deg(%1)", "math.deg(%1)")

    ASSERT_LUA(0, "pi() --%1", "math.pi --%1")
}

TEST_METHOD(can_be_several_parser_instances)
{
    OPEN_LUA(lua1)
    OPEN_LUA(lua2)
    ASSERT_LUA_CALC(lua1, "2+2", 4)
    ASSERT_LUA_CALC(lua2, "3+3", 6)
}

TEST_METHOD(different_instances_use_differenet_globals)
{
    OPEN_LUA(lua1)
    OPEN_LUA(lua2)

    lua1.setGlobalVar("var1", 2);
    lua2.setGlobalVar("var1", 3);

    ASSERT_LUA_CALC(lua1, "var1^2", 4)
    ASSERT_LUA_CALC(lua2, "var1^2", 9)
}

TEST_METHOD(can_set_one_global_before_code)
{
    OPEN_LUA(lua)
    lua.setGlobalVar("var1", 5);
    lua.setCode("res=var1^2");
    ASSERT_LUA_EXEC(lua, "res", 25)
}

TEST_METHOD(can_set_one_global_after_code)
{
    OPEN_LUA(lua)
    lua.setCode("res=var1^2");
    lua.setGlobalVar("var1", 5);
    ASSERT_LUA_EXEC(lua, "res", 25)
}

TEST_METHOD(can_set_several_globals_before_code)
{
    OPEN_LUA(lua)
    lua.setGlobalVars({{"a", 2}, {"b", 3}});
    lua.setCode("c = a + b");
    ASSERT_LUA_EXEC(lua, "c", 5)
}

TEST_METHOD(can_set_several_globals_after_code)
{
    OPEN_LUA(lua)
    lua.setCode("c = a + b");
    lua.setGlobalVars({{"a", 3}, {"b", 4}});
    ASSERT_LUA_EXEC(lua, "c", 7)
}

// TODO: read lua docs carefully and find out if is possible
// to compile the code once and run it many times with different values
TEST_METHOD(can_change_globals_with_the_same_code)
{
    OPEN_LUA(lua)

    lua.setCode("res=var1^2");

    lua.setGlobalVar("var1", 5);
    ASSERT_LUA_EXEC(lua, "res", 25)

    lua.setGlobalVar("var1", 6);
    ASSERT_LUA_EXEC(lua, "res", 36)
}

TEST_METHOD(can_reuse_globals_with_different_code)
{
    OPEN_LUA(lua)

    lua.setGlobalVars({{"a", 3}, {"b", 4}});

    ASSERT_LUA_CALC(lua, "a + b", 7)
    ASSERT_LUA_CALC(lua, "a * b", 12)
}

TEST_METHOD(can_calc_several_values)
{
    OPEN_LUA(lua)

    lua.setCode("A=1; B=2; C=3; D=4");

    auto res = lua.execute();
    ASSERT_EQ_STR(res, "")

    auto vars = lua.getGlobalVars();
    ASSERT_EQ_DBL(vars["A"], 1)
    ASSERT_EQ_DBL(vars["B"], 2)
    ASSERT_EQ_DBL(vars["C"], 3)
    ASSERT_EQ_DBL(vars["D"], 4)
}

TEST_METHOD(can_remove_global)
{
    OPEN_LUA(lua)

    auto res1 = lua.calculate("a + a");
    ASSERT_IS_FALSE(res1.ok())
    TEST_LOG(res1.error())

    lua.setGlobalVar("a", 5);

    ASSERT_LUA_CALC(lua, "a + a", 10)

    lua.removeGlobalVar("a");
    auto res2 = lua.calculate("a + a");
    ASSERT_IS_FALSE(res2.ok())
    TEST_LOG(res2.error())
}

TEST_METHOD(can_show_refined_error_messages)
{
    OPEN_LUA(lua)

    ASSERT_LUA_ERROR(lua, "a + a", "Unknown variable 'a'")
    ASSERT_LUA_ERROR(lua, "a(2)", "Unknown function 'a'")
    ASSERT_LUA_ERROR(lua, "sin(a)", "Invalid argument for 'sin': unknown variable")
}

//------------------------------------------------------------------------------

TEST_GROUP("LuaHelper",
    ADD_TEST(global_funcs),
    ADD_TEST(can_be_several_parser_instances),
    ADD_TEST(different_instances_use_differenet_globals),
    ADD_TEST(can_set_one_global_before_code),
    ADD_TEST(can_set_one_global_after_code),
    ADD_TEST(can_set_several_globals_before_code),
    ADD_TEST(can_set_several_globals_after_code),
    //ADD_TEST(can_change_globals_with_the_same_code),
    ADD_TEST(can_reuse_globals_with_different_code),
    ADD_TEST(can_calc_several_values),
    ADD_TEST(can_remove_global),
    ADD_TEST(can_show_refined_error_messages),
)

} // namespace LuaHelperTests
} // namespace Tests
} // namespace Z
