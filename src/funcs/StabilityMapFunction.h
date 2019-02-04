#ifndef STABILITY_MAP_FUNCTION_H
#define STABILITY_MAP_FUNCTION_H

#include "../core/CommonTypes.h"
#include "../funcs/PlotFunction.h"

class StabilityMapFunction : public PlotFunction
{
public:
    FUNC_ALIAS("StabMap")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Stability Map"))

    StabilityMapFunction(Schema *schema) : PlotFunction(schema) {}

    void calculate() override;
    bool hasOptions() const override { return true; }
    void loadPrefs() override;
    const char* iconPath() const override { return ":/toolbar/func_stab_map"; }

    Z::Enums::StabilityCalcMode stabilityCalcMode() const { return _stabilityCalcMode; }
    void setStabilityCalcMode(Z::Enums::StabilityCalcMode mode) { _stabilityCalcMode = mode; }

    Z::Unit defaultUnitX() const override;

private:
    Z::Enums::StabilityCalcMode _stabilityCalcMode = Z::Enums::StabilityCalcMode::Normal;
};

#endif // STABILITY_MAP_FUNCTION_H
