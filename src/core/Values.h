#ifndef VALUES_H
#define VALUES_H

#include "Units.h"
#include "core/OriFloatingPoint.h"

namespace Z {

class Value
{
public:
    Value() {}
    Value(double value, Unit unit): _value(value), _unit(unit) {}
    Value(const Value& other): _value(other.value()), _unit(other.unit()) {}

    const double& value() const { return _value; }
    Unit unit() const { return _unit; }

    QString str() const;
    QString strDebug() const;

    double toSi() const { return _unit->toSi(_value); }

    void operator = (const Value& v) { _value = v._value, _unit = v._unit; }
    void operator = (const double& v) { _value = v, _unit = Z::Units::m(); }
    bool operator == (const Value& v) const { return Double(toSi()).is(v.toSi()); }
    bool operator == (const double& v) const { return Double(toSi()).is(v); }
    bool operator != (const Value& v) const { return Double(toSi()).isNot(v.toSi()); }
    bool operator != (const double& v) const { return Double(toSi()).isNot(v); }
    bool operator > (const Value& v) const { return toSi() > v.toSi(); }
    bool operator > (const double& v) const { return toSi() > v; }
    bool operator >= (const Value& v) const { return toSi() >= v.toSi(); }
    bool operator >= (const double& v) const { return toSi() >= v; }
    bool operator < (const Value& v) const { return toSi() < v.toSi(); }
    bool operator < (const double& v) const { return toSi() < v; }
    bool operator <= (const Value& v) const { return toSi() <= v.toSi(); }
    bool operator <= (const double& v) const { return toSi() <= v; }

private:
    double _value;
    Unit _unit;
};

//------------------------------------------------------------------------------

class PrefixedValue
{
public:
    PrefixedValue(double value, Unit unit);

    QString str() const;

private:
    double _value;
    Unit _unit;
    Z::Units::Prefix _prefix;
};

//------------------------------------------------------------------------------

struct ValueTS
{
    double T;
    double S;

    ValueTS() : T(0), S(0) {}
    ValueTS(const double& t, const double& s) : T(t), S(s) {}
    ValueTS(const ValueTS& other) : T(other.T), S(other.S) {}
    ValueTS(const ValueTS* other) : T(other->T), S(other->S) {}

    void operator =(const double& v) { T = v, S = v; }

    void set(const double& t, const double& s) { T = t, S = s; }

    QString str() const;
};

//------------------------------------------------------------------------------

struct DoublePoint
{
    double X;
    double Y;

    QString str() const;
};

//------------------------------------------------------------------------------

} // namespace Z

#endif // VALUES_H
