#include "Formula.h"
#include "Protocol.h"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "muParser.h"
#pragma GCC diagnostic warning "-Wdeprecated-declarations"

#include <cassert>

#include <QRegExp>

namespace Z {

bool Formula::prepare(Parameters& availableDeps)
{
    if (_code.isEmpty())
    {
        _status = "Formula is empty";
        Z_ERROR(QString("Empty formula for param '%1'").arg(_target->alias()));
        return false;
    }
    try
    {

    }
    catch (mu::ParserError &e)
    {
        if (e.GetCode() == mu::ecUNASSIGNABLE_TOKEN)
        {
            auto depName = QString::fromStdString(e.GetToken());
            if (depName.isEmpty())
            {
                _status = QString::fromStdString(e.GetMsg());
                Z_ERROR(QString("Bad formula for param '%1': %2 (code=%3, pos=%4, token=%5)")
                        .arg(_target->alias()).arg(_status).arg(e.GetCode()).arg(e.GetPos())
                        .arg(QString::fromStdString(e.GetToken())));
            }
            auto dep = availableDeps.byAlias(depName);
            if (!dep)
            {
                _status = QString::fromStdString(e.GetMsg());
                Z_ERROR(QString("Bad formula for param '%1': %2 (code=%3, pos=%4, token=%5)")
                        .arg(_target->alias()).arg(_status).arg(e.GetCode()).arg(e.GetPos())
                        .arg(QString::fromStdString(e.GetToken())));
            }
        }
        else
        {
            _status = QString::fromStdString(e.GetMsg());
            Z_ERROR(QString("Bad formula for param '%1': %2 (code=%3, pos=%4, token=%5)")
                    .arg(_target->alias()).arg(_status).arg(e.GetCode()).arg(e.GetPos())
                    .arg(QString::fromStdString(e.GetToken())));
        }
    }
    return false;
}

void Formula::calculate()
{
    if (_code.isEmpty())
    {
        _status = "Formula is empty";
        Z_ERROR(QString("Empty formula for param '%1'").arg(_target->alias()));
        return;
    }
    auto unit = _target->value().unit();
    try
    {
        mu::Parser p;
        p.SetExpr(_code.toStdString());

        // TODO: add names and values for deps
        double value = p.Eval();
        _target->setValue(Value(value, unit));
        _status.clear();
    }
    catch (mu::ParserError &e)
    {
        _status = QString::fromStdString(e.GetMsg());
        Z_ERROR(QString("Bad formula for param '%1': %2 (code=%3, pos=%4, token=%5)")
                .arg(_target->alias()).arg(_status).arg(e.GetCode()).arg(e.GetPos())
                .arg(QString::fromStdString(e.GetToken())));
    }
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
