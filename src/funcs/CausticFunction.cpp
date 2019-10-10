#include "CausticFunction.h"

#include "../core/Protocol.h"
#include "../core/Schema.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "AbcdBeamCalculator.h"

#include <QApplication>
#include <QDebug>

void CausticFunction::calculate()
{
    if (!checkArgElem()) return;

    auto elem = Z::Utils::asRange(arg()->element);
    if (!elem)
    {
        setError("CausticFunction.arg.element is not range");
        return;
    }

    _wavelenSI = schema()->wavelength().value().toSi() / elem->ior();

    auto tmpRange = arg()->range;
    tmpRange.stop = Z::Value(elem->axisLengthSI(), Z::Units::m());
    auto range = tmpRange.plottingRange();
    if (!prepareResults(range)) return;
    if (!prepareCalculator(elem, true)) return;

    bool isResonator = _schema->isResonator();
    bool isPrepared = isResonator
            ? prepareResonator()
            : prepareSinglePass();
    if (!isPrepared) return;

    BackupAndLock locker(elem, elem->paramLength());

    bool needCheckStability = isResonator;
    auto calcBeamParams = isResonator
            ? &CausticFunction::calculateResonator
            : &CausticFunction::calculateSinglePass;
    Z::PointTS prevRes(Double::nan(), Double::nan());
    for (auto x : range.values())
    {
        elem->setSubRangeSI(x);
        _calc->multMatrix();

        // After the first round-trip was calculated,
        // we should check if system is unstable
        if (needCheckStability)
        {
            needCheckStability = false;
            auto stab = _calc->isStable();
            if (!stab.T && !stab.S)
            {
                setError(qApp->translate("Calc error", "System is unstable, can't calculate caustic"));
                return;
            }
        }

        Z::PointTS res = (this->*calcBeamParams)();

        if (_mode == FontRadius)
        {
            // If wavefront radius changes its sign, then we have a pole at waist
            if (!std::isnan(prevRes.T) && (prevRes.T * res.T) < 0)
                _results.T.addPoint(x, Double::nan()); // finish previous segment
            if (!std::isnan(prevRes.S) && (prevRes.S * res.S) < 0)
                _results.S.addPoint(x, Double::nan()); // finish previous segment
            prevRes = res;
        }

        addResultPoint(x, res);
    }

    finishResults();
}

bool CausticFunction::prepareSinglePass()
{    
    if (!_pump)
    {
        setError(qApp->translate("Calc error",
            "There is no active pump in the schema. "
            "Use 'Pumps' window to create a new pump or activate one of the existing ones."));
        return false;
    }
    if (!_pumpCalc.T) _pumpCalc.T = PumpCalculator::T();
    if (!_pumpCalc.S) _pumpCalc.S = PumpCalculator::S();

    // NOTE: We have _wavelenSI member here in the function, but it can be lower
    // than schema's wavelength in order to account IOR of the reference element.
    // But pump is supposed to be in air, so get wavelength from schema.
    const double lambda = schema()->wavelength().value().toSi();
    if (!_pumpCalc.T->init(_pump, lambda) ||
        !_pumpCalc.S->init(_pump, lambda))
    {
        setError("Unsupported pump mode");
        return false;
    }
    return true;
}

bool CausticFunction::prepareResonator()
{
    if (!_beamCalc) _beamCalc.reset(new AbcdBeamCalculator);
    _beamCalc->setWavelenSI(_wavelenSI);
    return true;
}

Z::PointTS CausticFunction::calculateSinglePass() const
{
    BeamResult beamT = _pumpCalc.T->calc(_calc->Mt(), _wavelenSI);
    BeamResult beamS = _pumpCalc.S->calc(_calc->Ms(), _wavelenSI);
    switch (_mode)
    {
    case BeamRadius: return { beamT.beamRadius, beamS.beamRadius };
    case FontRadius: return { beamT.frontRadius, beamS.frontRadius };
    case HalfAngle: return { beamT.halfAngle, beamS.halfAngle };
    }
    qCritical() << "Unsupported caustic result mode";
    return { Double::nan(), Double::nan() };
}

Z::PointTS CausticFunction::calculateResonator() const
{
    switch (_mode)
    {
    case BeamRadius: return _beamCalc->beamRadius(_calc->Mt(), _calc->Ms());
    case FontRadius: return _beamCalc->frontRadius(_calc->Mt(), _calc->Ms());
    case HalfAngle: return _beamCalc->halfAngle(_calc->Mt(), _calc->Ms());
    }
    qCritical() << "Unsupported caustic result mode";
    return Z::PointTS();
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

Z::Unit CausticFunction::defaultUnitX() const
{
    return _arg.parameter->value().unit();
}

Z::Unit CausticFunction::defaultUnitsForMode(CausticFunction::Mode mode)
{
    // TODO: Preferable units for each mode can be stored and restored from CustomPrefs
    switch (mode)
    {
    case CausticFunction::BeamRadius: return Z::Units::mkm();
    case CausticFunction::FontRadius: return Z::Units::m();
    case CausticFunction::HalfAngle: return Z::Units::deg();
    }
    return Z::Units::none();
}
