#include "Format.h"
#include "Variable.h"

#include <QSettings>

namespace Z {

void Variable::calculate(int& points, double &step)
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
}

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
