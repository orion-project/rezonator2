#include <QApplication>
#include <QDebug>

#include "CausticFunction.h"
#include "../funcs/BeamCalculator.h"
#include "../funcs/BeamCalculator1.h"
#include "../funcs/PumpCalculator.h"
#include "../funcs/RoundTripCalculator.h"
#include "../core/Protocol.h"

using namespace Z;

CausticFunction::~CausticFunction()
{
}

void CausticFunction::calculate()
{
    if (!checkArguments()) return;

    auto elem = arg()->element;
    auto param = arg()->parameter;

    auto rangeElem = Z::Utils::asRange(elem);
    if (!rangeElem)
    {
        setError("CausticFunction.arg.element is not range");
        return;
    }
    _wavelenSI = schema()->wavelength().value().toSi();

    auto tmpRange = arg()->range;
    tmpRange.stop = Z::Value(rangeElem->axisLengthSI(), Z::Units::m());
    auto range = tmpRange.plottingRange();
    if (!prepareResults(range)) return;
    if (!prepareCalculator(elem, true)) return;

    auto tripType = _schema->tripType();
    if (tripType == TripType::SP)
        if (!prepareSP()) return;

    BackupAndLock locker(elem, param);

    bool stabilityChecked = !_schema->isResonator();
    auto argUnit = param->value().unit();
    Z::PointTS prevRes(Double::nan(), Double::nan());
    for (auto x : range.values())
    {
        rangeElem->setSubRangeSI(x);
        _calc->multMatrix();

        // After the first round-trip was calculated,
        // we should check if system is unstable
        if (!stabilityChecked)
        {
            stabilityChecked = true;
            auto stab = _calc->isStable();
            if (!stab.T && !stab.S)
            {
                setError(qApp->translate("Calc error", "System is unstable, can't calculate caustic"));
                return;
            }
        }

        Z::PointTS res;
        switch (tripType)
        {
        case TripType::SW:
        case TripType::RR: res = calculateResonator(); break;
        case TripType::SP: res = calculateSinglePass(); break;
        }

        double argX = argUnit->fromSi(x);

        if (_mode == FontRadius)
        {
            // If wavefront radius changes its sign, then we have a pole at waist
            if (!std::isnan(prevRes.T) && (prevRes.T * res.T) < 0)
                _results.T.addPoint(argX, Double::nan()); // finish previous segment
            if (!std::isnan(prevRes.S) && (prevRes.S * res.S) < 0)
                _results.S.addPoint(argX, Double::nan()); // finish previous segment
            prevRes = res;
        }

        convertFromSiToModeUnits(res);
        addResultPoint(argX, res);
    }

    finishResults();
}

bool CausticFunction::prepareSP()
{
    PumpParams *pump = _schema->activePump();
    if (!pump)
    {
        setError(qApp->translate("Calc error",
            "There is no active pump in the schema. "
            "Use 'Pumps' window to create a new pump or activate one of the existing ones."));
        return false;
    }

    // Calculate beam parameters at first element of the schema.

    auto pumpWaist = dynamic_cast<PumpParams_Waist*>(pump);
    if (pumpWaist)
    {
        _pumpMode = PumpMode::Gauss;
        _inQ = PumpCalculator::calcFront(pumpWaist, _wavelenSI);
        _MI = pumpWaist->MI()->value().toSi();
        return true;
    }

    auto pumpFront = dynamic_cast<PumpParams_Front*>(pump);
    if (pumpFront)
    {
        _pumpMode = PumpMode::Gauss;
        _inQ = PumpCalculator::calcFront(pumpFront, _wavelenSI);
        _MI = pumpWaist->MI()->value().toSi();
        return true;
    }

    // InvComplex should be tested before Complex, because Complex is more generic
    auto pumpInvComplex = dynamic_cast<PumpParams_InvComplex*>(pump);
    if (pumpInvComplex)
    {
        _pumpMode = PumpMode::Gauss;
        _inQ = PumpCalculator::calcFront(pumpInvComplex);
        _MI = pumpWaist->MI()->value().toSi();
        return true;
    }

    auto pumpComplex = dynamic_cast<PumpParams_Complex*>(pump);
    if (pumpComplex)
    {
        _pumpMode = PumpMode::Gauss;
        _inQ = PumpCalculator::calcFront(pumpComplex);
        _MI = pumpWaist->MI()->value().toSi();
        return true;
    }

    auto pumpRayVector = dynamic_cast<PumpParams_RayVector*>(pump);
    if (pumpRayVector)
    {
        _pumpMode = PumpMode::RayVector;
        _inRay = PumpCalculator::calcFront(pumpRayVector);
        Z_INFO("Input rays: T =" << _inRay.T.str() << "S =" << _inRay.S.str())
        return true;
    }

    auto pumpTwoSections = dynamic_cast<PumpParams_TwoSections*>(pump);
    if (pumpTwoSections)
    {
        _pumpMode = PumpMode::RayVector;
        _inRay = PumpCalculator::calcFront(pumpTwoSections);
        Z_INFO("Input rays: T =" << _inRay.T.str() << "S =" << _inRay.S.str())
        return true;
    }

    setError("Unsupported pump mode");
    return false;
}

