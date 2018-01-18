#include "Formula.h"

#include <cassert>

#include <QRegExp>

namespace Z {

void Formula::calculate()
{
    if (_code.isEmpty())
    {
        _status = "Formula is empty";
        return;
    }
    bool ok;
    double value = _code.toDouble(&ok);
    if (!ok)
    {
        _status = "Invalid double value";
        return;
    }
    _target->setValue(Value(value, _target->value().unit()));
    _status.clear();
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
