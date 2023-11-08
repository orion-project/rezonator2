#include "StabilityMapFunction.h"

#include "../app/AppSettings.h"
#include "../app/CustomPrefs.h"
#include "../math/RoundTripCalculator.h"

#include <QApplication>

void StabilityMapFunction::calculate()
{
    _approxStabBounds.T.clear();
    _approxStabBounds.S.clear();

    if (!checkArguments()) return;

    auto elem = arg()->element;
    auto param = arg()->parameter;
    ElementEventsLocker elemLock(elem);
    Z::ParamValueBackup paramLock(param);

    _plotRange = arg()->range.plottingRange();
    if (!prepareResults(_plotRange)) return;
    if (!prepareCalculator(elem)) return;
    _calc->setStabilityCalcMode(stabilityCalcMode());

    std::optional<double> prevX;
    Z::PairTS<std::optional<ApproxBound>> startBound;

    auto checkStabBound = [&](Z::WorkPlane ts, double x, const Z::PairTS<bool>& isStable) {
        bool wasStable = startBound[ts].has_value();
        if (not wasStable and isStable[ts]) {
            if (prevX.has_value())
                startBound[ts] = {prevX.value(), false};
            else startBound[ts] = {_plotRange.start(), true};
        } else if (wasStable and not isStable[ts]) {
            _approxStabBounds[ts].append({startBound[ts].value(), {x, false}});
            startBound[ts].reset();
        }
    };

    for (auto x : _plotRange.values())
    {
        param->setValue({x, _plotRange.unit()});
        _calc->multMatrix();
        addResultPoint(x, _calc->stability());

        auto isStable = _calc->isStable();
        checkStabBound(Z::Plane_T, x, isStable);
        checkStabBound(Z::Plane_S, x, isStable);

        prevX = x;
    }
    if (startBound.T.has_value())
        _approxStabBounds.T.append({startBound.T.value(), {_plotRange.stop(), true}});
    if (startBound.S.has_value())
        _approxStabBounds.S.append({startBound.S.value(), {_plotRange.stop(), true}});

    finishResults();
}

void StabilityMapFunction::loadPrefs()
{
    _stabilityCalcMode = Z::Enums::fromStr(
                CustomPrefs::recentStr(QStringLiteral("func_stab_map_mode")),
                Z::Enums::StabilityCalcMode::Normal);
}

Z::PointTS StabilityMapFunction::calculateAt(const Z::Value& v)
{
    _calc->setStabilityCalcMode(stabilityCalcMode());
    auto elem = arg()->element;
    auto param = arg()->parameter;
    ElementEventsLocker elemLock(elem);
    Z::ParamValueBackup paramLock(param);
    param->setValue(v);
    _calc->multMatrix();
    return _calc->stability();
}

QVector<Z::RangeSi> StabilityMapFunction::calcStabilityBounds(Z::WorkPlane ts) const
{
    auto elem = arg()->element;
    auto param = arg()->parameter;
    ElementEventsLocker elemLock(elem);
    Z::ParamValueBackup paramLock(param);
    _calc->setStabilityCalcMode(Z::Enums::StabilityCalcMode::Squared);

    auto solve = [&](double x1, double x2) -> std::optional<double> {
        const double eps = 1e-7;
        const int maxIters = 100;
        param->setValue({x1, _plotRange.unit()});
        _calc->multMatrix();
        double p1 = _calc->stability(ts);
        int iter = 0;
        double p0, x0 = (x1 + x2) / 2.0;
        while (qAbs(p1) > eps and iter < 100) {
            param->setValue({x0, _plotRange.unit()});
            _calc->multMatrix();
            p0 = _calc->stability(ts);
            if (p1 * p0 < 0) x2 = x0; else x1 = x0, p1 = p0;
            x0 = (x1 + x2) / 2.0, iter++;
        }
        if (iter == maxIters) {
            qWarning() << "StabilityMapFunction::calcStabilityBounds: failed to solve bound after" << iter << "iterations";
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
        if (start.has_value() and stop.has_value())
            res.append({ start.value(), stop.value() });
    }
    return res;
}

QString StabilityMapFunction::calculateSpecPoints(const SpecPointParams& params)
{
    Z::Unit unitX = params.value(spUnitX).unit();
    #define FMT_SI(v) Z::Value::fromSi(v, unitX).displayStr()

    Z::PairTS<QVector<Z::RangeSi>> stabBounds = {
        calcStabilityBounds(Z::Plane_T),
        calcStabilityBounds(Z::Plane_S)
    };

    QString report;
    QTextStream stream(&report);

    auto reportTS = [&](Z::WorkPlane ts){
        stream << "<p><span class='plane'>" << Z::planeName(ts) << ":</span><br>";
        if (stabBounds[ts].isEmpty())
            stream << "<span class='error'>"
                   << qApp->tr("Stability regions not found in variation range") << "</span>";
        else {
            int index = 1;
            for (auto bounds: stabBounds[ts]) {
                stream << "<span class='position'>" << index << "-beg:</span> " << FMT_SI(bounds.start) << "<br>"
                       << "<span class='position'>" << index << "-end:</span> " << FMT_SI(bounds.stop) << "<br><br>";
                index++;
            }
        }
    };

    reportTS(Z::Plane_T);
    reportTS(Z::Plane_S);
    if (AppSettings::instance().isDevMode)
        // Ori::Gui::applyTextBrowserStyleSheet should be called on the target browser
        stream << "<p><a href='do://edit-css'>Edit styles</a>";
    #undef FMT_SI
    return report;
}
