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

    _ior = elem->ior();

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

    auto calcBeamParams = isResonator
            ? &CausticFunction::calculateResonator
            : &CausticFunction::calculateSinglePass;

    // Calculate round-trip matrix and check if the caustic can be calculated
    elem->setSubRangeSI(range.values().first());
    _calc->multMatrix();
    if (isResonator) // Can't be calculated for unstable resonator
    {
        auto stab = _calc->isStable();
        if (not stab.T and not stab.S)
        {
            setError(qApp->translate("Calc error", "System is unstable, can't calculate caustic"));
            return;
        }
    }
    // Caustic can't be calculated for SP-system with geometric pump and complex matrices
    else if (Pumps::isGeometric(_pump) and (not _calc->Mt().isReal() or not _calc->Ms().isReal()))
    {
        setError(qApp->translate("Calc error", "Geometric pump can't be used with complex matrices"));
        return;
    }

    Z::PointTS prevRes(Double::nan(), Double::nan());
    for (auto x : range.values())
    {
        elem->setSubRangeSI(x);
        _calc->multMatrix();

        if (_writeProtocol)
        {
            Z_INFO("Offset" << x)
            Z_INFO("Mt =" << _calc->Mt().str() << "| Ms =" << _calc->Ms().str())
        }

        Z::PointTS res = (this->*calcBeamParams)();

        if (_mode == FrontRadius)
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
    QString res = FunctionUtils::preparePumpCalculator(schema(), _pump, _pumpCalc, _writeProtocol);
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
    _beamCalc.reset(new AbcdBeamCalculator(schema()->wavelenSi()));
    return true;
}

Z::PointTS CausticFunction::calculateSinglePass() const
{
    BeamResult beamT = _pumpCalc.T->calc(_calc->Mt(), _ior);
    BeamResult beamS = _pumpCalc.S->calc(_calc->Ms(), _ior);
    switch (_mode)
    {
    case BeamRadius: return { beamT.beamRadius, beamS.beamRadius };
    case FrontRadius: return { beamT.frontRadius, beamS.frontRadius };
    case HalfAngle: return { beamT.halfAngle, beamS.halfAngle };
    }
    qCritical() << "Unsupported caustic result mode";
    return { Double::nan(), Double::nan() };
}

Z::PointTS CausticFunction::calculateResonator() const
{
    switch (_mode)
    {
    case BeamRadius: return _beamCalc->beamRadius(_calc->Mt(), _calc->Ms(), _ior);
    case FrontRadius: return _beamCalc->frontRadius(_calc->Mt(), _calc->Ms(), _ior);
    case HalfAngle: return _beamCalc->halfAngle(_calc->Mt(), _calc->Ms(), _ior);
    }
    qCritical() << "Unsupported caustic result mode";
    return Z::PointTS();
}

Z::PointTS CausticFunction::calculateAt(const Z::Value &arg)
{
    double argSI = arg.toSi();
    auto elem = Z::Utils::asRange(this->arg()->element);
    double x = qMin(qMax(argSI, 0.0), elem->axisLengthSI());
    auto calcBeamParams = _schema->isResonator()
            ? &CausticFunction::calculateResonator
            : &CausticFunction::calculateSinglePass;
    elem->setSubRangeSI(x);
    _calc->multMatrix();
    return (this->*calcBeamParams)();
}

QString CausticFunction::modeAlias(Mode mode)
{
    switch (mode)
    {
    case CausticFunction::Mode::BeamRadius:
        return QStringLiteral("W");
    case CausticFunction::Mode::FrontRadius:
        return QStringLiteral("R");
    case CausticFunction::Mode::HalfAngle:
        return QStringLiteral("V");
    }
    return QString();
}

QString CausticFunction::modeDisplayName(Mode mode)
{
    switch (mode)
    {
    case CausticFunction::Mode::BeamRadius:
        return qApp->tr("Beam radius");
    case CausticFunction::Mode::FrontRadius:
        return qApp->tr("Wavefront curvature radius");
    case CausticFunction::Mode::HalfAngle:
        return qApp->tr("Half of divergence angle");
    }
    return QString();
}
