#include "StabilityMapFunction.h"

#include "../CustomPrefs.h"
#include "../core/Protocol.h"
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
    _calc->setStabilityCalcMode(stabilityCalcMode());

    auto targetUnitX = arg()->range.start.unit();

    for (auto x : range.values())
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

        addResultPoint(targetUnitX->fromSi(x), _calc->stability());
    }

    finishResults();
}

void StabilityMapFunction::loadPrefs()
{
    _stabilityCalcMode = Z::Enums::fromStr(
                CustomPrefs::recentStr(QStringLiteral("func_stab_map_mode")),
                Z::Enums::StabilityCalcMode::Normal);
}

