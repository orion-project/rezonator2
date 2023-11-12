#include "CausticFunction.h"

#include "../app/AppSettings.h"
#include "../core/Protocol.h"
#include "../core/Schema.h"
#include "../math/AbcdBeamCalculator.h"
#include "../math/FunctionUtils.h"
#include "../math/GaussCalculator.h"
#include "../math/PumpCalculator.h"
#include "../math/RoundTripCalculator.h"

#include <QApplication>
#include <QDebug>

Z::VariableRange CausticFunction::givenRange()
{
    auto range = arg()->range;
    auto elem = Z::Utils::asRange(arg()->element);
    range.stop = Z::Value(elem->axisLengthSI(), Z::Units::m());
    return range;
}

void CausticFunction::calculate(CalculationMode calcMode)
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

    if (calcMode != CALC_PLOT) return;

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

QString CausticFunction::valueSymbol() const
{
    switch (_mode)
    {
    case CausticFunction::Mode::BeamRadius:
        return beamsizeSymbol();
    case CausticFunction::Mode::FrontRadius:
        return QStringLiteral("R");
    case CausticFunction::Mode::HalfAngle:
        return QStringLiteral("V");
    }
    return QString();
}

QString CausticFunction::beamsizeSymbol() const
{
    return (_schema->isResonator() or _pumpCalc->isGauss()) ? QStringLiteral("w") : QStringLiteral("y");
}

