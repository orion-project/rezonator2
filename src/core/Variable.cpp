#include "Format.h"
#include "Variable.h"

#include <QSettings>

namespace Z {

QString PlottingRange::str() const
{
    return QString("start=%1%6, stop=%2%6, range=%3%6, step=%4%6, points=%5%6")
            .arg(_start).arg(_stop).arg(_range).arg(_step).arg(_points).arg(_unit->alias());
}

PlottingRange VariableRange::plottingRange() const
{
    Q_ASSERT(start.unit()->siUnit() == stop.unit()->siUnit());
    Q_ASSERT(start.unit()->siUnit() == step.unit()->siUnit());
    Q_ASSERT(step.unit()->siUnit() == stop.unit()->siUnit());

    PlottingRange res;
    res._start = start.toSi();
    res._stop = stop.toSi();
    res._range = res.stop() - res.start();
    res._unit = start.unit()->siUnit();
    Q_ASSERT(res.range() > 0);
    if (useStep)
    {
        // int((10.0 - 0.0) / 1.0) + 1 = 11 points
        // int((10.0 - 0.0) / 3.0) + 1 = 4 points: 0 3 6 9 10
        res._step = qMin(step.toSi(), res.range());
        Q_ASSERT(res.step() > 0);
        res._points = res.range() / res.step();
    }
    else
    {
        Q_ASSERT(points > 1);
        // (10 - 0) / (10 - 1) ~ 1.1
        res._points = points;
        res._step = res.range() / double(points - 1);
    }
    return res;
}

/*REMOVE void Variable::calculate(int& points, double &step)
{
//    auto range = stop - start;
//    if (useStep)
//    {
//        // int((10.0 - 0.0) / 1.0) + 1 = 11 points
//        // int((10.0 - 0.0) / 3.0) + 1 = 4 points: 0 3 6 9 10
//        step = (range > 0 && this->step > 0)? this->step: 0;
//        if (step > range) step = range;
//        points = (step > 0)? int(range / step) + 1: 0;
//    }
//    else
//    {
//        // (10 - 0) / (10 - 1) ~ 1.1
//        points = (range > 0 && this->points > 1)? this->points: 0;
//        step = (points > 1)? range / double(points - 1): 0;
//    }
}*/

//void Variable::load(QSettings *settings)
//{
//    start = settings->value("Start").toDouble();
//    stop = settings->value("Stop").toDouble();
//    step = settings->value("Step").toDouble();
//    points = settings->value("Points").toInt();
//    useStep = settings->value("UseStep").toBool();
//}

//void Variable::save(QSettings *settings)
//{
//    settings->setValue("Start", start);
//    settings->setValue("Stop", stop);
//    settings->setValue("Step", step);
//    settings->setValue("Points", points);
//    settings->setValue("UseStep", useStep);
//}

//QString Variable::str() const
//{
//    return QString("element: %1; param: %2; start: %3; stop: %4; step: %5; points: %6; useStep: %7")
//        .arg(element).arg(param).arg(Z::str(start)).arg(Z::str(stop)).arg(Z::str(step)).arg(points).arg(useStep);
//}

} // namespace Z
