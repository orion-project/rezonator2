#include "Format.h"

#include "../app/AppSettings.h"

namespace Z {

inline QString imagUnit(bool useJ)
{
    return useJ ? QStringLiteral("j") : QStringLiteral("i");
}

QString str(const Complex& v)
{
    auto s = AppSettings::instancePtr();
    return QString("%1%2%3%4%5")
            .arg(v.real(), 0, 'g', 16)
            .arg(v.imag() < 0 ? '-' : '+')
            .arg(s->showImagUnitAtEnd ? QString() : imagUnit(s->showImagUnitAsJ))
            .arg(qAbs(v.imag()), 0, 'g', 16)
            .arg(s->showImagUnitAtEnd ? imagUnit(s->showImagUnitAsJ) : QString());
}

QString format(const double& v)
{
    return QString::number(v, 'g', AppSettings::instance().numberPrecisionData);
}

QString format(const Complex& v)
{
    auto s = AppSettings::instancePtr();
    return QString("%1 %2 %3%4%5")
            .arg(v.real(), 0, 'g', s->numberPrecisionData)
            .arg(v.imag() < 0 ? '-' : '+')
            .arg(s->showImagUnitAtEnd ? QString() : imagUnit(s->showImagUnitAsJ))
            .arg(qAbs(v.imag()), 0, 'g', s->numberPrecisionData)
            .arg(s->showImagUnitAtEnd ? imagUnit(s->showImagUnitAsJ) : QString());
}

} // namespace Z
