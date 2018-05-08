#include "BeamCalculator1.h"

#include "BeamCalculator.h"

using namespace Z;

BeamResult BeamCalculator1::calcVector(const RayVector& input, const Matrix& matrix)
{
    RayVector output(input, matrix);
    BeamResult beam;
    beam.beamRadius = output.Y;
    beam.halfAngle = output.V;
    beam.frontRadius = 0; // TODO calculate
    return beam;
}

BeamResult BeamCalculator1::calcGauss(const Z::Complex& input, const Z::Matrix& matrix, double lambda, double MI)
{
    Complex output = matrix.multComplexBeam(input);

    BeamCalculator gauss;
    gauss.setM2(MI);
    gauss.setLambda(lambda);
    gauss.setLock(BeamCalculator::Lock::Front);
    gauss.setReQ1(output.real());
    gauss.setImQ1(output.imag());
    gauss.calc();

    BeamResult beam;
    beam.beamRadius = gauss.w();
    beam.halfAngle = gauss.R();
    beam.frontRadius = gauss.Vs();
    return beam;
}
