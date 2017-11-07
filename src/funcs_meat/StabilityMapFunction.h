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

    StabilityCalcMode stabilityCalcMode() const { return _stabilityCalcMode; }
    void setStabilityCalcMode(StabilityCalcMode mode) { _stabilityCalcMode = mode; }

private:
    StabilityCalcMode _stabilityCalcMode = StabilityCalcMode::Normal;
};

#endif // STABILITY_MAP_FUNCTION_H
