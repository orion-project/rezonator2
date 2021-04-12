#ifndef ABCD_BEAM_CALCULATOR_H
#define ABCD_BEAM_CALCULATOR_H

#include "../core/Values.h"

namespace Z {
class Matrix;
} // namespace Z

class AbcdBeamCalculator
{
public:
    AbcdBeamCalculator(double lambdaSI);

    double beamRadius(const Z::Matrix& m, double ior) const;
    double frontRadius(const Z::Matrix &m, double ior) const;
    double halfAngle(const Z::Matrix &m, double ior) const;

    Z::PointTS beamRadius(const Z::Matrix& mt, const Z::Matrix& ms, double ior) const;
    Z::PointTS frontRadius(const Z::Matrix &mt, const Z::Matrix& ms, double ior) const;
    Z::PointTS halfAngle(const Z::Matrix &mt, const Z::Matrix& ms, double ior) const;

private:
    double _wavelenSI;
};

#endif // ABCD_BEAM_CALCULATOR_H
