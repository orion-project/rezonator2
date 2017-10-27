#ifndef UNITS_H
#define UNITS_H

#include "core/OriTemplates.h"

#include <QString>
#include <QList>

#define DECLARE_UNIT(unit_name) Unit unit_name();
#define DECLARE_DIM(dim_name) Dim dim_name();
#define UNIT(unit_name) Z::Units::unit_name()

namespace Z {

class _Unit_;
class _Dim_;
typedef const _Unit_* Unit;
typedef const _Dim_* Dim;
typedef const QList<Unit>& UnitList;
typedef const QList<Dim>& DimList;

class _Unit_
{
public:
    virtual QString alias() const = 0;
    virtual QString name() const = 0;
    virtual double toSi(const double& value) const = 0;
    virtual double fromSi(const double& value) const = 0;
    virtual Unit siUnit() const = 0;
};

class _Dim_
{
public:
    virtual QString alias() const = 0;
    virtual QString name() const = 0;
    virtual UnitList units() const = 0;
    Unit unitByAlias(const QString& alias) const;
};

namespace Units {

DECLARE_UNIT(none)

DECLARE_UNIT(Ao)
DECLARE_UNIT(nm)
DECLARE_UNIT(mkm)
DECLARE_UNIT(mm)
DECLARE_UNIT(cm)
DECLARE_UNIT(m)

DECLARE_UNIT(mrad)
DECLARE_UNIT(rad)
DECLARE_UNIT(amin)
DECLARE_UNIT(deg)

DECLARE_UNIT(Hz)

} // namespace Units

namespace Dims {

DECLARE_DIM(none)
DECLARE_DIM(linear)
DECLARE_DIM(angular)

DimList dims();

} // namespace Dims

////////////////////////////////////////////////////////////////////////////////

namespace Units {

/// Metric prefixes of units
DECLARE_ENUM(Prefix, 0, Prefix_None, Prefix_Kilo, Prefix_Mega, Prefix_Giga, Prefix_Tera)

/// Returns a human-readable prefix name.
QString prefixNameTr(Prefix prefix);

/// Extracts maximal prefix for a value and respectively decreases the source.
/// E.g.: value = 1400000 -> Prefix_Mega, value = 1.4
Prefix simplify(double &value);

/// Returns a dimension the unit belongs to.
/// E.g.: mm -> linear, deg -> angular, none -> none
Z::Dim guessDim(Z::Unit unit);

} // namespace Units

} // namespace Z

#endif // UNITS_H
