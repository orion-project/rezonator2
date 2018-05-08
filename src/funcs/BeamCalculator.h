#ifndef BEAM_CALCULATOR_H
#define BEAM_CALCULATOR_H

#include "../core/Math.h"
#include "../core/Values.h"

struct BeamResult
{
    double beamRadius; ///< Beam rdius.
    double frontRadius; ///< Wavefront ROC.
    double halfAngle; ///< Half of divergence angle.
};

class BeamCalculator
{
public:
    static BeamResult calcVector(const Z::RayVector& input, const Z::Matrix& matrix);
    static BeamResult calcGauss(const Z::Complex& input, const Z::Matrix& matrix, double lambda, double MI);
};

#endif // BEAM_CALCULATOR_H
