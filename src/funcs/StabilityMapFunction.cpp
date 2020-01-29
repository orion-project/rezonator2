#include "StabilityMapFunction.h"

#include "../CustomPrefs.h"
#include "../core/Protocol.h"
#include "RoundTripCalculator.h"

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

    for (auto x : range.values())
    {
        auto value = Z::Value(x, range.unit());

        param->setValue(value);
        elem->calcMatrix("StabilityMapFunction::calculate");
        _calc->multMatrix();

        addResultPoint(x, _calc->stability());
    }

    finishResults();
}

void StabilityMapFunction::loadPrefs()
{
    _stabilityCalcMode = Z::Enums::fromStr(
                CustomPrefs::recentStr(QStringLiteral("func_stab_map_mode")),
                Z::Enums::StabilityCalcMode::Normal);
}
