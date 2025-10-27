#ifndef BEAM_CALCULATOR_H
#define BEAM_CALCULATOR_H

#include "AbcdCalculator.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../core/CommonTypes.h"

class Schema;

class BeamCalculator
{
public:
    BeamCalculator(Schema *schema);
    
    void calcRoundTrip(Element *ref, bool splitRange, const char *reason);
    
    Z::WorkPlane plane() const { return _ts; }
    void setPlane(Z::WorkPlane ts) { _ts = ts; }
    double ior() const { return _ior; }
    void setIor(double ior) { _ior = ior; }

    double beamRadius(Z::WorkPlane ts, double ior);
    double frontRadius(Z::WorkPlane ts, double ior);
    double halfAngle(Z::WorkPlane ts, double ior);
    
    double beamRadius() { return beamRadius(_ts, _ior); }
    double frontRadius() { return frontRadius(_ts, _ior); }
    double halfAngle() { return halfAngle(_ts, _ior); }
    
    bool ok() const { return _error.isEmpty(); }
    QString error() const { return _error; }
    
    const PumpCalculator& pumpCalc() const { return *_pump; };
    
private:
    Schema *_schema;
    QString _error;
    Z::WorkPlane _ts = Z::T;
    std::optional<PumpCalculator> _pump;
    std::optional<AbcdCalculator> _abcd;
    std::optional<RoundTripCalculator> _rt;
    double _ior = 1;
};

#endif // BEAM_CALCULATOR_H
