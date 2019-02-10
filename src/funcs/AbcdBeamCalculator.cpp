#include "AbcdBeamCalculator.h"

#include "../core/Math.h"

double AbcdBeamCalculator::beamRadius(const Z::Matrix& m) const
{
    double p = 1 - SQR((m.A + m.D) * 0.5);
    if (p <= 0) return Double::nan();
    double w2 = _wavelenSI * qAbs(m.B) * M_1_PI / sqrt(p);
    if (w2 < 0) return Double::nan();
    return sqrt(w2);
}

double AbcdBeamCalculator::frontRadius(const Z::Matrix& m) const
{
    if (!Double(m.D).is(m.A))
        return 2 * m.B / (m.D - m.A);
    return (m.B < 0) ? -Double::infinity() : +Double::infinity();
}

double AbcdBeamCalculator::halfAngle(const Z::Matrix& m) const
{
    double p = 4.0 - SQR(m.A + m.D);
    if (p <= 0) return Double::nan();
    return sqrt(_wavelenSI * M_1_PI * 2.0 * qAbs(m.C) / sqrt(p));
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
