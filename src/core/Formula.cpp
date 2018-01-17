#include "Formula.h"

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
    _target->setValue(Value(value, Units::none()));
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
    qDeleteAll(_items.values());
    _items.clear();
}

} // namespace Z
