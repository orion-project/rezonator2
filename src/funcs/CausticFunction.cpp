#include "CausticFunction.h"

#include "AbcdBeamCalculator.h"
#include "FunctionUtils.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../core/Protocol.h"
#include "../core/Schema.h"

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
            : prepareSinglePass(elem);
    if (!isPrepared) return;

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

bool CausticFunction::prepareSinglePass(Element* ref)
{
    QString res = FunctionUtils::preparePumpCalculator(schema(), _pump, _pumpCalc);
    if (!res.isEmpty())
    {
        setError(res);
        return false;
    }

    FunctionUtils::prepareDynamicElements(schema(), ref, _pumpCalc);
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
