#ifndef BEAM_CALC_WRAPPER_H
#define BEAM_CALC_WRAPPER_H

#include "../core/CommonTypes.h"

class BeamCalcWrapper
{
public:
    BeamCalcWrapper(Z::WorkPlane);
    
    Z::WorkPlane plane() const { return _plane; }
    
private:
    Z::WorkPlane _plane;
};

#endif // BEAM_CALC_WRAPPER_H
