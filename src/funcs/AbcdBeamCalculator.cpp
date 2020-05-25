#include "AbcdBeamCalculator.h"

#include "../core/Math.h"

double AbcdBeamCalculator::beamRadius(const Z::Matrix& m, double ior) const
{
    double p = 1 - SQR((m.A + m.D) * 0.5);
    if (p <= 0) return Double::nan();
    double w2 = _wavelenSI/ior * qAbs(m.B) * M_1_PI / sqrt(p);
    if (w2 < 0) return Double::nan();
    return sqrt(w2);
}

double AbcdBeamCalculator::frontRadius(const Z::Matrix& m, double ior) const
{
    Q_UNUSED(ior)
    if (!Double(m.D).is(m.A))
        return 2 * m.B / (m.D - m.A);
    return (m.B < 0) ? -Double::infinity() : +Double::infinity();
}

double AbcdBeamCalculator::halfAngle(const Z::Matrix& m, double ior) const
{
    double p = 4.0 - SQR(m.A + m.D);
    if (p <= 0) return Double::nan();
    return sqrt(_wavelenSI/ior * M_1_PI * 2.0 * qAbs(m.C) / sqrt(p));
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
