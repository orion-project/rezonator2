#include "Units.h"

#include <math.h>
#include <QApplication>

inline QString translateUnits(const char* s)
{
// Qt5 supposes source code is UTF8 but Qt4 thinks it is ASCII
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    return QApplication::translate("Units", QString::fromUtf8(s).toAscii().data());
#else
    return QApplication::translate("Units", s);
#endif
}


#define DEFINE_UNIT(_unit_, _unit_name_, _to_si_formula_, _from_si_formula_, _si_unit_)\
    Unit _unit_(); \
    class _Unit_##_unit_ : public _Unit_\
    {\
    public:\
        ~_Unit_##_unit_() override; \
        QString alias() const override { return QStringLiteral(#_unit_); }\
        QString name() const override { return translateUnits(_unit_name_); }\
        double toSi(const double& v) const override { return _to_si_formula_; }\
        double fromSi(const double& v) const override { return _from_si_formula_; }\
        Unit siUnit() const override { return _si_unit_(); }\
    };\
    Unit _unit_()\
    {\
        static _Unit_##_unit_ u;\
        return &u;\
    }\
    _Unit_##_unit_::~_Unit_##_unit_() {} // out-of-line virtual function definition

#define DEFINE_DIM(_dim_, _dim_name_, _si_unit_, ...)\
    class _Dim_##_dim_ : public _Dim_\
    {\
    public:\
        ~_Dim_##_dim_() override; \
        QString alias() const override { return QStringLiteral(#_dim_); }\
        QString name() const override { return translateUnits(_dim_name_); }\
        UnitList units() const override\
        {\
            static QList<Unit> unitList({__VA_ARGS__});\
            return unitList;\
        }\
        Unit siUnit() const override { return _si_unit_; }\
    };\
    Dim _dim_()\
    {\
        static _Dim_##_dim_ dim;\
        return &dim;\
    }\
    _Dim_##_dim_::~_Dim_##_dim_() {} // out-of-line virtual function definition

namespace Z {

_Unit_::~_Unit_()
{
}

_Dim_::~_Dim_()
{
}

Unit _Dim_::unitByAlias(const QString& alias) const
{
    for (Unit u : units())
        if (u->alias() == alias)
            return u;
    return nullptr;
}

Unit _Dim_::unitByAliasOrSi(const QString& alias) const
{
    auto unit = unitByAlias(alias);
    return unit ? unit : siUnit();
}

//------------------------------------------------------------------------------

namespace Units {

DEFINE_UNIT(none, "", v, v, none)

DEFINE_UNIT(m, QT_TRANSLATE_NOOP_UTF8("Units", "m"), v, v, m) // SI-unit must be first
DEFINE_UNIT(Ao, QT_TRANSLATE_NOOP_UTF8("Units", "Å"), v * 1e-10, v * 1e+10, m)
DEFINE_UNIT(nm, QT_TRANSLATE_NOOP_UTF8("Units", "nm"), v * 1e-9, v * 1e+9, m)
DEFINE_UNIT(mkm, QT_TRANSLATE_NOOP_UTF8("Units", "µm"), v * 1e-6, v * 1e+6, m)
DEFINE_UNIT(mm, QT_TRANSLATE_NOOP_UTF8("Units", "mm"), v * 0.001, v * 1000, m)
DEFINE_UNIT(cm, QT_TRANSLATE_NOOP_UTF8("Units", "cm"), v * 0.01, v * 100, m)

DEFINE_UNIT(rad, QT_TRANSLATE_NOOP_UTF8("Units", "rad"), v, v, rad) // SI-unit must be first
DEFINE_UNIT(mrad, QT_TRANSLATE_NOOP_UTF8("Units", "mrad"), v * 0.001, v * 1000, rad)
DEFINE_UNIT(amin, QT_TRANSLATE_NOOP_UTF8("Units", "′"), v / 60.0 * M_PI / 180.0, v * 180 / M_PI * 60, rad)
DEFINE_UNIT(deg, QT_TRANSLATE_NOOP_UTF8("Units", "°"), v * M_PI / 180.0, v * 180.0 / M_PI, rad)

DEFINE_UNIT(Hz, QT_TRANSLATE_NOOP_UTF8("Units", "Hz"), v, v, Hz)
DEFINE_UNIT(inv_m2, QT_TRANSLATE_NOOP_UTF8("Units", "1/m²"), v, v, inv_m2)

} // namespace Units

//------------------------------------------------------------------------------

namespace Dims {

DEFINE_DIM(none, QT_TRANSLATE_NOOP("Units", "None"), UNIT(none),
    UNIT(none),
)

DEFINE_DIM(linear, QT_TRANSLATE_NOOP("Units", "Linear"), UNIT(m),
    UNIT(Ao),
    UNIT(nm),
    UNIT(mkm),
    UNIT(mm),
    UNIT(cm),
    UNIT(m),
)

DEFINE_DIM(angular, QT_TRANSLATE_NOOP("Units", "Angular"), UNIT(rad),
    UNIT(mrad),
    UNIT(rad),
    UNIT(amin),
    UNIT(deg),
)

DEFINE_DIM(fixed, QT_TRANSLATE_NOOP("Units", "Misc"), UNIT(none),
    UNIT(Hz),
    UNIT(inv_m2),
)

DimList dims()
{
    static QList<Dim> dims({none(), linear(), angular(), fixed()});
    return dims;
}

Dim findByAlias(const QString& alias)
{
    for (auto dim : dims())
        if (dim->alias() == alias)
            return dim;
    return nullptr;
}

Dim findByAliasOrNone(const QString& alias)
{
    auto dim = findByAlias(alias);
    return dim ? dim : none();
}

} // namespace Dims

//------------------------------------------------------------------------------

namespace Units {

QString prefixNameTr(Prefix prefix)
{
    switch (prefix)
    {
    case Prefix_None: return QString();
    case Prefix_Kilo: return QApplication::translate("Units", "k", "Decimal prefix");
    case Prefix_Mega: return QApplication::translate("Units", "M", "Decimal prefix");
    case Prefix_Giga: return QApplication::translate("Units", "G", "Decimal prefix");
    case Prefix_Tera: return QApplication::translate("Units", "T", "Decimal prefix");
    }
    return QString();
}

const double PrefixValues[ENUM_COUNT(Prefix)] = { 1, 1e3, 1e6, 1e9, 1e12 };

Prefix simplify(double& value)
{
    Prefix p;
    for (p = ENUM_MIN(Prefix); p < ENUM_MAX(Prefix); p = Prefix(p+1))
        if (qAbs(value) < PrefixValues[p+1])
        {
            value /= PrefixValues[p];
            return p;
        }
    value /= PrefixValues[p];
    return p;
}

Dim guessDim(Unit unit)
{
    for (auto d : Dims::dims())
        for (auto u : d->units())
            if (unit == u)
                return d;
    return Dims::none();
}

Unit findByAlias(const QString& alias, Unit defaultUnit)
{
    for (const Dim& dim : Dims::dims())
    {
        Unit unit = dim->unitByAlias(alias);
        if (unit) return unit;
    }
    return defaultUnit;
}

} // namespace Units

} // namespace Z
