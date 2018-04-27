#ifndef VALUES_H
#define VALUES_H

#include "Units.h"
#include "core/OriFloatingPoint.h"

#include <complex>

namespace Z {

class Value
{
public:
    Value() : _value(0), _unit(Units::none()) {}
    Value(double value, Unit unit): _value(value), _unit(unit) {}
    Value(const Value& other): _value(other.value()), _unit(other.unit()) {}

    const double& value() const { return _value; }
    Unit unit() const { return _unit; }

    QString str() const;

    /// Returns string representation suitable for displaying to user.
    /// It should not be such as `0.776242642387489237489 mm`
    /// but more like to `0.776 mm` depending on application preferences.
    /// It uses @a Z::format() method to prepare value.
    QString displayStr() const;

    double toSi() const { return _unit->toSi(_value); }

    void operator = (const Value& v) { _value = v._value, _unit = v._unit; }
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

    QString toStoredStr() const;
    bool fromStoredStr(const QString& s);

private:
    double _value;
    Unit _unit;
};

//------------------------------------------------------------------------------

class ValueTS
{
public:
    ValueTS() : _valueT(0), _valueS(0), _unit(Units::none()) {}
    ValueTS(double valueT, double valueS, Unit unit) : _valueT(valueT), _valueS(valueS), _unit(unit) {}
    ValueTS(const ValueTS& other) : _valueT(other._valueT), _valueS(other._valueS), _unit(other._unit) {}

    Value valueT() const { return Value(_valueT, _unit); }
    Value valueS() const { return Value(_valueS, _unit); }
    const double& rawValueT() const { return _valueT; }
    const double& rawValueS() const { return _valueS; }
    Unit unit() const { return _unit; }

    void operator = (const ValueTS& v) { _valueT = v._valueT, _valueS = v._valueS, _unit = v._unit; }

    QString str() const;

    /// Returns string representation suitable for displaying to user.
    /// It should not be such as `0.776242642387489237489 mm`
    /// but more like to `0.776 mm` depending on application preferences.
    /// It uses @a Z::format() method to prepare value.
    QString displayStr() const;

private:
    double _valueT, _valueS;
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

template <typename TValue>
struct PairTS
{
    TValue T {};
    TValue S {};

    PairTS() {}
    PairTS(const TValue& t, const TValue& s) : T(t), S(s) {}
    PairTS(const PairTS& other) : T(other.T), S(other.S) {}
    PairTS(const PairTS* other) : T(other->T), S(other->S) {}

    void operator =(const TValue& v) { T = v, S = v; }

    void set(const TValue& t, const TValue& s) { T = t, S = s; }

    QString str() const { return QString(); }
};

using PointTS = PairTS<double>;

//------------------------------------------------------------------------------

struct DoublePoint
{
    double X = 0;
    double Y = 0;

    QString str() const;
};

using Complex = std::complex<double>;
using ComplexTS = PairTS<Complex>;

//------------------------------------------------------------------------------

} // namespace Z

#endif // VALUES_H
