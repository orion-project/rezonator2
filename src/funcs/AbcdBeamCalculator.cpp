#include "AbcdBeamCalculator.h"

#include "../core/Math.h"
#include "../core/Format.h"

#include <QDebug>

static bool isReal(const Z::Complex& v)
{
    static Double zero(0);
    return std::isnan(v.imag()) or Double(v.imag()).almostEqual(zero);
}

static bool isImag(const Z::Complex& v)
{
    static Double zero(0);
    return std::isnan(v.real()) or Double(v.real()).almostEqual(zero);
}

double AbcdBeamCalculator::beamRadius(const Z::Matrix& m) const
{
    auto w = sqrt(_wavelenSI * M_1_PI * 2.0 * m.B / sqrt(4.0 - SQR(m.A + m.D)));
    //qDebug() << "w =" << Z::str(w);
    return isReal(w) ? w.real() : Double::nan();
}

double AbcdBeamCalculator::frontRadius(const Z::Matrix& m) const
{
    auto r = 2.0 * m.B / (m.D - m.A);
    //qDebug() << "R =" << Z::str(r);
    return isReal(r) ? r.real() : Double::nan();
}

double AbcdBeamCalculator::halfAngle(const Z::Matrix& m) const
{
    auto v = sqrt(_wavelenSI * M_1_PI * 2.0 * m.C / sqrt(4.0 - SQR(m.A + m.D)));
    //qDebug() << "C =" << Z::str(m.C);
    //qDebug() << "V =" << Z::str(v);
    // C can be negative in a system with pure real matrices
    // and this gives a pure imaginary V while the system is stable.
    // TODO:COMPLEX: check what happens in a system with complex round-trip matrix.
     return isReal(v) ? v.real() : (isImag(v) ? v.imag() : Double::nan());
}

Z::PointTS AbcdBeamCalculator::beamRadius(const Z::Matrix& mt, const Z::Matrix& ms) const
{
    return { beamRadius(mt), beamRadius(ms) };
}

Z::PointTS AbcdBeamCalculator::frontRadius(const Z::Matrix &mt, const Z::Matrix& ms) const
{
    return { frontRadius(mt), frontRadius(ms) };
}

Z::PointTS AbcdBeamCalculator::halfAngle(const Z::Matrix &mt, const Z::Matrix& ms) const
{
    return { halfAngle(mt), halfAngle(ms) };
}
