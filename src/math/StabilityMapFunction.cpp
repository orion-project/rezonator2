#include "StabilityMapFunction.h"

#include "../app/AppSettings.h"
#include "../app/PersistentState.h"
#include "../math/RoundTripCalculator.h"

#include <QApplication>

#ifdef Q_OS_LINUX
#include <optional>
#endif

void StabilityMapFunction::calculate(CalculationMode calcMode)
{
    _approxStabBounds.T.clear();
    _approxStabBounds.S.clear();

    if (!checkArguments()) return;

    auto elem = arg()->element;
    auto param = arg()->parameter;
    ElementEventsLocker elemLock(elem, "StabilityMapFunction::calculate");
    Z::ParamValueBackup paramLock(param, "StabilityMapFunction::calculate");

    _plotRange = arg()->range.plottingRange();
    if (!prepareResults(_plotRange)) return;
    if (!prepareCalculator(elem)) return;
    _calc->setStabilityCalcMode(stabilityCalcMode());

    if (calcMode != CALC_PLOT) return;

    std::optional<double> prevX;
    Z::PairTS<std::optional<ApproxBound>> startBound;

    auto checkStabBound = [&](Z::WorkPlane ts, double x, const Z::PairTS<bool>& isStable) {
        auto wasStable = bool(startBound[ts]);
        if (not wasStable and isStable[ts]) {
            if (prevX)
                startBound[ts] = {*prevX, false};
            else startBound[ts] = {_plotRange.start(), true};
        } else if (wasStable and not isStable[ts]) {
            _approxStabBounds[ts].append({*startBound[ts], {x, false}});
            startBound[ts].reset();
        }
    };

    for (auto x : _plotRange.values())
    {
        param->setValue({x, _plotRange.unit()});
        _calc->multMatrix("StabilityMapFunction::calculate");
        addResultPoint(x, _calc->stability());

        auto isStable = _calc->isStable();
        checkStabBound(Z::T, x, isStable);
        checkStabBound(Z::S, x, isStable);

        prevX = x;
    }
    if (startBound.T) _approxStabBounds.T.append({*startBound.T, {_plotRange.stop(), true}});
    if (startBound.S) _approxStabBounds.S.append({*startBound.S, {_plotRange.stop(), true}});

    finishResults();
}

void StabilityMapFunction::loadPrefs()
{
    _stabilityCalcMode = RecentData::getEnum("func_stab_map_mode", Z::Enums::StabilityCalcMode::Normal);
}

Z::PointTS StabilityMapFunction::calculateAt(const Z::Value& v)
{
    _calc->setStabilityCalcMode(stabilityCalcMode());
    auto elem = arg()->element;
    auto param = arg()->parameter;
    ElementEventsLocker elemLock(elem, "StabilityMapFunction::calculateAt");
    Z::ParamValueBackup paramLock(param, "StabilityMapFunction::calculateAt");
    param->setValue(v);
    _calc->multMatrix("StabilityMapFunction::calculateAt");
    return _calc->stability();
}

QVector<Z::RangeSi> StabilityMapFunction::findStabilityBounds(Z::WorkPlane ts) const
{
    auto elem = arg()->element;
    auto param = arg()->parameter;
    ElementEventsLocker elemLock(elem, "StabilityMapFunction::findStabilityBounds");
    Z::ParamValueBackup paramLock(param, "StabilityMapFunction::findStabilityBounds");
    _calc->setStabilityCalcMode(Z::Enums::StabilityCalcMode::Squared);

    auto solve = [&](double x1, double x2) -> std::optional<double> {
        const double eps = 1e-7;
        const int maxIters = 100;
        param->setValue({x1, _plotRange.unit()});
        _calc->multMatrix("StabilityMapFunction::findStabilityBounds");
        double p1 = _calc->stability(ts);
        int iter = 0;
        double p0, x0 = (x1 + x2) / 2.0;
        while (qAbs(p1) > eps and iter < 100) {
            param->setValue({x0, _plotRange.unit()});
            _calc->multMatrix("StabilityMapFunction::findStabilityBounds");
            p0 = _calc->stability(ts);
            if (p1 * p0 < 0) x2 = x0;
            else x1 = x0, p1 = p0;
            x0 = (x1 + x2) / 2.0, iter++;
        }
        if (iter == maxIters) {
            qWarning() << "StabilityMapFunction::calcStabilityBounds: "
                       << "failed to solve bound after" << iter << "iterations";
            return {};
        }
        return x1;
    };

    QVector<Z::RangeSi> res;
    for (const ApproxRange& approxRange: _approxStabBounds[ts])
    {
        std::optional<double> start, stop;
        if (approxRange.start.exact)
            start = approxRange.start.value;
        else
        {
            double x1 = approxRange.start.value;
            double x2 = x1 + _plotRange.step();
            start = solve(x1, x2);
        }
        if (approxRange.stop.exact)
            stop = approxRange.stop.value;
        else
        {
            double x2 = approxRange.stop.value;
            double x1 = x2 - _plotRange.step();
            stop = solve(x1, x2);
        }
        if (start and stop)
            res.append({ *start, *stop });
    }
    return res;
}

QString StabilityMapFunction::calculateSpecPoints(const SpecPointParams& params)
{
    if (!ok()) return QString();

    QString report;
    QTextStream stream(&report);
    stream << "<p class='title'>" << qApp->tr("Stability regions");

    Z::Unit unitX = params.value(spUnitX).unit();

    auto stabBounds = Z::PairTS(findStabilityBounds(Z::T), findStabilityBounds(Z::S));

    auto fmtSi = [unitX](Z::ValueSi v) { return Z::Value::fromSi(v, unitX).displayStr(); };

    auto reportTS = [&](Z::WorkPlane ts) {
        stream << "<p><span class='plane'>" << Z::planeName(ts) << ":</span><br>";
        if (stabBounds[ts].isEmpty()) {
            stream << "<span class='error'>"
                   << qApp->tr("Stability regions not found in variation range") << "</span>";
            return;
        }
        int index = 1;
        foreach (auto bounds, stabBounds[ts]) {
            stream << "<span class='position'>" << index << "-beg:</span> " << fmtSi(bounds.start) << "<br>"
                   << "<span class='position'>" << index << "-end:</span> " << fmtSi(bounds.stop) << "<br><br>";
            index++;
        }
    };

    reportTS(Z::T);
    reportTS(Z::S);
    if (AppSettings::instance().isDevMode)
        // Ori::Gui::applyTextBrowserStyleSheet should be called on the target browser
        stream << "<p><a href='do://edit-css'>Edit styles</a>";
    return report;
}
