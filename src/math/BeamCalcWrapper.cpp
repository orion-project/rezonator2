#include "BeamCalcWrapper.h"

#include "AbcdCalculator.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"

BeamCalcWrapper::BeamCalcWrapper(Z::WorkPlane ts, PumpCalculator *pump, 
    AbcdCalculator *abcd, RoundTripCalculator* rt, double ior) 
    : _ts(ts), _pump(pump), _abcd(abcd), _rt(rt), _ior(ior)
{
}

double BeamCalcWrapper::beamRadius()
{
    if (_pump) {
        BeamResult r = _pump->calc(_ts, _rt->M(_ts), _ior);
        return r.beamRadius;
    }
    if (_abcd) {
        Z::PointTS r = _abcd->beamRadius(_rt->Mt(), _rt->Ms(), _ior);
        return r[_ts];
    }
    return qQNaN();
}

double BeamCalcWrapper::frontRadius()
{
    if (_pump) {
        BeamResult r = _pump->calc(_ts, _rt->M(_ts), _ior);
        return r.frontRadius;
    }
    if (_abcd) {
        Z::PointTS r = _abcd->frontRadius(_rt->Mt(), _rt->Ms(), _ior);
        return r[_ts];
    }
    return qQNaN();
}

double BeamCalcWrapper::halfAngle()
{
    if (_pump) {
        BeamResult r = _pump->calc(_ts, _rt->M(_ts), _ior);
        return r.halfAngle;
    }
    if (_abcd) {
        Z::PointTS r = _abcd->halfAngle(_rt->Mt(), _rt->Ms(), _ior);
        return r[_ts];
    }
    return qQNaN();
}
