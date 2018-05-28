#include "PumpCalculator.h"

#include "GaussCalculator.h"
#include "../core/Math.h"
#include "../core/Pump.h"

using namespace Z;

//--------------------------------------------------------------------------------
//                               PumpCalculatorImpl
//--------------------------------------------------------------------------------

class PumpCalculatorImpl final
{
    friend class PumpCalculator;

    std::const_mem_fun_t<double, Z::ValueTS> getPumpParam;
    enum { GAUSS, RAY_VECTOR } mode;
    double lambda;
    double MI;
    union {
        Complex Q;
        RayVector ray;
    } input;

    inline double paramValueSI(ParameterTS* param)
    {
        return param->value().unit()->toSi(getPumpParam(param->value()));
    }

    inline double paramValueInvSI(ParameterTS* param)
    {
        // When one wants to convert inverted non-SI to inverted SI units,
        // one can use fromSi() function as it have inverted action of toSi().
        return param->value().unit()->fromSi(getPumpParam(param->value()));
    }

    template <class TPumpParams>
    bool initInput(Z::PumpParams* pump)
    {
        auto p = dynamic_cast<TPumpParams*>(pump);
        if (!p) return false;
        initInput(p);
        return true;
    }

    void initInput(PumpParams_Waist *pump)
    {
        mode = GAUSS;
        MI = getPumpParam(pump->MI()->value());
        GaussCalculator gauss;
        gauss.setLock(GaussCalculator::Lock::Waist);
        gauss.setLambda(lambda);
        gauss.setW0(paramValueSI(pump->waist()));
        gauss.setZ(paramValueSI(pump->distance()));
        gauss.setMI(MI);
        gauss.calc();
        input.Q = Complex(gauss.reQ(), gauss.imQ());
    }

    void initInput(PumpParams_Front *pump)
    {
        mode = GAUSS;
        MI = getPumpParam(pump->MI()->value());
        GaussCalculator gauss;
        gauss.setLock(GaussCalculator::Lock::Front);
        gauss.setLambda(lambda);
        gauss.setW(paramValueSI(pump->beamRadius()));
        gauss.setR(paramValueSI(pump->frontRadius()));
        gauss.setMI(MI);
        gauss.calc();
        input.Q = Complex(gauss.reQ(), gauss.imQ());
    }

    void initInput(PumpParams_RayVector *pump)
    {
        mode = RAY_VECTOR;
        auto y = paramValueSI(pump->radius());
        auto v = paramValueSI(pump->angle());
        auto z = paramValueSI(pump->distance());
        input.ray.set(y + z * tan(v), v);
    }

    void initInput(PumpParams_TwoSections *pump)
    {
        mode = RAY_VECTOR;
        auto y1 = paramValueSI(pump->radius1());
        auto y2 = paramValueSI(pump->radius2());
        auto z = paramValueSI(pump->distance());
        input.ray.set(y2, atan((y2 - y1) / z));
    }

    void initInput(PumpParams_Complex *pump)
    {
        mode = GAUSS;
        MI = getPumpParam(pump->MI()->value());
        input.Q = Complex(paramValueSI(pump->real()),
                          paramValueSI(pump->imag()));
    }

    void initInput(PumpParams_InvComplex *pump)
    {
        mode = GAUSS;
        MI = getPumpParam(pump->MI()->value());
        // Inverted complex beam parameter is measured in inverted units (1/m, 1/mm)
        // and value's unit actually means 1/unit for this case (e.g. mm means 1/mm).
        input.Q = 1.0 / Complex(paramValueInvSI(pump->real()),
                                paramValueInvSI(pump->imag()));
    }
};

//--------------------------------------------------------------------------------
//                                PumpCalculator
//--------------------------------------------------------------------------------

PumpCalculator PumpCalculator::T()
{
    PumpCalculator c;
    c._impl->getPumpParam = std::mem_fun(&ValueTS::rawValueT);
    return c;
}

PumpCalculator PumpCalculator::S()
{
    PumpCalculator c;
    c._impl->getPumpParam = std::mem_fun(&ValueTS::rawValueS);
    return c;
}

PumpCalculator::PumpCalculator() : _impl(new PumpCalculatorImpl)
{
}

PumpCalculator::~PumpCalculator()
{
    delete _impl;
}

bool PumpCalculator::init(Z::PumpParams* pump, double lambdaSI)
{
    _impl->lambda = lambdaSI;

    // InvComplex should be tested before Complex, because Complex is more generic
    return _impl->initInput<PumpParams_Waist>(pump) ||
           _impl->initInput<PumpParams_Front>(pump) ||
           _impl->initInput<PumpParams_InvComplex>(pump) ||
           _impl->initInput<PumpParams_Complex>(pump) ||
           _impl->initInput<PumpParams_RayVector>(pump) ||
           _impl->initInput<PumpParams_TwoSections>(pump);
}

