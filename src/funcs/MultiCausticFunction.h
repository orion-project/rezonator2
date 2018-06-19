#ifndef MULTI_CAUSTIC_FUNCTION_H
#define MULTI_CAUSTIC_FUNCTION_H

#include "../funcs/PlotFunction.h"

class MultiCausticFunction : public PlotFunction
{
public:
    FUNC_ALIAS("Multicaustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Multicaustic"))

    MultiCausticFunction(Schema *schema) : PlotFunction(schema) {}

    void calculate() override {}
};

#endif // MULTI_CAUSTIC_FUNCTION_H
