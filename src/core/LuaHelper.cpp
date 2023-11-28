#include "LuaHelper.h"

#include <QApplication>
#include <QDebug>
#include <QtMath>
#include <QRegularExpression>

#include <cmath>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define RESULT_VAR "ans"
#define FORMULA_ID "formula"

static double impl_cot(double arg) { return 1.0 / qTan(arg); }
static double impl_acot(double arg) { return qAtan(1.0 / arg); }
static double impl_coth(double arg) { return 1.0 / tanh(arg); }

static double impl_sec(double arg) { return 1.0 / qCos(arg); }
static double impl_sech(double arg) { return 1.0 / cosh(arg); }
static double impl_csc(double arg) { return 1.0 / qSin(arg); }
static double impl_csch(double arg) { return 1.0 / sinh(arg); }

static int global_pi(lua_State *L)
{
    lua_pushnumber(L, M_PI);
    return 1;
}

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

void Lua::registerGlobalFuncs(lua_State* lua)
{
    LUA_REGISTER_GLOBAL_FUN(lua, sin)
    LUA_REGISTER_GLOBAL_FUN(lua, sinh)
    LUA_REGISTER_GLOBAL_FUN(lua, asin)

    LUA_REGISTER_GLOBAL_FUN(lua, cos)
    LUA_REGISTER_GLOBAL_FUN(lua, cosh)
    LUA_REGISTER_GLOBAL_FUN(lua, acos)

    LUA_REGISTER_GLOBAL_FUN(lua, tan)
    LUA_REGISTER_GLOBAL_FUN(lua, tanh)
    LUA_REGISTER_GLOBAL_FUN(lua, atan)

    LUA_REGISTER_GLOBAL_FUN(lua, cot)
    LUA_REGISTER_GLOBAL_FUN(lua, coth)
    LUA_REGISTER_GLOBAL_FUN(lua, acot)

    LUA_REGISTER_GLOBAL_FUN(lua, sec)
    LUA_REGISTER_GLOBAL_FUN(lua, sech)
    LUA_REGISTER_GLOBAL_FUN(lua, csc)
    LUA_REGISTER_GLOBAL_FUN(lua, csch)

    LUA_REGISTER_GLOBAL_FUN(lua, abs)
    LUA_REGISTER_GLOBAL_FUN(lua, floor)
    LUA_REGISTER_GLOBAL_FUN(lua, ceil)

    LUA_REGISTER_GLOBAL_FUN(lua, exp)
    LUA_REGISTER_GLOBAL_FUN(lua, ln)
    LUA_REGISTER_GLOBAL_FUN(lua, lg)

    LUA_REGISTER_GLOBAL_FUN(lua, sqrt)

    LUA_REGISTER_GLOBAL_FUN(lua, deg2rad)
    LUA_REGISTER_GLOBAL_FUN(lua, rad2deg)

    LUA_REGISTER_GLOBAL_FUN(lua, pi)
}

QString Lua::open()
{
    if (_lua) lua_close(_lua);

    _lua = luaL_newstate();
    if (!_lua)
        return qApp->translate("Formula", "Not enough memory to initialize formula parser");

    luaL_openlibs(_lua);
    registerGlobalFuncs(_lua);
    return QString();
}

Z::Result<double> Lua::calculate(const QString& formula)
{
    Q_ASSERT(_lua);

    static QRegularExpression resultVar(RESULT_VAR "\\s*=");
    QString code = formula;
    int pos = code.indexOf(resultVar);
    if (pos < 0) code = (RESULT_VAR "=") + code;

    QString error = setCode(code);
    if (!error.isEmpty())
        return Z::Result<double>::fail(error);

    error = execute();
    if (!error.isEmpty())
        return Z::Result<double>::fail(error);

    return getGlobalVar(RESULT_VAR);
}

