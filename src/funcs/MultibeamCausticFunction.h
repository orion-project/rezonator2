#ifndef MULTI_BEAM_CAUSTIC_FUNCTION_H
#define MULTI_BEAM_CAUSTIC_FUNCTION_H

#include "MultiCausticFunction.h"

class MultibeamCausticFunction : public MultiCausticFunction
{
public:
    FUNC_ALIAS("MultibeamCaustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Multibeam Caustic"))

    MultibeamCausticFunction(Schema *schema) : MultiCausticFunction(schema) {}

    bool hasOptions() const override { return false; }

    CausticFunction::Mode mode() const = delete;
    void setMode(CausticFunction::Mode mode) = delete;
};

#endif // MULTI_BEAM_CAUSTIC_FUNCTION_H
