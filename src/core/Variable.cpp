#include "Format.h"
#include "Variable.h"
#include "Schema.h"

#include <QSettings>

namespace Z {

QString PlottingRange::str() const
{
    return QString("start=%1%6, stop=%2%6, range=%3%6, step=%4%6, points=%5")
            .arg(_start).arg(_stop).arg(_range).arg(_step).arg(_points).arg(_unit->alias());
}

//------------------------------------------------------------------------------

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

    // Calc all point values
    double x = res.start();
    auto values = QVector<double>(res.points());
    for (int i = 0; i < res.points(); i++)
    {
        values[i] = x;
        x += res.step();
    }
    values[res.points()-1] = res.stop();
    res._values = values;

    return res;
}

QString VariableRange::str() const
{
    return QString("start: %1; stop: %2; step: %3; points: %4; useStep: %5")
        .arg(start.str()).arg(stop.str()).arg(step.str()).arg(points).arg(Z::str(useStep));
}

//------------------------------------------------------------------------------

void Variable::load(QSettings *settings, Schema* schema)
{
    element = schema->elementByLabel(settings->value("element").toString());
    parameter = element ? element->params().byAlias(settings->value("parameter").toString()) : nullptr;
    // TODO extract validation logic from variable editor dialog into helper method and use it here
    // to verify that restored range is valid and suitable for restored element and its parameter.
    range.start.fromStoredStr(settings->value("start").toString());
    range.stop.fromStoredStr(settings->value("stop").toString());
    range.step.fromStoredStr(settings->value("step").toString());
    range.points = settings->value("points").toInt();
    range.useStep = settings->value("useStep").toBool();
}

void Variable::save(QSettings *settings)
{
    settings->setValue("element", element ? element->label() : QString());
    settings->setValue("parameter", parameter ? parameter->alias() : QString());
    settings->setValue("start", range.start.toStoredStr());
    settings->setValue("stop", range.stop.toStoredStr());
    settings->setValue("step", range.step.toStoredStr());
    settings->setValue("points", range.points);
    settings->setValue("useStep", range.useStep);
}

QString Variable::str() const
{
    return QString("element: %1; param: %2; %3")
            .arg(element ? element->displayLabel() : QString("(null)"))
            .arg(parameter ? parameter->alias() : QString("(null)"))
            .arg(range.str());
}

} // namespace Z
