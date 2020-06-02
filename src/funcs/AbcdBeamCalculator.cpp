#include "AbcdBeamCalculator.h"

#include "../core/Math.h"
#include "../core/Format.h"

#include <QDebug>

static bool isReal(const Z::Complex& v)
{
#ifdef Q_OS_MAC
    // While Windows and Linux versions give exact zero here,
    // macOS version gives some small value (not digged why).
    // Probably this epsilon has to be adjusted, it's given from tests.
    return std::isnan(v.imag()) or qAbs(v.imag()) <= 1e-18;
#else
    static Double zero(0);
    return std::isnan(v.imag()) or Double(v.imag()).almostEqual(zero);
#endif
}

static bool isImag(const Z::Complex& v)
{
#ifdef Q_OS_MAC
    // While Windows and Linux versions give exact zero here,
    // macOS version gives some small value (not digged why)
    // Probably this epsilon has to be adjusted, it's given from tests.
    return std::isnan(v.real()) or qAbs(v.real()) <= 1e-18;
#else
    static Double zero(0);
    return std::isnan(v.real()) or Double(v.real()).almostEqual(zero);
#endif
}

double AbcdBeamCalculator::beamRadius(const Z::Matrix& m, double ior) const
{
    auto w = sqrt(_wavelenSI/ior * M_1_PI * 2.0 * m.B / sqrt(4.0 - SQR(m.A + m.D)));
    //qDebug() << "B =" << Z::str(m.B) << "W =" << Z::str(w);
    // B can be negative in a system with pure real matrices
    // and this gives a pure imaginary W while the system is stable.
    // TODO:COMPLEX: check what happens in a system with complex round-trip matrix.
    return isReal(w) ? w.real() : (isImag(w) ? w.imag() : Double::nan());
}

double AbcdBeamCalculator::frontRadius(const Z::Matrix& m, double ior) const
{
    Q_UNUSED(ior)
    auto r = 2.0 * m.B / (m.D - m.A);
    //qDebug() << "R =" << Z::str(r);
    return isReal(r) ? r.real() : Double::nan();
}

double AbcdBeamCalculator::halfAngle(const Z::Matrix& m, double ior) const
{
    auto v = sqrt(_wavelenSI/ior * M_1_PI * 2.0 * m.C / sqrt(4.0 - SQR(m.A + m.D)));
    qDebug() << "C =" << Z::str(m.C) << "V =" << Z::str(v);
    // C can be negative in a system with pure real matrices
    // and this gives a pure imaginary V while the system is stable.
    // TODO:COMPLEX: check what happens in a system with complex round-trip matrix.
     return isReal(v) ? v.real() : (isImag(v) ? v.imag() : Double::nan());
}

Z::PointTS AbcdBeamCalculator::beamRadius(const Z::Matrix& mt, const Z::Matrix& ms, double ior) const
{
    return { beamRadius(mt, ior), beamRadius(ms, ior) };
}

Z::PointTS AbcdBeamCalculator::frontRadius(const Z::Matrix &mt, const Z::Matrix& ms, double ior) const
{
    return { frontRadius(mt, ior), frontRadius(ms, ior) };
}

Z::PointTS AbcdBeamCalculator::halfAngle(const Z::Matrix &mt, const Z::Matrix& ms, double ior) const
{
    return { halfAngle(mt, ior), halfAngle(ms, ior) };
}
