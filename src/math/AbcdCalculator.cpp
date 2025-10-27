#include "AbcdCalculator.h"

#include "../core/Math.h"

#include <QDebug>

AbcdCalculator::AbcdCalculator(double lambdaSI)
{
    _wavelenSI = qAbs(lambdaSI);
}

// Reverse complex beam parameter from matrix components
#define CALC_Q \
    auto g = SQR(m.A + m.D) / 4.0; \
    if (g.real() > 1.0) return Double::nan(); \
    auto q = ((m.D - m.A) / 2.0 + Z::Complex(0, 1) * sqrt(1.0 - g)) / m.B

double AbcdCalculator::beamRadius(const Z::Matrix& m, double ior) const
{
    CALC_Q;
    return sqrt(_wavelenSI/qAbs(ior) * M_1_PI / qAbs(q.imag()));

    //auto w = sqrt(_wavelenSI/ior * M_1_PI * 2.0 * m.B / sqrt(4.0 - SQR(m.A + m.D)));
    //return Z::isReal(w) ? w.real() : (Z::isImag(w) ? w.imag() : Double::nan());
}

double AbcdCalculator::frontRadius(const Z::Matrix& m, double ior) const
{
    Q_UNUSED(ior)
    CALC_Q;
    return 1 / q.real();

    //auto r = 2.0 * m.B / (m.D - m.A);
    //return Z::isReal(r) ? r.real() : Double::nan();
}

double AbcdCalculator::halfAngle(const Z::Matrix& m, double ior) const
{
    CALC_Q;
    auto l = _wavelenSI/qAbs(ior);
    auto w2 = l * M_1_PI / qAbs(q.imag());
    auto t = M_PI * w2 / l * q.real();
    auto w0 = sqrt(w2 / (1 + t*t));
    return l * M_1_PI / w0;

    //auto v = sqrt(_wavelenSI/ior * M_1_PI * 2.0 * m.C / sqrt(4.0 - SQR(m.A + m.D)));
    //return Z::isReal(v) ? v.real() : (Z::isImag(v) ? v.imag() : Double::nan());
}

Z::PointTS AbcdCalculator::beamRadius(const Z::Matrix& mt, const Z::Matrix& ms, double ior) const
{
    return { beamRadius(mt, ior), beamRadius(ms, ior) };
}

Z::PointTS AbcdCalculator::frontRadius(const Z::Matrix &mt, const Z::Matrix& ms, double ior) const
{
    return { frontRadius(mt, ior), frontRadius(ms, ior) };
}

Z::PointTS AbcdCalculator::halfAngle(const Z::Matrix &mt, const Z::Matrix& ms, double ior) const
{
    return { halfAngle(mt, ior), halfAngle(ms, ior) };
}
