#ifndef VALUES_H
#define VALUES_H

#include "../core/CommonTypes.h"
#include "../core/Complex.h"
#include "../core/Units.h"

#include "core/OriFloatingPoint.h"

namespace Z {

template <typename TValue>
struct PairTS
{
    TValue T {};
    TValue S {};

    PairTS() {}
    PairTS(const TValue& t, const TValue& s) : T(t), S(s) {}
    PairTS(const PairTS& other) : T(other.T), S(other.S) {}
    PairTS(const PairTS* other) : T(other->T), S(other->S) {}

    PairTS& operator = (const TValue& v) { T = v, S = v; return *this; }
    PairTS& operator = (const PairTS& other) { T = other.T, S = other.S; return *this; }

    void set(const TValue& t, const TValue& s) { T = t, S = s; }

    const TValue& operator[](Z::WorkPlane ts) const { return ts == Z::Plane_T ? T : S; }
    TValue& operator[](Z::WorkPlane ts) { return ts == Z::Plane_T ? T : S; }

    QString str() const;
};

using PointTS = PairTS<double>;
using PointComplexTS = PairTS<Complex>;

//------------------------------------------------------------------------------

struct DoublePoint
{
    double X = 0;
    double Y = 0;

    QString str() const;
};

using ValueSi = double;
struct RangeSi
{
    ValueSi start;
    ValueSi stop;
};

//------------------------------------------------------------------------------

class Value
{
public:
    Value() : _value(0), _unit(Units::none()) {}
    Value(double value): _value(value), _unit(Units::none()) {}
    Value(double value, Unit unit): _value(value), _unit(unit) {}
    Value(const Value& other): _value(other.value()), _unit(other.unit()) {}

    double value() const { return _value; }
    Unit unit() const { return _unit; }

    QString str() const;

    /// Returns string representation suitable for displaying to user.
    /// It should not be such as `0.776242642387489237489 mm`
    /// but more like to `0.776 mm` depending on application preferences.
    /// It uses @a Z::format() method to prepare value.
    QString displayStr() const;

    double toSi() const { return _unit->toSi(_value); }
    Z::Value toUnit(Z::Unit unit) const { return {unit->fromSi(_unit->toSi(_value)), unit}; }

    Value& operator = (const Value& v) { _value = v._value; _unit = v._unit; return *this; }
    Value& operator = (const double& v) { _value = v; return *this; }
    Value operator - () const { return Value(-_value, _unit); }
    Value operator * (const double& v) const { return Value(_value * v, _unit); }
    Value operator / (const double& v) const { return Value(_value / v, _unit); }
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

    static Value parse(const QString& valueStr);
    static Value fromSi(const double& valueSi, Unit unit) { return Value(unit->fromSi(valueSi), unit); }

private:
    double _value;
    Unit _unit;
};

struct ValuePoint
{
    Value X;
    Value Y;
};

} // namespace Z

//------------------------------------------------------------------------------
//                             Value literals
//
// They are useful for making values in natural way like:
//
// auto v = 100_mkm
//
// Should be in global namespace for convenience.

#define Z_VALUE_LITERAL(unit)\
    inline Z::Value operator "" _##unit(long double value) { return Z::Value(double(value), Z::Units::unit()); }\
    inline Z::Value operator "" _##unit(unsigned long long value) { return Z::Value(double(value), Z::Units::unit()); }

Z_VALUE_LITERAL(Ao)
Z_VALUE_LITERAL(nm)
Z_VALUE_LITERAL(mkm)
Z_VALUE_LITERAL(mm)
Z_VALUE_LITERAL(cm)
Z_VALUE_LITERAL(m)

Z_VALUE_LITERAL(deg)

namespace Z {

//------------------------------------------------------------------------------

class ValueTS
{
public:
    ValueTS() : _valueT(0), _valueS(0), _unit(Units::none()) {}
    ValueTS(double value) : _valueT(value), _valueS(value), _unit(Units::none()) {}
    ValueTS(double value, Unit unit) : _valueT(value), _valueS(value), _unit(unit) {}
    ValueTS(double valueT, double valueS) : _valueT(valueT), _valueS(valueS), _unit(Units::none()) {}
    ValueTS(double valueT, double valueS, Unit unit) : _valueT(valueT), _valueS(valueS), _unit(unit) {}
    ValueTS(const ValueTS& other) : _valueT(other._valueT), _valueS(other._valueS), _unit(other._unit) {}
    ValueTS(const Value& v) : _valueT(v.value()), _valueS(v.value()), _unit(v.unit()) {}

    Value valueT() const { return Value(_valueT, _unit); }
    Value valueS() const { return Value(_valueS, _unit); }
    double rawValueT() const { return _valueT; }
    double rawValueS() const { return _valueS; }
    Unit unit() const { return _unit; }

    ValueTS& operator = (const ValueTS& v) { _valueT = v._valueT; _valueS = v._valueS; _unit = v._unit; return *this; }
    ValueTS& operator = (const Value& v) { _valueT = v.value(); _valueS = v.value(); _unit = v.unit(); return *this; }
    ValueTS operator - () const { return ValueTS(-_valueT, -_valueS, _unit); }

    QString str() const;

    /// Returns string representation suitable for displaying to user.
    /// It should not be such as `0.776242642387489237489 mm`
    /// but more like to `0.776 mm` depending on application preferences.
    /// It uses @a Z::format() method to prepare value.
    QString displayStr() const;

    PointTS toSi() const { return PointTS(_unit->toSi(_valueT), _unit->toSi(_valueS)); }

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
    QString format() const;

private:
    double _value;
    Unit _unit;
    Z::Units::Prefix _prefix;
};

//------------------------------------------------------------------------------

} // namespace Z

#endif // VALUES_H