QString Lua::setCode(const QString& code)
{
    Q_ASSERT(_lua);

    auto codeBytes = code.toLatin1();
    int res = luaL_loadbufferx(_lua, codeBytes.data(), static_cast<size_t>(codeBytes.size()), FORMULA_ID, "t");
    if (res != LUA_OK)
        return getLuaError(res);
    return QString();
}

QString Lua::execute()
{
    Q_ASSERT(_lua);

    int res = lua_pcall(_lua, 0, 0, 0);
    if (res != LUA_OK)
        return getLuaError(res);
    return QString();
}

QString Lua::getLuaError(int errCode) const
{
    Q_ASSERT(_lua);

    QString error(lua_tostring(_lua, -1));
    if (error.isEmpty())
        return qApp->translate("Formula", "Unknown error, code=%1").arg(errCode);

    // Clean up error message which looks like
    // [string "formula"]:1: <useful message>
    static QString prefix("[string \"" FORMULA_ID "\"]:");
    if (error.startsWith(prefix))
        error.remove(0, prefix.length());
    int pos = error.indexOf(':');
    if (pos >= 0)
        error.remove(0, pos+2);
    return refineLuaError(error);
}

QString Lua::refineLuaError(const QString& err) const
{
    auto match = QRegularExpression("attempt to perform arithmetic on a nil value \\(global '(.+)'\\)").match(err);
    if (match.hasMatch())
        return qApp->translate("Formula", "Unknown variable '%1'").arg(match.captured(1));

    match = QRegularExpression("attempt to call a nil value \\(global '(.+)'\\)").match(err);
    if (match.hasMatch())
        return qApp->translate("Formula", "Unknown function '%1'").arg(match.captured(1));

    match = QRegularExpression("bad argument #(\\d+) to '(.+)' \\(number expected, got nil\\)").match(err);
    if (match.hasMatch())
        return qApp->translate("Formula", "Invalid argument for '%1': unknown variable").arg(match.captured(2));

    return err;
}

Z::Result<double> Lua::getGlobalVar(const char* name)
{
    Q_ASSERT(_lua);

    int valueType = lua_getglobal(_lua, name);
    if (valueType != LUA_TNUMBER)
        return Z::Result<double>::fail(qApp->translate("Formula", "Variable '%1' is not a number").arg(name));

    auto value = lua_tonumber(_lua, -1);
    return Z::Result<double>::success(value);
}

QMap<QString, double> Lua::getGlobalVars()
{
    Q_ASSERT(_lua);

    QMap<QString, double> vars;
    lua_pushglobaltable(_lua); // get global table (index: -2)
    lua_pushnil(_lua); // put a nil key on the stack (index: -1)
    while (lua_next(_lua, -2)) // get a key and put next key-value pair,
    {                          // -2 is index of table
        auto keyType = lua_type(_lua, -2);
        auto valueType = lua_type(_lua, -1);
        if (keyType == LUA_TSTRING and valueType == LUA_TNUMBER)
        {
            auto namePtr = lua_tostring(_lua, -2);
            if (namePtr)
            {
                auto varName = QString::fromLatin1(namePtr);
                vars[varName] = lua_tonumber(_lua, -1);
            }
        }
        lua_pop(_lua, 1); // remove value, keep key for next iteration
    }
    lua_pop(_lua, 1); // remove global table
    return vars;
}

void Lua::setGlobalVar(const QString& name, double value)
{
    if (!_lua) return;

    lua_pushnumber(_lua, value);
    lua_setglobal(_lua, name.toLatin1().data());
}

void Lua::setGlobalVars(const QMap<QString, double>& vars)
{
    Q_ASSERT(_lua);

    QMapIterator<QString, double> it(vars);
    while (it.hasNext())
    {
        it.next();
        setGlobalVar(it.key(), it.value());
    }
}

void Lua::removeGlobalVar(const QString& name)
{
    Q_ASSERT(_lua);

    lua_pushnil(_lua);
    lua_setglobal(_lua, name.toLatin1().data());
}

} // namespace Z
