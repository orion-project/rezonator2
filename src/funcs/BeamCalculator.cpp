#include "BeamCalculator.h"

#include "GaussCalculator.h"

#include <QDebug>

using namespace Z;

BeamResult BeamCalculator::calcVector(const RayVector& input, const Matrix& matrix)
{
    RayVector output(input, matrix);
    BeamResult beam;
    beam.beamRadius = output.Y;
    beam.halfAngle = output.V;
    beam.frontRadius = output.Y / sin(output.V);
    return beam;
}

BeamResult BeamCalculator::calcGauss(const Z::Complex& input, const Z::Matrix& matrix, double lambda, double MI)
{
    Complex output = matrix.multComplexBeam(input);

    GaussCalculator gauss;
    gauss.setMI(MI);
    gauss.setLambda(lambda);
    gauss.setLock(GaussCalculator::Lock::Front);
    gauss.setReQ(output.real());
    gauss.setImQ(output.imag());
    gauss.calc();

    BeamResult beam;
    beam.beamRadius = gauss.w();
    beam.halfAngle = gauss.Vs();
    beam.frontRadius = gauss.R();
    return beam;
}
