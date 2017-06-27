#include "Pump.h"
//#include "Units.h"

#include <QApplication>
#include <QDebug>

namespace Z {
namespace Pump {

Params::Params()
{

}
/*
const Raw& Params::current() const
{
    switch (mode)
    {
    case PumpMode_waist: return waist;
    case PumpMode_front: return front;
    case PumpMode_complex: return complex;
    case PumpMode_icomplex: return icomplex;
    case PumpMode_vector: return vector;
    case PumpMode_sections: return sections;
    }
    qWarning() << "Unknown pump mode" << mode;
    return waist;
}

QString Params::verify() const
{
    return current().verify();
}

QString Waist::verify() const
{
    // TODO
    return QString();
}

QString Front::verify() const
{
    // TODO
    return QString();
}

QString Complex::verify() const
{
    // TODO
    return QString();
}

QString Icomplex::verify() const
{
    // TODO
    return QString();
}

QString Vector::verify() const
{
    // TODO
    return QString();
}

QString Sections::verify() const
{
    if (distance().T <= 0 || distance().S <= 0)
        return qApp->translate("Pump error", "Distance between sections must be greater than zero");
    return QString();
}
*/
} // namespace Pump
} // namespace Z
