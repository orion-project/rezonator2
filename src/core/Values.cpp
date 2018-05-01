#include "Values.h"
#include "Format.h"

#include <QDebug>

namespace Z {

using namespace Units;
using namespace Strs;

//------------------------------------------------------------------------------
//                                  Value
//------------------------------------------------------------------------------

QString Value::str() const
{
    if (_unit == deg() || _unit == amin())
        return Z::str(_value) % _unit->alias();
    if (_unit == rad() || _unit == none())
        return Z::str(_value);
    return Z::str(_value) % ' ' % _unit->alias();
}

QString Value::displayStr() const
{
    if (_unit == deg() || _unit == amin())
        return format(_value) % _unit->name();
    if (_unit == rad() || _unit == none())
        return format(_value);
    return format(_value) % ' ' % _unit->name();
}

QString Value::toStoredStr() const
{
    return QString("%1_%2").arg(Z::storedStr(_value)).arg(_unit->alias());
}

bool Value::fromStoredStr(const QString& s)
{
    if (s.isEmpty()) return false;

    int pos = s.indexOf('_');
    if (pos < 1) return false;

    QStringRef valueStr(&s, 0, pos);
    bool ok;
    double value = valueStr.toDouble(&ok);
    if (!ok)
    {
        qWarning() << s << "is invalid Z::Value, bad value";
        return false;
    }

    QStringRef unitStr(&s, pos+1, s.length()-pos-1);
    auto unit = Z::Units::findByAlias(unitStr.toString());
    if (!unit)
    {
        qWarning() << s << "is invalid Z::Value, unknown unit";
        return false;
    }

    _value = value;
    _unit = unit;
    return true;
}

//------------------------------------------------------------------------------
//                                  ValueTS
//------------------------------------------------------------------------------

QString ValueTS::str() const
{
    if (_unit == deg() || _unit == amin())
        return Z::str(_valueT) % _unit->alias() % ' ' % multX() % ' ' % Z::str(_valueS) % _unit->alias();
    if (_unit == rad() || _unit == none())
        return Z::str(_valueT) % ' ' % multX() % ' ' % Z::str(_valueS);
    return Z::str(_valueT) % ' ' % multX() % ' ' % Z::str(_valueS) % ' ' % _unit->alias();
}

QString ValueTS::displayStr() const
{
    if (_unit == deg() || _unit == amin())
        return format(_valueT) % _unit->name() % ' ' % multX() % ' ' % format(_valueS) % _unit->name();
    if (_unit == rad() || _unit == none())
        return format(_valueT) % ' ' % multX() % ' ' % format(_valueS);
    return format(_valueT) % ' ' % multX() % ' ' % format(_valueS) % ' ' % _unit->name();
}

//------------------------------------------------------------------------------
//                               PrefixedValue
//------------------------------------------------------------------------------

PrefixedValue::PrefixedValue(double value, Unit unit) : _value(value), _unit(unit)
{
    _prefix = Z::Units::simplify(_value);
}

QString PrefixedValue::str() const
{
    return Z::str(_value) % Z::Units::prefixNameTr(_prefix) % _unit->name();
}

//------------------------------------------------------------------------------
//                                  ValueTS
//------------------------------------------------------------------------------

template<>
QString PointTS::str() const
{
    return QString("[T: %1; S: %2]").arg(Z::str(T), Z::str(S));
}

//------------------------------------------------------------------------------
//                                DoublePoint
//------------------------------------------------------------------------------

QString DoublePoint::str() const
{
    return QString("[X: %1; Y: %2]").arg(Z::str(X), Z::str(Y));
}

} // namespace Z
