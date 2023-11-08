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
        const double epsX = 1e-7;
        const double infR = 1/epsX;
        const int maxCount = 1000;
    #define SOLVE_WAIST(TS, Mts) \
        if (qAbs(startR.TS) >= infR) { \
            waistW.TS = startW.TS, waistX.TS = startX; \
        } else if (qAbs(stopR.TS) >= infR) { \
            waistW.TS = stopW.TS, waistX.TS = stopX; \
        } else if (startR.TS * stopR.TS < 0) { \
            int count = 0; \
            double x1 = startX, x2 = stopX, x0 = (x1 + x2) / 2.0, r1 = startR.TS, r0; \
            while (qAbs(x2-x1) > epsX and count < maxCount) { \
                elem->setSubRangeSI(x0); _calc->multMatrix(); \
                r0 = isResonator \
                    ? _beamCalc->frontRadius(_calc->Mts(), _ior) \
                    : _pumpCalc->calcT(_calc->Mts(), _ior).frontRadius; \
                if (r1 * r0 < 0) x2 = x0; else x1 = x0, r1 = r0; \
                x0 = (x1 + x2) / 2.0; count++; \
            } \
            if (count == maxCount) \
                qWarning() << "CausticFunction::calculateSpecPoints: failed to solve waist after" << count << "iterations"; \
            else { \
                waistX.TS = x0; \
                waistW.TS = isResonator \
                    ? _beamCalc->beamRadius(_calc->Mts(), _ior) \
                    : _pumpCalc->calcT(_calc->Mts(), _ior).beamRadius; \
            } \
        } \
        if (waistW.TS > 0) { \
            GaussCalculator gauss; \
            gauss.setMI(isResonator ? 1 : _pumpCalc->MI().TS); \
            gauss.setLambda(schema()->wavelenSi() / _ior); \
            gauss.setLock(GaussCalculator::Lock::Waist); \
            gauss.setW0(waistW.TS); \
            gauss.calc(); \
            waistZ0.TS = gauss.z0(); \
            waistVs.TS = gauss.Vs(); \
        }
        SOLVE_WAIST(T, Mt)
        SOLVE_WAIST(S, Ms)
    }

    Z::Unit unitX = params.value(spUnitX).unit();
    Z::Unit unitW = params.value(spUnitW).unit();
    Z::Unit unitR = params.value(spUnitR).unit();
    double offset = params.value(spOffset).value();
    #define FMT_SI(v, u) Z::Value::fromSi(v, u).displayStr()
    QString stopPos = FMT_SI(stopX, unitX);
    QString startOffset, stopOffset;
    if (offset > 0)
    {
        startOffset = " (" % FMT_SI(offset, unitX) % ')';
        stopOffset = " (" % FMT_SI(offset + stopX, unitX) % ')';
    }
    QString report;
    QTextStream stream(&report);
    QChar beamsizeName = isGauss ? 'w' : 'y';
    #define REPORT_TS(TS) \
        stream << "<p><span class='plane'>" << #TS << ":</span><br>" \
               << "<span class='param'>" << beamsizeName << ":</span> " << FMT_SI(startW.TS, unitW) \
               << " <span class='position'>@ 0" << startOffset << "</span><br>"; \
        if (isGauss) { \
            stream << "<span class='param'>R:</span> " << FMT_SI(startR.TS, unitR) \
                   << " <span class='position'>@ 0" << startOffset << "</span><br>"; \
        } else { \
            stream << "<span class='param'>V:</span> " << Z::format(startR.TS) \
                   << " (" << Z::format(startR.TS * 180 / M_PI) << "&deg;)<br>"; \
        } \
        if (waistW.TS > 0) { \
            stream << "<span class='waist'>w<sub>0</sub> = " << FMT_SI(waistW.TS, unitW) \
                   << " @ " << FMT_SI(waistX.TS, unitX); \
            if (offset > 0) \
                stream << " (" << FMT_SI(waistX.TS + offset, unitX) << ')'; \
            stream << "</span><br>" \
                   << "<span class='waist'>Z<sub>0</sub> = " << FMT_SI(waistZ0.TS, unitX) << "</span><br>" \
                   << "<span class='waist'>V<sub>s</sub> = " << Z::format(waistVs.TS) \
                   << " (" << Z::format(waistVs.TS * 180 / M_PI) << "&deg;)</span><br>"; \
        } \
        stream << "<span class='param'>" << beamsizeName << ":</span> " << FMT_SI(stopW.TS, unitW) \
               << " <span class='position'>@ " << stopPos << stopOffset << "</span>"; \
        if (isGauss) { \
            stream << "<br><span class='param'>R:</span> " << FMT_SI(stopR.TS, unitR) \
                   << " <span class='position'>@ " << stopPos << stopOffset << "</span>"; \
        } // for ray vectors start and stop angles are the same, no reason to show twice
    REPORT_TS(T)
    REPORT_TS(S)
    if (AppSettings::instance().isDevMode)
        // Ori::Gui::applyTextBrowserStyleSheet should be called on the target browser
        stream << "<p><a href='do://edit-css'>Edit styles</a>";
    #undef SOLVE_WAIST
    #undef REPORT_TS
    #undef FMT_SI
    return report;
}
