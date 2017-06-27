#include "StabilityMapFunction.h"

void StabilityMapFunction::calculate()
{
    /* TODO auto elem = argElement();
    if (!elem) return;

    auto param = elem->params().at(_arg.param); // TODO param as pointer with checking (like argElement)

    double backupValue = param.getValue(param.unit);
    elem->lock(); // prevent multiple elementChanged events

    int points;
    double step;
    if (!prepareResults(points, step)) return;
    if (!prepareCalculator(elem)) return;

    int index = 0;
    double value = _arg.start;
    while (index < points)
    {
        auto error = param.setValue(value, param.unit);
        if (error)
        {
            setError(qApp->translate("Calc error", "Failed to assign %1 = %2%3: %4")
                    .arg(param.name).arg(value).arg(param.unitStr())
                    .arg(qApp->translate("Param error", error)));
            break;
        }
        _calc->multMatrix();

        _x_t[index] = value, _y_t[index] = _calc->stabilityT();
        _x_s[index] = value, _y_s[index] = _calc->stabilityS();

        index++;
        value += step;
        if (value > _arg.stop)
            value = _arg.stop;
    }
    param.setValue(backupValue, param.unit); // restore original value
    elem->unlock();*/
}

QString StabilityMapFunction::calculateNotables()
{
    return QString();
}