QString CausticFunction::calculateSpecPoints(const SpecPointParams &params)
{
    if (!ok()) return QString();

    bool isResonator = _schema->isResonator();
    bool isGauss = isResonator or _pumpCalc->isGauss();
    auto elem = Z::Utils::asRange(arg()->element);
    auto range = givenRange();
    double startX = range.start.toSi();
    double stopX = range.stop.toSi();
    Z::PointTS startW, startR, stopW, stopR;

    elem->setSubRangeSI(startX);
    _calc->multMatrix();
    if (isResonator)
    {
        startW = _beamCalc->beamRadius(_calc->Mt(), _calc->Ms(), _ior);
        startR = _beamCalc->frontRadius(_calc->Mt(), _calc->Ms(), _ior);
    }
    else
    {
        BeamResult beamT = _pumpCalc->calcT(_calc->Mt(), _ior);
        BeamResult beamS = _pumpCalc->calcS(_calc->Ms(), _ior);
        startW = { beamT.beamRadius, beamS.beamRadius };
        startR = { isGauss ? beamT.frontRadius : beamT.halfAngle,
                   isGauss ? beamS.frontRadius : beamS.halfAngle };
    }

    elem->setSubRangeSI(stopX);
    _calc->multMatrix();
    if (isResonator)
    {
        stopW = _beamCalc->beamRadius(_calc->Mt(), _calc->Ms(), _ior);
        stopR = _beamCalc->frontRadius(_calc->Mt(), _calc->Ms(), _ior);
    }
    else
    {
        BeamResult beamT = _pumpCalc->calcT(_calc->Mt(), _ior);
        BeamResult beamS = _pumpCalc->calcS(_calc->Ms(), _ior);
        stopW = { beamT.beamRadius, beamS.beamRadius };
        stopR = { isGauss ? beamT.frontRadius : beamT.halfAngle,
                  isGauss ? beamS.frontRadius : beamS.halfAngle };
    }

    Z::PointTS waistW(0, 0), waistX, waistZ0, waistVs;
    if (isGauss)
    {
        auto solve = [&](Z::WorkPlane ts){
            const double epsX = 1e-7;
            const double infR = 1/epsX;
            const int maxIters = 100;
            if (qAbs(startR[ts]) >= infR) {
                waistW[ts] = startW[ts], waistX[ts] = startX;
                return;
            }
            if (qAbs(stopR[ts]) >= infR) {
                waistW[ts] = stopW[ts], waistX[ts] = stopX;
                return;
            }
            if (startR[ts] * stopR[ts] > 0) {
                return;
            }
            int iter = 0;
            double x1 = startX, r1 = startR[ts];
            double x2 = stopX;
            double x0 = (x1 + x2) / 2.0, r0;
            while (qAbs(x2-x1) > epsX and iter < maxIters) {
                elem->setSubRangeSI(x0);
                _calc->multMatrix();
                r0 = isResonator \
                      ? _beamCalc->frontRadius(_calc->M(ts), _ior)
                      : _pumpCalc->calcT(_calc->M(ts), _ior).frontRadius;
                if (r1 * r0 < 0) x2 = x0;
                else x1 = x0, r1 = r0;
                x0 = (x1 + x2) / 2.0;
                iter++;
            }
            if (iter == maxIters) {
                qWarning() << "CausticFunction::calculateSpecPoints: "
                           << "failed to solve waist after" << iter << "iterations";
                return;
            }
            waistX[ts] = x0;
            waistW[ts] = isResonator
                  ? _beamCalc->beamRadius(_calc->M(ts), _ior)
                  : _pumpCalc->calcT(_calc->M(ts), _ior).beamRadius;
            GaussCalculator gauss;
            gauss.setMI(isResonator ? 1 : _pumpCalc->MI()[ts]);
            gauss.setLambda(schema()->wavelenSi() / _ior);
            gauss.setLock(GaussCalculator::Lock::Waist);
            gauss.setW0(waistW[ts]);
            gauss.calc();
            waistZ0[ts] = gauss.z0();
            waistVs[ts] = gauss.Vs();
        };

        solve(Z::T);
        solve(Z::S);
    }

    auto fmtSi = [](Z::ValueSi v, Z::Unit u){ return Z::Value::fromSi(v, u).displayStr(); };

    Z::Unit unitX = params.value(spUnitX).unit();
    Z::Unit unitW = params.value(spUnitW).unit();
    Z::Unit unitR = params.value(spUnitR).unit();
    double offset = params.value(spOffset).value();
    QString stopPos = fmtSi(stopX, unitX);
    QString startOffset, stopOffset;
    if (offset > 0)
    {
        startOffset = " (" % fmtSi(offset, unitX) % ')';
        stopOffset = " (" % fmtSi(offset + stopX, unitX) % ')';
    }
    QString report;
    QTextStream stream(&report);
    QString beamsizeName = beamsizeSymbol();

    auto reportTS = [&](Z::WorkPlane ts) {
        stream << "<p><span class='plane'>" << Z::planeName(ts) << ":</span><br>"
               << "<span class='param'>" << beamsizeName << ":</span> " << fmtSi(startW[ts], unitW)
               << " <span class='position'>@ 0" << startOffset << "</span><br>";
        if (isGauss)
            stream << "<span class='param'>R:</span> " << fmtSi(startR[ts], unitR)
                   << " <span class='position'>@ 0" << startOffset << "</span><br>";
        else stream << "<span class='param'>V:</span> " << Z::format(startR[ts])
                    << " (" << Z::format(startR[ts] * 180 / M_PI) << "&deg;)<br>";

        if (waistW[ts] > 0) {
            stream << "<span class='waist'>w<sub>0</sub> = " << fmtSi(waistW[ts], unitW)
                   << " @ " << fmtSi(waistX[ts], unitX);
            if (offset > 0)
                stream << " (" << fmtSi(waistX[ts] + offset, unitX) << ')';
            stream << "</span><br>"
                   << "<span class='waist'>Z<sub>0</sub> = " << fmtSi(waistZ0[ts], unitX) << "</span><br>"
                   << "<span class='waist'>V<sub>s</sub> = " << Z::format(waistVs[ts])
                   << " (" << Z::format(waistVs[ts] * 180 / M_PI) << "&deg;)</span><br>";
        }
        stream << "<span class='param'>" << beamsizeName << ":</span> " << fmtSi(stopW[ts], unitW)
               << " <span class='position'>@ " << stopPos << stopOffset << "</span>";
        if (isGauss)
            stream << "<br><span class='param'>R:</span> " << fmtSi(stopR[ts], unitR)
                   << " <span class='position'>@ " << stopPos << stopOffset << "</span>";
        // for ray vectors start and stop angles are the same, no reason to show twice
    };

    reportTS(Z::T);
    reportTS(Z::S);
    if (AppSettings::instance().isDevMode)
        // Ori::Gui::applyTextBrowserStyleSheet should be called on the target browser
        stream << "<p><a href='do://edit-css'>Edit styles</a>";
    return report;
}
