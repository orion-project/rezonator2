#ifndef STABILITY_MAP_FUNCTION_H
#define STABILITY_MAP_FUNCTION_H

#include "../funcs/PlotFunction.h"

class StabilityMapFunction : public PlotFunction
{
public:
    FUNC_ALIAS("StabMap")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Stability Map"))

    StabilityMapFunction(Schema *schema) : PlotFunction(schema) {}

    void calculate();
    QString calculateNotables();
};

#endif // STABILITY_MAP_FUNCTION_H
