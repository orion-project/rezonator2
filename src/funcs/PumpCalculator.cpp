#include "PumpCalculator.h"

#include "../core/Math.h"
#include "../core/Pump.h"
#include "../core/Protocol.h"

using namespace Z;

//--------------------------------------------------------------------------------
//                               PumpCalculatorImpl
//--------------------------------------------------------------------------------

class PumpCalculatorImpl final
{
    friend class PumpCalculator;
    using GetPumpParam = std::const_mem_fun_t<double, Z::ValueTS>;

    PumpCalculatorImpl(GetPumpParam f) : getPumpParam(f) {}

    GetPumpParam getPumpParam;
    enum { GAUSS, RAY_VECTOR } mode;
    const char* id;
    double MI;
    double wavelen; // Schema wavelength, we always treat pumps as located in vacuum
    bool protocol;
    Complex inputQ {0, 0};
    RayVector inputRay {0, 0};

    inline double paramValueSI(ParameterTS* param)
    {
        return param->value().unit()->toSi(getPumpParam(&param->value()));
    }

    inline double paramValueInvSI(ParameterTS* param)
    {
        // When one wants to convert inverted non-SI to inverted SI units,
        // one can use fromSi() function as it have inverted action of toSi().
        return param->value().unit()->fromSi(getPumpParam(&param->value()));
    }

    template <class TPumpParams, typename ...Lambda>
    bool initInput(PumpParams* pump, const char* ident, bool writeProtocol, Lambda ...lambda)
    {
        auto p = dynamic_cast<TPumpParams*>(pump);
        if (!p) return false;
        initInput(p, ident, writeProtocol, lambda...);
        return true;
    }

    void initInput(PumpParams_Waist *pump, const char* ident, bool writeProtocol, double lambda)
    {
        mode = GAUSS;
        id = ident;
        wavelen = lambda;
        protocol = writeProtocol;
        MI = qAbs(getPumpParam(&pump->MI()->value()));
        const double z = paramValueSI(pump->distance());
        const double w0_hyper = paramValueSI(pump->waist());
        const double w0_equiv_2 = SQR(w0_hyper) / MI;
        const double z0_equiv = M_PI * w0_equiv_2 / lambda;
        const double w_equiv_2 = w0_equiv_2 * (1 + SQR(z / z0_equiv));
        if (Double(z).almostEqual(Double(0)))
        {
            inputQ = 1.0 / Complex(0, lambda / M_PI / w_equiv_2);
        }
        else
        {
            const double R_equiv = z * (1 + SQR(z0_equiv / z));
            inputQ = 1.0 / Complex(1.0 / R_equiv, lambda / M_PI / w_equiv_2);
        }
        if (protocol)
            Z_INFO(id << "Q_in =" << Z::str(inputQ))
    }

    void initInput(PumpParams_Front *pump, const char* ident, bool writeProtocol, double lambda)
    {
        mode = GAUSS;
        id = ident;
        wavelen = lambda;
        protocol = writeProtocol;
        MI = qAbs(getPumpParam(&pump->MI()->value()));
        const double w_hyper = paramValueSI(pump->beamRadius());
        const double w_equiv_2 = SQR(w_hyper) / MI;
        const double R_equiv = paramValueSI(pump->frontRadius());
        inputQ = 1.0 / Complex(1.0 / R_equiv, lambda / M_PI / w_equiv_2);
        if (protocol)
            Z_INFO(id << "Q_in =" << Z::str(inputQ))
    }

    void initInput(PumpParams_RayVector *pump, const char* ident, bool writeProtocol)
    {
        mode = RAY_VECTOR;
        id = ident;
        protocol = writeProtocol;
        const double y = paramValueSI(pump->radius());
        const double v = paramValueSI(pump->angle());
        const double z = paramValueSI(pump->distance());
        inputRay.set(y + z * tan(v), v);
    }

    void initInput(PumpParams_TwoSections *pump, const char* ident, bool writeProtocol)
    {
        mode = RAY_VECTOR;
        id = ident;
        protocol = writeProtocol;
        const double y1 = paramValueSI(pump->radius1());
        const double y2 = paramValueSI(pump->radius2());
        const double z = paramValueSI(pump->distance());
        inputRay.set(y2, atan((y2 - y1) / z));
    }

    void initInput(PumpParams_Complex *pump, const char* ident, bool writeProtocol, double lambda)
    {
        mode = GAUSS;
        id = ident;
        wavelen = lambda;
        protocol = writeProtocol;
        MI = qAbs(getPumpParam(&pump->MI()->value()));
        Complex q_inv_hyper = 1.0 / Complex(paramValueSI(pump->real()),
                                            paramValueSI(pump->imag()));
        inputQ = 1.0 / Complex(q_inv_hyper.real(),
                               q_inv_hyper.imag() * MI);
        if (protocol)
            Z_INFO(id << "Q_in =" << Z::str(inputQ))
    }