Z::PointTS CausticFunction::calculateSinglePass() const
{
    BeamResult beamT, beamS;
    Z::PointTS result;
    switch (_pumpMode)
    {
    case PumpMode::Gauss:
        beamT = BeamCalculator1::calcGauss(_inQ.T, _calc->Mt(), _wavelenSI, _MI.T);
        beamS = BeamCalculator1::calcGauss(_inQ.S, _calc->Ms(), _wavelenSI, _MI.S);
        break;
    case PumpMode::RayVector:
        beamT = BeamCalculator1::calcVector(_inRay.T, _calc->Mt());
        beamS = BeamCalculator1::calcVector(_inRay.S, _calc->Ms());
        break;
    }
    switch (_mode)
    {
    case BeamRadius:
        result.set(beamT.beamRadius, beamS.beamRadius);
        break;
    case FontRadius:
        result.set(beamT.frontRadius, beamS.frontRadius);
        break;
    case HalfAngle:
        result.set(beamT.halfAngle, beamS.halfAngle);
        break;
    }
    return result;
}

Z::PointTS CausticFunction::calculateResonator() const
{
#define CALC_TS_POINT(func) return Z::PointTS(func(_calc->Mt()), func(_calc->Ms()))

    switch (_mode)
    {
    case BeamRadius: CALC_TS_POINT(calculateResonator_beamRadius);
    case FontRadius: CALC_TS_POINT(calculateResonator_frontRadius);
    case HalfAngle: CALC_TS_POINT(calculateResonator_halfAngle);
    }
    return Z::PointTS();

#undef CALC_TS_POINT
}

// TODO:NEXT-VER should be in Calculator as it can be reused by another functions, e.g. BeamParamsAtElems
double CausticFunction::calculateResonator_beamRadius(const Z::Matrix& m) const
{
    double p = 1 - SQR((m.A + m.D) * 0.5);
    if (p <= 0) return NAN;
    double w2 = _wavelenSI * qAbs(m.B) * M_1_PI / sqrt(p);
    if (w2 < 0) return NAN;
    return sqrt(w2);
}

// TODO:NEXT-VER should be in Calculator as it can be reused by another functions, e.g. BeamParamsAtElems
double CausticFunction::calculateResonator_frontRadius(const Z::Matrix& m) const
{
    if (m.D != m.A)
        return 2 * m.B / (m.D - m.A);
    return (m.B < 0) ? -INFINITY : +INFINITY;
}

// TODO:NEXT-VER should be in Calculator as it can be reused by another functions, e.g. BeamParamsAtElems
double CausticFunction::calculateResonator_halfAngle(const Z::Matrix& m) const
{
    double p = 4.0 - SQR(m.A + m.D);
    if (p <= 0) return NAN;
    return sqrt(_wavelenSI * M_1_PI * 2.0 * qAbs(m.C) / sqrt(p));
}

QString CausticFunction::calculatePoint(const double& arg)
{
    Q_UNUSED(arg)
    return QString();

/* TODO:NEXT-VER
    if (!_elem || !_calc || !_range.has(arg)) return QString();

    _elem->setSubRange(arg);
    _calc->multMatrix();

    Z::PointTS value;
    switch (_schema->tripType())
    {
    case TripType::SW: break; // TODO
    case TripType::RR: break; // TODO
    case TripType::SP: value = calculateSinglePass(); break;
    }

    return QString::fromUtf8("ω<sub>T</sub>: %1; ω<sub>S</sub>: %2")
            .arg(Z::format(value.T), Z::format(value.S)); */
}

void CausticFunction::convertFromSiToModeUnits(Z::PointTS& point) const
{
    switch (_mode)
    {
    case BeamRadius:
        point.T = _beamsizeUnit->fromSi(point.T);
        point.S = _beamsizeUnit->fromSi(point.S);
        break;

    case FontRadius:
        point.T = _curvatureUnit->fromSi(point.T);
        point.S = _curvatureUnit->fromSi(point.S);
        break;

    case HalfAngle:
        point.T = _angleUnit->fromSi(point.T);
        point.S = _angleUnit->fromSi(point.S);
        break;
    }
}

