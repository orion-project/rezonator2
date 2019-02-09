#ifndef BEAM_VARIATION_FUNCTION_H
#define BEAM_VARIATION_FUNCTION_H

#include "PlotFunction.h"

class BeamVariationFunction : public PlotFunction
{
public:
    FUNC_ALIAS("BeamVariation")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Beam Variation"))

    BeamVariationFunction(Schema *schema);

    const char* iconPath() const override { return ":/toolbar/func_beam_variation"; }
};

#endif // BEAM_VARIATION_FUNCTION_H