    void initInput(PumpParams_InvComplex *pump, const char* ident, bool writeProtocol, double lambda)
    {
        mode = GAUSS;
        id = ident;
        wavelen = lambda;
        protocol = writeProtocol;
        MI = qAbs(getPumpParam(&pump->MI()->value()));
        // Inverted complex beam parameter is measured in inverted units (1/m, 1/mm)
        // and value's unit actually means 1/unit for this case (e.g. mm means 1/mm).
        inputQ = 1.0 / Complex(paramValueInvSI(pump->real()),
                               paramValueInvSI(pump->imag()) * MI);
        if (protocol)
            Z_INFO(id << "Q_in =" << Z::str(inputQ))
    }

    BeamResult calcVector(const Matrix& matrix)
    {
        BeamResult beam;
        if (matrix.isReal())
        {
            RayVector output(inputRay, matrix);
            beam.beamRadius = output.Y;
            beam.halfAngle = output.V;
            beam.frontRadius = output.Y / sin(output.V);
        }
        else
        {
            // Geometric pump can't be used with complex matrices
            beam.beamRadius = Double::nan();
            beam.halfAngle = Double::nan();
            beam.frontRadius = Double::nan();
        }
        return beam;
    }

    BeamResult calcGauss(const Matrix& matrix, double ior)
    {
        const double lambda = wavelen / ior;

        // In Gerrard and Burch's "Introduction to matric methods in optics"
        // q is defined as 1/q = 1/R + i*lambda/pi/w^2
        // while in Seigman's "Lasers" and in Yariv's "Quantum electronics"
        // it is defined as 1/q = 1/R - i*lambda/pi/w^2 (negative imaginary part)
        // Both definitions are transformed by matrices in the same way
        // and give positive or negative imaginary part respectively.
        // See bin/test_files/complex_pump.rez
        // That's way we use qAbs(q_inv.imag() in calculations below.
        Complex q_cur = matrix.multComplexBeam(inputQ);
        Complex q_inv = 1.0 / q_cur;
        if (protocol)
            Z_INFO(id << "Q =" << Z::str(q_cur) << "| 1/Q =" << Z::str(q_inv))
        const double R = 1.0 / q_inv.real();
        const double w_equiv_2 = lambda / M_PI / qAbs(q_inv.imag());
        const double w_hyper = sqrt(w_equiv_2 * MI);

        // see GaussCalculator for next formulas
        // calc_Z_from_R_W_MI(), calc_Z0_from_R_Z()
        const double tmp = SQR(w_equiv_2 * M_PI);
        const double z = tmp * R / (SQR(lambda * R) + tmp);
        const double z0_equiv = sqrt(z * (R - z));

        const double z0_hyper = z0_equiv;
        const double w0_hyper = sqrt(z0_hyper * MI * lambda / M_PI);
        const double Vs_hyper = w0_hyper / z0_hyper;

        BeamResult beam;
        beam.beamRadius = w_hyper;
        beam.halfAngle = Vs_hyper;
        beam.frontRadius = R;
        return beam;
    }
};

//--------------------------------------------------------------------------------
//                                PumpCalculator
//--------------------------------------------------------------------------------

std::shared_ptr<PumpCalculator> PumpCalculator::T()
{
    PumpCalculator *c = new PumpCalculator();
    c->_impl = new PumpCalculatorImpl(std::mem_fun(&ValueTS::rawValueT));
    return std::shared_ptr<PumpCalculator>(c);
}

std::shared_ptr<PumpCalculator> PumpCalculator::S()
{
    PumpCalculator *c = new PumpCalculator();
    c->_impl = new PumpCalculatorImpl(std::mem_fun(&ValueTS::rawValueS));
    return std::shared_ptr<PumpCalculator>(c);
}

PumpCalculator::~PumpCalculator()
{
    if (_impl) delete _impl;
}

bool PumpCalculator::init(PumpParams* pump, double lambdaSI, const char *ident, bool writeProtocol)
{
    // InvComplex should be tested before Complex, because Complex is more generic
    return _impl->initInput<PumpParams_Waist>(pump, ident, writeProtocol, lambdaSI) ||
           _impl->initInput<PumpParams_Front>(pump, ident, writeProtocol, lambdaSI) ||
           _impl->initInput<PumpParams_InvComplex>(pump, ident, writeProtocol, lambdaSI) ||
           _impl->initInput<PumpParams_Complex>(pump, ident, writeProtocol, lambdaSI) ||
           _impl->initInput<PumpParams_RayVector>(pump, ident, writeProtocol) ||
           _impl->initInput<PumpParams_TwoSections>(pump, ident, writeProtocol);
}

BeamResult PumpCalculator::calc(const Z::Matrix& matrix, double ior)
{
    switch (_impl->mode)
    {
    case PumpCalculatorImpl::GAUSS:
        return _impl->calcGauss(matrix, ior);

    case PumpCalculatorImpl::RAY_VECTOR:
        return _impl->calcVector(matrix);
    }

    qCritical() << "Unsupported pump calculation mode" << int(_impl->mode);
    return { Double::nan(), Double::nan(), Double::nan() };
}
