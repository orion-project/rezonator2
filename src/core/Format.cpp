#include "Format.h"

#include "../AppSettings.h"

namespace Z {

QString str(const Complex& v)
{
    return QString("%1 %2 i%3")
            .arg(v.real(), 0, 'g', 16)
            .arg(v.imag() < 0 ? '-' : '+')
            .arg(v.imag(), 0, 'g', 16);
}

QString format(const double& v)
{
    return QString::number(v, 'g', AppSettings::instance().numberPrecisionData);
}

QString format(const Complex& v)
{
    auto precision = AppSettings::instance().numberPrecisionData;
    return QString("%1 %2 i%3")
            .arg(v.real(), 0, 'g', precision)
            .arg(v.imag() < 0 ? '-' : '+')
            .arg(v.imag(), 0, 'g', precision);
}

} // namespace Z
