#include "Formula.h"
#include "Protocol.h"
#include "LuaHelper.h"

#include <QApplication>

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
        Z_ERROR(QString("Bad formula for param '%1': %2").arg(_target->alias(), _status))
        return;
    }

    Z::Lua lua;
    _status = lua.open();
    if (!_status.isEmpty())
    {
        Z_ERROR(QString("Bad formula for param '%1': %2").arg(_target->alias(), _status))
        return;
    }

    for (auto dep : _deps)
        lua.setGlobalVar(dep->alias(), dep->value().toSi());

    auto res = lua.calculate(_code);
    if (!res.ok())
    {
        _status = res.error();
        Z_ERROR(QString("Bad formula for param '%1': %2").arg(_target->alias(), _status))
        return;
    }

    auto unit = _target->value().unit();
    auto value = unit->fromSi(res.value());
    _target->setValue(Value(value, unit));
    _status.clear();
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
