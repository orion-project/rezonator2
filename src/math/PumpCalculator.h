#ifndef PUMP_CALCULATOR_H
#define PUMP_CALCULATOR_H

#include "../core/Values.h"

class PumpParams;
class PumpCalculatorImpl;

namespace Z {
class Matrix;
}

class PumpCalculator final
{
public:
    PumpCalculator(PumpParams* pump, double lambdaSI, bool writeProtocol = false);
    ~PumpCalculator();

    BeamResult calc(Z::WorkPlane ts, const Z::Matrix& m, double ior) const { return calc(ts == Z::WorkPlane::T ? _implT : _implS, m, ior); }
    BeamResult calcT(const Z::Matrix& m, double ior) const { return calc(_implT, m, ior); }
    BeamResult calcS(const Z::Matrix& m, double ior) const { return calc(_implS, m, ior); }

    bool isGauss() const;
    Z::PointTS MI() const;

    Z::PointTS beamRadius(const Z::Matrix& mt, const Z::Matrix& ms, double ior) const;
    Z::PointTS frontRadius(const Z::Matrix &mt, const Z::Matrix& ms, double ior) const;
    Z::PointTS halfAngle(const Z::Matrix &mt, const Z::Matrix& ms, double ior) const;

private:
    PumpCalculatorImpl *_implT, *_implS;

    BeamResult calc(PumpCalculatorImpl* impl, const Z::Matrix& matrix, double ior) const;
};

#endif // PUMP_CALCULATOR_H
