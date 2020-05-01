#include "AbcdBeamCalculator.h"

#include "../core/Math.h"

double AbcdBeamCalculator::beamRadius(const Z::Matrix& m) const
{
    // TODO:COMPLEX: rewrite via self-consistent q
    double A = m.A.real();
    double B = m.B.real();
    double D = m.D.real();
    double p = 1.0 - SQR((A + D) * 0.5);
    if (p <= 0) return Double::nan();
    double w2 = _wavelenSI * qAbs(B) * M_1_PI / sqrt(p);
    if (w2 < 0) return Double::nan();
    return sqrt(w2);
}

double AbcdBeamCalculator::frontRadius(const Z::Matrix& m) const
{
    // TODO:COMPLEX: rewrite via self-consistent q
    double A = m.A.real();
    double B = m.B.real();
    double D = m.D.real();
    if (!Double(D).is(A))
        return 2 * B / (D - A);
    return (B < 0) ? -Double::infinity() : +Double::infinity();
}

double AbcdBeamCalculator::halfAngle(const Z::Matrix& m) const
{
    // TODO:COMPLEX: rewrite via self-consistent q
    double A = m.A.real();
    double C = m.C.real();
    double D = m.D.real();
    double p = 4.0 - SQR(A + D);
    if (p <= 0) return Double::nan();
    return sqrt(_wavelenSI * M_1_PI * 2.0 * qAbs(C) / sqrt(p));
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
