#ifndef ABCD_BEAM_CALCULATOR_H
#define ABCD_BEAM_CALCULATOR_H

#include "../core/Values.h"

namespace Z {
class Matrix;
} // namespace Z

class AbcdBeamCalculator
{
public:
    void setWavelenSI(double wavelen) { _wavelenSI = wavelen; }

    double beamRadius(const Z::Matrix& m) const;
    double frontRadius(const Z::Matrix &m) const;
    double halfAngle(const Z::Matrix &m) const;

    Z::PointTS beamRadius(const Z::Matrix& mt, const Z::Matrix& ms) const;
    Z::PointTS frontRadius(const Z::Matrix &mt, const Z::Matrix& ms) const;
    Z::PointTS halfAngle(const Z::Matrix &mt, const Z::Matrix& ms) const;

private:
    double _wavelenSI;
};

#endif // ABCD_BEAM_CALCULATOR_H
