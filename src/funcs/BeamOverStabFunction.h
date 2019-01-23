#ifndef BEAM_OVER_STAB_FUNCTION_H
#define BEAM_OVER_STAB_FUNCTION_H

#include "PlotFunction.h"

class BeamOverStabFunction : public PlotFunction
{
public:
    FUNC_ALIAS("BeamVsStab")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Beam Over Stability"))

    BeamOverStabFunction(Schema *schema);

    const char* iconPath() const override { return ":/toolbar/func_beam_over_stab"; }
};

#endif // BEAM_OVER_STAB_FUNCTION_H
