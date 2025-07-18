#include "Formula.h"
#include "Protocol.h"
#include "LuaHelper.h"

#include <QApplication>
#include <QHash>
#include <QRegularExpression>

namespace Z {

Formula::Formula(Parameter* target): _target(target)
{
    _target->setValueDriver(ParamValueDriver::Formula);
}

Formula::~Formula()
{
    _target->setValueDriver(ParamValueDriver::None);
    foreach (Z::Parameter* dep, _deps)
        dep->removeListener(this);
}

bool Formula::prepare(Parameters& availableDeps)
{
    Q_UNUSED(availableDeps)
    return false;
}

void Formula::setError(const QString &error)
{
    _error = error;
    _target->setError(_error);
    Z_ERROR(QString("Bad formula for param '%1': %2").arg(_target->alias(), _error))
}

void Formula::calculate()
{
    if (_code.isEmpty())
    {
        setError(qApp->translate("Formula", "Formula is empty"));
        return;
    }
    
    for (auto dep : std::as_const(_deps))
        if (dep->failed()) {
            setError(qApp->translate("Formula", "Dependency parameter %1 failed: %2")
                .arg(dep->displayLabel(), dep->error()));
            return;
        }

    Z::Lua lua;
    QString err = lua.open();
    if (!err.isEmpty())
    {
        setError(err);
        return;
    }

    for (auto dep : std::as_const(_deps))
        lua.setGlobalVar(dep->alias(), dep->value().toSi());

    auto res = lua.calculate(_code);
    if (!res.ok())
    {
        setError(res.error());
        return;
    }

    auto unit = _target->value().unit();
    auto value = unit->fromSi(res.value());
    _target->setValue(Value(value, unit));
    _error.clear();
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

static QRegularExpression varNameRegex()
{
    static QRegularExpression r("[a-zA-Z_][a-zA-Z_0-9]*");
    return r;
}

bool Formula::renameDep(Parameter *param, const QString &newName)
{
    if (!_deps.contains(param))
        return false;
    QString newCode;
    QTextStream s(&newCode);
    int prevOffset = 0;
    bool depFound = false;
    auto it = varNameRegex().globalMatch(_code);
    while (it.hasNext()) {
        auto m = it.next();
        if (m.captured() == param->alias()) {
            s << _code.mid(prevOffset, m.capturedStart()-prevOffset);
            s << newName;
            prevOffset = m.capturedEnd();
            depFound = true;
        }
    }
    if (depFound) {
        if (prevOffset < _code.length())
            s << _code.mid(prevOffset);
        _code = newCode;
    }
    return depFound;
}

void Formula::findDeps(const Parameters &globalParams)
{
    QHash<QString, Parameter*> newDeps;
    auto it = varNameRegex().globalMatch(_code);
    while (it.hasNext()) {
        auto m = it.next();
        QString var = m.captured();
        if (newDeps.contains(var))
            continue;
        for (auto p : std::as_const(globalParams))
            if (p->alias() == var) {
                newDeps.insert(var, p);
                break;
            }
    }
    Parameters oldDeps(_deps);
    for (auto p : std::as_const(oldDeps))
        if (!newDeps.contains(p->alias()))
            removeDep(p);
    for (auto it = newDeps.cbegin(); it != newDeps.cend(); it++) {
        bool alreadyExists = false;
        for (auto p : std::as_const(_deps))
            if (p->alias() == it.key()) {
                alreadyExists = true;
                break;
            }
        if (!alreadyExists)
            addDep(it.value());
    }
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
        Q_ASSERT(_items[p]->target() == p);
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
    qDeleteAll(_items);
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
    for (auto it = _items.cbegin(); it != _items.cend(); it++) {
        auto formula = it.value();
        auto deps = formula->deps();
        for (auto param : std::as_const(deps))
            if (param == whichParam)
            {
                if (!result.contains(formula->target()))
                    result << formula->target();
                break;
            }
    }
    return result;
}

bool Formulas::renameDependency(Parameter *param, const QString &newName)
{
    bool depFound = false;
    for (auto it = _items.cbegin(); it != _items.cend(); it++) {
        auto formula = it.value();
        if (formula->renameDep(param, newName))
            depFound = true;
    }
    return depFound;
}

//------------------------------------------------------------------------------

namespace FormulaUtils {

bool isValidVariableName(const QString& s)
{
    static QRegularExpression r("^[a-zA-Z_][a-zA-Z_0-9]*$");
    return r.match(s).hasMatch();
}

} // namespace ParameterUtils

} // namespace Z
