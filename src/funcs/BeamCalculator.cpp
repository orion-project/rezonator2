#include "BeamCalculator.h"

#include "GaussCalculator.h"

using namespace Z;

BeamResult BeamCalculator::calcVector(const RayVector& input, const Matrix& matrix)
{
    RayVector output(input, matrix);
    BeamResult beam;
    beam.beamRadius = output.Y;
    beam.halfAngle = output.V;
    beam.frontRadius = 0; // TODO calculate
    return beam;
}

BeamResult BeamCalculator::calcGauss(const Z::Complex& input, const Z::Matrix& matrix, double lambda, double MI)
{
    Complex output = matrix.multComplexBeam(input);

    GaussCalculator gauss;
    gauss.setM2(MI);
    gauss.setLambda(lambda);
    gauss.setLock(GaussCalculator::Lock::Front);
    gauss.setReQ1(output.real());
    gauss.setImQ1(output.imag());
    gauss.calc();

    BeamResult beam;
    beam.beamRadius = gauss.w();
    beam.halfAngle = gauss.R();
    beam.frontRadius = gauss.Vs();
    return beam;
}
