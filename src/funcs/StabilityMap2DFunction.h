#ifndef STABILITY_MAP_2D_FUNCTION_H
#define STABILITY_MAP_2D_FUNCTION_H

#include "PlotFunction.h"

class StabilityMap2DFunction : public PlotFunction
{
public:
    FUNC_ALIAS("StabMap2D")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Contour Stability Map"))

    StabilityMap2DFunction(Schema *schema) : PlotFunction(schema) {}

    Z::Variable* paramX() { return &_paramX; }
    Z::Variable* paramY() { return &_paramY; }

    void calculate() override;
    bool hasOptions() const override { return true; }
    void loadPrefs() override;
    const char* iconPath() const override { return ":/toolbar/func_stab_map_2d"; }

private:
    Z::Variable _paramX, _paramY;
};

#endif // STABILITY_MAP_2D_FUNCTION_H
