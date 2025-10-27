#ifndef BEAM_CALC_WRAPPER_H
#define BEAM_CALC_WRAPPER_H

#include "../core/CommonTypes.h"

class AbcdCalculator;
class PumpCalculator;
class RoundTripCalculator;

class BeamCalcWrapper
{
public:
    BeamCalcWrapper(Z::WorkPlane ts, PumpCalculator* pump,
        AbcdCalculator* abcd, RoundTripCalculator* rt, double ior);
    
    Z::WorkPlane plane() const { return _ts; }
    double ior() const { return _ior; }
    
    double beamRadius();
    double frontRadius();
    double halfAngle();
    
private:
    Z::WorkPlane _ts;
    PumpCalculator *_pump;
    AbcdCalculator *_abcd;
    RoundTripCalculator *_rt;
    double _ior;
};

#endif // BEAM_CALC_WRAPPER_H
