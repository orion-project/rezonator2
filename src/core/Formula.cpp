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

Formula::Formula(Parameter* target): _target(target)
{
    _target->setValueDriver(ParamValueDriver::Formula);
}

Formula::~Formula()
{
    _target->setValueDriver(ParamValueDriver::None);
    for (Z::Parameter* dep : _deps)
        dep->removeListener(this);
}

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
    #define FORMULA_ID "formula"

    // TODO: add math functions to global namespace

    for (Z::Parameter* dep : _deps)
    {
        lua_pushnumber(L, dep->value().toSi());
        lua_setglobal(L, dep->alias().toLatin1().data());
    }

    static QRegExp resultVar(RESULT_VAR "\\s*=");
    QString code = _code;
    int pos = code.indexOf(resultVar);
    if (pos < 0) code = (RESULT_VAR "=") + code;

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
                auto valueSi = lua_tonumber(L, -1);
                auto unit = _target->value().unit();
                auto value = unit->fromSi(valueSi);
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

        if (!_status.isEmpty())
        {
            // Clean up error message which looks like
            // [string "formula"]:1: <useful message>
            static QString prefix("[string \"" FORMULA_ID "\"]:");
            if (_status.startsWith(prefix))
                _status.remove(0, prefix.length());
            int pos = _status.indexOf(':');
            if (pos >= 0)
                _status.remove(0, pos+2);
        }

        if (_status.isEmpty())
            _status = qApp->tr("Formula", "Unknown error, code=%1").arg(res);
        else
            _status = qApp->tr("Error: %1").arg(_status);
    }

    if (!_status.isEmpty())
        Z_ERROR(QString("Bad formula for param '%1': %2").arg(_target->alias(), _status));

    lua_close(L);

    #undef RESULT_VAR
    #undef FORMULA_ID
}

void Formula::addDep(Parameter* param)
{
    _deps.append(param);
    param->addListener(this);
}

void Formula::removeDep(Parameter* param)
{
    param->removeListener(this);
    _deps.removeAll(param);
}

void Formula::assignDeps(const Formula *formula)
{
    while (!_deps.isEmpty())
        removeDep(_deps.first());
    for (auto dep : formula->_deps)
        addDep(dep);
}

QString Formula::displayStr() const
{
    QStringList params;
    for (auto dep : _deps)
        params << dep->displayLabel();
    return QStringLiteral("f(%1)").arg(params.join(QStringLiteral(", ")));
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

bool Formulas::ifDependsOn(Parameter *whichParam, Parameter *onParam) const
{
    if (!_items.contains(whichParam))
        return false;

    auto formula = _items[whichParam];
    if (formula->deps().isEmpty())
        return false;

    for (auto param : formula->deps())
    {
        if (param == onParam)
            return true;

        if (ifDependsOn(param, onParam))
            return true;
    }

    return false;
}

Parameters Formulas::dependentParams(Parameter *whichParam) const
{
    Parameters result;
    for (Formula *formula : _items.values())
        for (Parameter *param : formula->deps())
            if (param == whichParam)
            {
                if (!result.contains(formula->target()))
                    result << formula->target();
                break;
            }
    return result;
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
