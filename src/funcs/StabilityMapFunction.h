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

    Z::Enums::StabilityCalcMode stabilityCalcMode() const { return _stabilityCalcMode; }
    void setStabilityCalcMode(Z::Enums::StabilityCalcMode mode) { _stabilityCalcMode = mode; }

private:
    Z::Enums::StabilityCalcMode _stabilityCalcMode = Z::Enums::StabilityCalcMode::Normal;
};

#endif // STABILITY_MAP_FUNCTION_H
