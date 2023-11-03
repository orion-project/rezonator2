#include "CausticFunction.h"

#include "AbcdBeamCalculator.h"
#include "FunctionUtils.h"
#include "PumpCalculator.h"
#include "RoundTripCalculator.h"
#include "../core/Protocol.h"
#include "../core/Schema.h"

#include <QApplication>
#include <QDebug>

Z::VariableRange CausticFunction::givenRange()
{
    auto range = arg()->range;
    auto elem = Z::Utils::asRange(arg()->element);
    range.stop = Z::Value(elem->axisLengthSI(), Z::Units::m());
    return range;
}

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

    auto range = givenRange().plottingRange();
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
    if (!_pump)
        _pump = schema()->activePump();
    if (!_pump)
    {
        setError(qApp->translate("Calc error",
            "There is no active pump in the schema. "
            "Use 'Pumps' window to create a new pump or activate one of the existing ones."));
        return false;
    }
    _pumpCalc.reset(new PumpCalculator(_pump, schema()->wavelenSi(), _writeProtocol));
    FunctionUtils::prepareDynamicElements(schema(), ref, _pumpCalc.get());
    return true;
}

bool CausticFunction::prepareResonator()
{
    _beamCalc.reset(new AbcdBeamCalculator(schema()->wavelenSi()));
    return true;
}

Z::PointTS CausticFunction::calculateSinglePass() const
{
    switch (_mode)
    {
    case BeamRadius: return _pumpCalc->beamRadius(_calc->Mt(), _calc->Ms(), _ior);
    case FrontRadius: return _pumpCalc->frontRadius(_calc->Mt(), _calc->Ms(), _ior);
    case HalfAngle: return _pumpCalc->halfAngle(_calc->Mt(), _calc->Ms(), _ior);
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
    elem->setSubRangeSI(x);
    _calc->multMatrix();
    if (_schema->isResonator())
        return calculateResonator();
    return calculateSinglePass();
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

QString CausticFunction::calculateNotables(Z::Unit unitX, Z::Unit unitY)
{
//    if (!ok()) return QString();

//    auto range = givenRange();
//    auto elem = Z::Utils::asRange(arg()->element);

//    elem->setSubRangeSI(range.start.toSi());
//    _calc->multMatrix();

//    Z::PointTS leftW, rightW, leftR, rightR;

//    if (_schema->isResonator())
//    {
//        leftW = _beamCalc->beamRadius(_calc->Mt(), _calc->Ms(), _ior);
//        leftR = _beamCalc->frontRadius(_calc->Mt(), _calc->Ms(), _ior);
//    }
//    else
//    {
//        BeamResult beamT = _pumpCalc.T->calc(_calc->Mt(), _ior);
//        BeamResult beamS = _pumpCalc.S->calc(_calc->Ms(), _ior);
//        leftW = { beamT.beamRadius, beamS.beamRadius };
//        leftR = { beamT.frontRadius, beamS.frontRadius };
//    }

//    elem->setSubRange(range.stop.toSi());
//    _calc->multMatrix();

//    if (_schema->isResonator())
//    {
//        rightW = _beamCalc->beamRadius(_calc->Mt(), _calc->Ms(), _ior);
//        rightR = _beamCalc->frontRadius(_calc->Mt(), _calc->Ms(), _ior);
//    }
//    else
//    {
//        BeamResult beamT = _pumpCalc.T->calc(_calc->Mt(), _ior);
//        BeamResult beamS = _pumpCalc.S->calc(_calc->Ms(), _ior);
//        rightW = { beamT.beamRadius, beamS.beamRadius };
//        rightR = { beamT.frontRadius, beamS.frontRadius };
//    }

//    if (_schema->isResonator() || _pumpCalc.isGauss())
//    {

//    }
    return QString();
}
