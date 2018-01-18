#include "Values.h"
#include "Format.h"

#include <QDebug>

namespace Z {

//------------------------------------------------------------------------------
//                                  Value
//------------------------------------------------------------------------------

QString Value::str() const
{
    return Z::str(_value) % ' ' % _unit->name();
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
