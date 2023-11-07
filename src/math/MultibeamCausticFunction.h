#ifndef MULTI_BEAM_CAUSTIC_FUNCTION_H
#define MULTI_BEAM_CAUSTIC_FUNCTION_H

#include "MultirangeCausticFunction.h"

class MultibeamCausticFunction : public MultirangeCausticFunction
{
public:
    FUNC_ALIAS("MultibeamCaustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "MB-Caustic"))
    FUNC_ICON(":/toolbar/func_multi_beam_caustic")

    MultibeamCausticFunction(Schema *schema);

    bool hasOptions() const override { return false; }

    CausticFunction::Mode mode() const = delete;
    void setMode(CausticFunction::Mode mode) = delete;
};

#endif // MULTI_BEAM_CAUSTIC_FUNCTION_H
