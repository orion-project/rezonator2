#include "LuaHelper.h"

#include <QApplication>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace Z {

Lua::Lua()
{
}

Lua::~Lua()
{
    if (_lua)
        lua_close(_lua);
}

Z::Result<double> Lua::calculate(const QString& formula)
{
    if (!_lua)
    {
        _lua = luaL_newstate();
        if (!_lua)
            return Z::Result<double>::fail(qApp->translate("Formula", "Not enough memory to create formula parser"));
        luaL_openlibs(_lua);
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

