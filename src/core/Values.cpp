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

Value Value::parse(const QString& valueStr)
{
    int i = 0;
    for (; i < valueStr.size(); i++)
        if (valueStr.at(i).isLetter())
            break;
    bool ok = false;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    double value = QStringView(valueStr).left(i).toDouble(&ok);
#else
    double value = QStringRef(&valueStr, 0, i).toDouble(&ok);
#endif
    if (!ok) return Value();
    auto unitStr = valueStr.right(valueStr.length()-i);
    auto unit = unitStr.isEmpty() ? none(): findByAlias(unitStr);
    if (!unit) return Value();
    return Value(value, unit);
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

QString PrefixedValue::format() const
{
    return Z::format(_value) % Z::Units::prefixNameTr(_prefix) % _unit->name();
}

//------------------------------------------------------------------------------
//                                  PointTS
//------------------------------------------------------------------------------

template<typename TValue> QString PairTS<TValue>::str() const
{
    return QString("str() func is indefined");
}

template<> QString PointTS::str() const
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
