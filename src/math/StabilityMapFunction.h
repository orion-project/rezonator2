#ifndef STABILITY_MAP_FUNCTION_H
#define STABILITY_MAP_FUNCTION_H

#include "../core/CommonTypes.h"
#include "PlotFunction.h"

class StabilityMapFunction : public PlotFunction
{
public:
    FUNC_ALIAS("StabMap")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Stability Map"))
    FUNC_ICON(":/toolbar/func_stab_map")

    StabilityMapFunction(Schema *schema) : PlotFunction(schema) {}

    void calculate() override;
    bool hasOptions() const override { return true; }
    void loadPrefs() override;

    Z::PointTS calculateAt(const Z::Value& v) override;

    Z::Enums::StabilityCalcMode stabilityCalcMode() const { return _stabilityCalcMode; }
    void setStabilityCalcMode(Z::Enums::StabilityCalcMode mode) { _stabilityCalcMode = mode; }

private:
    Z::Enums::StabilityCalcMode _stabilityCalcMode = Z::Enums::StabilityCalcMode::Normal;
};

#endif // STABILITY_MAP_FUNCTION_H
