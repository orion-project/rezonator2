#include "StabilityMapFunction.h"
#include "../funcs/Calculator.h"

void StabilityMapFunction::calculate()
{
    if (!checkArguments()) return;

    auto elem = arg()->element;
    auto param = arg()->parameter;
    BackupAndLock locker(elem, param);

    auto range = arg()->range.plottingRange();
    if (!prepareResults(range)) return;
    if (!prepareCalculator(elem)) return;

    int index = 0;
    double x = range.start();
    while (index < range.points())
    {
        auto value = Z::Value(x, range.unit());

        /*TODO auto error = param->verify(value);
        if (error)
        {
            setError(qApp->translate("Calc error", "Failed to assign %1 = %2%3: %4")
                    .arg(param.name).arg(value).arg(param.unitStr())
                    .arg(qApp->translate("Param error", error)));
            break;
        }*/

        param->setValue(value);
        _calc->multMatrix();

        _x_t[index] = x, _y_t[index] = _calc->stabilityT();
        _x_s[index] = x, _y_s[index] = _calc->stabilityS();

        index++;
        x = qMin(x + range.step(), range.stop());
    }
}

QString StabilityMapFunction::calculateNotables()
{
    return QString();
}
