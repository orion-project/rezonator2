#include "Formula.h"
#include "Protocol.h"

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <cassert>

#include <QApplication>
#include <QRegExp>

namespace Z {

bool Formula::prepare(Parameters& availableDeps)
{
    Q_UNUSED(availableDeps)
    return false;
}

void Formula::calculate()
{
    if (_code.isEmpty())
    {
        _status = qApp->translate("Formula", "Formula is empty");
        Z_ERROR(QString("Empty formula for param '%1'").arg(_target->alias()));
        return;
    }

    lua_State *L = luaL_newstate();
    if (!L)
    {
        _status = qApp->translate("Formula", "Not enough memory to create formula parser");
        Z_ERROR(QString("Bad formula for param '%1'").arg(_target->alias(), _status));
        return;
    }

    luaL_openlibs(L);

    #define RESULT_VAR "result"

    // TODO: add names and values for deps

    static QRegExp resultVar(RESULT_VAR "\\s*=");
    QString code = _code;
    int pos = code.indexOf(resultVar);
    if (pos < 0) code = (RESULT_VAR "=") + code;

    int res = luaL_loadstring(L, code.toLatin1().data());
    if (res == LUA_OK)
    {
        res = lua_pcall(L, 0, 0, 0);
        if (res == LUA_OK)
        {
            int valueType = lua_getglobal(L, RESULT_VAR);
            if (valueType == LUA_TNUMBER)
            {
                auto value = lua_tonumber(L, -1);
                auto unit = _target->value().unit();
                _target->setValue(Value(value, unit));
                _status.clear();
            }
            else
                _status = qApp->translate("Formula", "Result value is not a number");
        }
    }
    if (res != LUA_OK)
    {
        _status = QString(lua_tostring(L, -1));
        if (_status.isEmpty())
            _status = qApp->tr("Formula", "Unknown error, code=%1").arg(res);
    }

    if (!_status.isEmpty())
        Z_ERROR(QString("Bad formula for param '%1': %2").arg(_target->alias(), _status));

    lua_close(L);

    #undef RESULT_VAR
}

void Formula::addDep(Parameter* param)
{
    _deps.append(param);
    param->addListener(this);
}

//------------------------------------------------------------------------------

void Formulas::put(Formula* f)
{
    auto p = f->target();
    if (_items.contains(p))
    {
        if (_items[p] != f)
            delete _items[p];
    }
    _items[p] = f;
}

Formula* Formulas::get(Parameter* p)
{
    if (_items.contains(p))
    {
        assert(_items[p]->target() == p);
        return _items[p];
    }
    return nullptr;
}

void Formulas::free(Parameter *p)
{
    if (_items.contains(p))
    {
        auto f = _items[p];
        _items.remove(p);
        delete f;
    }
}

void Formulas::clear()
{
    qDeleteAll(_items.values());
    _items.clear();
}

//------------------------------------------------------------------------------

namespace FormulaUtils {

bool isValidVariableName(const QString& s)
{
    static QRegExp r("^[a-zA-Z_][a-zA-Z_0-9]*$");
    return r.exactMatch(s);
}

} // namespace ParameterUtils

} // namespace Z
