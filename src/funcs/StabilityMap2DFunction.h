#ifndef STABILITY_MAP_2D_FUNCTION_H
#define STABILITY_MAP_2D_FUNCTION_H

#include "PlotFunction.h"

class QCPColorMap;

class StabilityMap2DFunction : public PlotFunction
{
public:
    FUNC_ALIAS("StabMap2D")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "2D Stability Map"))

    StabilityMap2DFunction(Schema *schema) : PlotFunction(schema) {}

    Z::Variable* paramX() { return &_paramX; }
    Z::Variable* paramY() { return &_paramY; }

    void calculate() override;
    bool hasOptions() const override { return true; }
    bool hasDataTable() const override { return false; }
    void loadPrefs() override;
    const char* iconPath() const override { return ":/toolbar/func_stab_map_2d"; }
    Z::Unit defaultUnitX() const override;
    Z::Unit defaultUnitY() const override;

    QCPColorMap *graphT = nullptr, *graphS = nullptr;

    Z::Enums::StabilityCalcMode stabilityCalcMode() const { return _stabilityCalcMode; }
    void setStabilityCalcMode(Z::Enums::StabilityCalcMode mode) { _stabilityCalcMode = mode; }

private:
    Z::Variable _paramX, _paramY;
    Z::Enums::StabilityCalcMode _stabilityCalcMode = Z::Enums::StabilityCalcMode::Normal;

    bool checkArg(Z::Variable* arg);
};

#endif // STABILITY_MAP_2D_FUNCTION_H
