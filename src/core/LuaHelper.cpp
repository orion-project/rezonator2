#include "LuaHelper.h"

#include <QApplication>
#include <QtMath>

#include <cmath>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static double impl_cot(double arg) { return 1.0 / qTan(arg); }
static double impl_acot(double arg) { return qAtan(1.0 / arg); }
static double impl_coth(double arg) { return 1.0 / tanh(arg); }

static double impl_sec(double arg) { return 1.0 / qCos(arg); }
static double impl_sech(double arg) { return 1.0 / cosh(arg); }
static double impl_csc(double arg) { return 1.0 / qSin(arg); }
static double impl_csch(double arg) { return 1.0 / sinh(arg); }

// func should return a number of results
#define LUA_DEFINE_GLOBAL_FUNC(name, meat) \
    static int global_##name(lua_State *L) { \
        double d = luaL_checknumber(L, 1); \
        lua_pushnumber(L, meat(d)); \
        return 1; \
    }

#define LUA_REGISTER_GLOBAL_FUN(lua, name) \
    lua_pushcfunction(lua, global_##name); \
    lua_setglobal(lua, #name);

LUA_DEFINE_GLOBAL_FUNC(sin, qSin)
LUA_DEFINE_GLOBAL_FUNC(sinh, sinh)
LUA_DEFINE_GLOBAL_FUNC(asin, qAsin)

LUA_DEFINE_GLOBAL_FUNC(cos, qCos)
LUA_DEFINE_GLOBAL_FUNC(cosh, cosh)
LUA_DEFINE_GLOBAL_FUNC(acos, qAcos)

LUA_DEFINE_GLOBAL_FUNC(tan, qTan)
LUA_DEFINE_GLOBAL_FUNC(tanh, tanh)
LUA_DEFINE_GLOBAL_FUNC(atan, qAtan)

LUA_DEFINE_GLOBAL_FUNC(cot, impl_cot)
LUA_DEFINE_GLOBAL_FUNC(coth, impl_coth)
LUA_DEFINE_GLOBAL_FUNC(acot, impl_acot)

LUA_DEFINE_GLOBAL_FUNC(sec, impl_sec)
LUA_DEFINE_GLOBAL_FUNC(sech, impl_sech)
LUA_DEFINE_GLOBAL_FUNC(csc, impl_csc)
LUA_DEFINE_GLOBAL_FUNC(csch, impl_csch)

LUA_DEFINE_GLOBAL_FUNC(abs, qAbs)
LUA_DEFINE_GLOBAL_FUNC(floor, qFloor)
LUA_DEFINE_GLOBAL_FUNC(ceil, qCeil)

LUA_DEFINE_GLOBAL_FUNC(exp, qExp)
LUA_DEFINE_GLOBAL_FUNC(ln, qLn)
LUA_DEFINE_GLOBAL_FUNC(lg, log10)

LUA_DEFINE_GLOBAL_FUNC(sqrt, qSqrt)

LUA_DEFINE_GLOBAL_FUNC(deg2rad, qDegreesToRadians)
LUA_DEFINE_GLOBAL_FUNC(rad2deg, qRadiansToDegrees)

namespace Z {

Lua::Lua()
{
}

Lua::~Lua()
{
    if (_lua)
        lua_close(_lua);
}

void Lua::registerGlobalFuncs()
{
    LUA_REGISTER_GLOBAL_FUN(_lua, sin);
    LUA_REGISTER_GLOBAL_FUN(_lua, sinh);
    LUA_REGISTER_GLOBAL_FUN(_lua, asin);

    LUA_REGISTER_GLOBAL_FUN(_lua, cos);
    LUA_REGISTER_GLOBAL_FUN(_lua, cosh);
    LUA_REGISTER_GLOBAL_FUN(_lua, acos);

    LUA_REGISTER_GLOBAL_FUN(_lua, tan);
    LUA_REGISTER_GLOBAL_FUN(_lua, tanh);
    LUA_REGISTER_GLOBAL_FUN(_lua, atan);

    LUA_REGISTER_GLOBAL_FUN(_lua, cot);
    LUA_REGISTER_GLOBAL_FUN(_lua, coth);
    LUA_REGISTER_GLOBAL_FUN(_lua, acot);

    LUA_REGISTER_GLOBAL_FUN(_lua, sec);
    LUA_REGISTER_GLOBAL_FUN(_lua, sech);
    LUA_REGISTER_GLOBAL_FUN(_lua, csc);
    LUA_REGISTER_GLOBAL_FUN(_lua, csch);

    LUA_REGISTER_GLOBAL_FUN(_lua, abs);
    LUA_REGISTER_GLOBAL_FUN(_lua, floor);
    LUA_REGISTER_GLOBAL_FUN(_lua, ceil);

    LUA_REGISTER_GLOBAL_FUN(_lua, exp);
    LUA_REGISTER_GLOBAL_FUN(_lua, ln);
    LUA_REGISTER_GLOBAL_FUN(_lua, lg);

    LUA_REGISTER_GLOBAL_FUN(_lua, sqrt);

    LUA_REGISTER_GLOBAL_FUN(_lua, deg2rad);
    LUA_REGISTER_GLOBAL_FUN(_lua, rad2deg);
}

Z::Result<double> Lua::calculate(const QString& formula)
{
    if (!_lua)
    {
        _lua = luaL_newstate();
        if (!_lua)
            return Z::Result<double>::fail(qApp->translate("Formula", "Not enough memory to create formula parser"));
        luaL_openlibs(_lua);
        registerGlobalFuncs();
    }

    auto L = _lua;

    #define RESULT_VAR "result"
    #define FORMULA_ID "formula"

    static QRegExp resultVar(RESULT_VAR "\\s*=");
    QString code = formula;
    int pos = code.indexOf(resultVar);
    if (pos < 0) code = (RESULT_VAR "=") + code;

    QString error;

    auto codeBytes = code.toLatin1();
    int res = luaL_loadbufferx(L, codeBytes.data(), static_cast<size_t>(codeBytes.size()), FORMULA_ID, "t");
    if (res == LUA_OK)
    {
        res = lua_pcall(L, 0, 0, 0);
        if (res == LUA_OK)
        {
            int valueType = lua_getglobal(L, RESULT_VAR);
            if (valueType == LUA_TNUMBER)
            {
                auto value = lua_tonumber(L, -1);
                return Z::Result<double>::success(value);
            }
            else
                error = qApp->translate("Formula", "Result value is not a number");
        }
    }
    if (res != LUA_OK)
    {
        error = QString(lua_tostring(L, -1));
        if (!error.isEmpty())
        {
            // Clean up error message which looks like
            // [string "formula"]:1: <useful message>
            static QString prefix("[string \"" FORMULA_ID "\"]:");
            if (error.startsWith(prefix))
                error.remove(0, prefix.length());
            int pos = error.indexOf(':');
            if (pos >= 0)
                error.remove(0, pos+2);
        }
    }

    if (error.isEmpty())
        error = qApp->tr("Formula", "Unknown error, code=%1").arg(res);
    return Z::Result<double>::fail(error);

    #undef RESULT_VAR
    #undef FORMULA_ID
}

} // namespace Z

