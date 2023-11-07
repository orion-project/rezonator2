#include "StabilityMapFunction.h"

#include "RoundTripCalculator.h"
#include "../app/CustomPrefs.h"

void StabilityMapFunction::calculate()
{
    if (!checkArguments()) return;

    auto elem = arg()->element;
    auto param = arg()->parameter;

    ElementEventsLocker elemLock(elem);
    Z::ParamValueBackup paramLock(param);

    auto range = arg()->range.plottingRange();
    if (!prepareResults(range)) return;
    if (!prepareCalculator(elem)) return;
    _calc->setStabilityCalcMode(stabilityCalcMode());

    for (auto x : range.values())
    {
        auto value = Z::Value(x, range.unit());

        param->setValue(value);
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

Z::PointTS StabilityMapFunction::calculateAt(const Z::Value& v)
{
    auto elem = arg()->element;
    auto param = arg()->parameter;
    ElementEventsLocker elemLock(elem);
    Z::ParamValueBackup paramLock(param);
    param->setValue(v);
    _calc->multMatrix();
    return _calc->stability();
}
