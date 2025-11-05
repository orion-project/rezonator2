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
    ~BeamCalculator();
    
    void calcRoundTrip(Element *ref, bool splitRange, const char *reason);
    
    Z::WorkPlane plane() const { return _ts; }
    void setPlane(Z::WorkPlane ts) { _ts = ts; }
    double ior() const { return _ior; }
    void setIor(double ior) { _ior = ior; }

    void multMatrix(const char *reason);
    double beamRadius();
    double frontRadius();
    double halfAngle();
    BeamResult calc();
    
    bool ok() const { return _error.isEmpty(); }
    QString error() const { return _error; }
    
    Z::Matrix matrix() const;
    std::optional<Z::Matrix> matrix(int index) const;
    Element* elem(int index) const;
    int matrixCount() const;
    double stability_normal() const;
    double stability_squared() const;
    PumpCalculator* pumpCalc() const { return _pump; };
    Element* ref() const { return _ref; }
    
private:
    Schema *_schema;
    Element *_ref = nullptr;
    QString _error;
    Z::WorkPlane _ts = Z::T;
    PumpCalculator *_pump = nullptr;
    AbcdCalculator *_abcd = nullptr;
    RoundTripCalculator *_rt = nullptr;
    double _ior = 1;
};

#endif // BEAM_CALCULATOR_H
