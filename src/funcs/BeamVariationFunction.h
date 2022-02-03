#ifndef BEAM_VARIATION_FUNCTION_H
#define BEAM_VARIATION_FUNCTION_H

#include "PlotFunction.h"

#include <memory>

class PumpCalculator;
class AbcdBeamCalculator;

class BeamVariationFunction : public PlotFunction
{
public:
    FUNC_ALIAS("BeamVariation")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Beam Variation"))
    FUNC_ICON(":/toolbar/func_beam_variation")

    BeamVariationFunction(Schema *schema) : PlotFunction (schema) {}

    void calculate() override;

    Z::PointTS calculateAt(const Z::Value& v) override;

    Z::PlotPosition* pos() { return &_pos; }

protected:
    Z::PlotPosition _pos;

    /// Index of refraction of an element in which the function is calculated
    double _ior = 0;

    Z::PairTS<std::shared_ptr<PumpCalculator>> _pumpCalc;
    std::shared_ptr<AbcdBeamCalculator> _beamCalc;

    bool prepareSinglePass();
    bool prepareResonator();
    inline Z::PointTS calculateSinglePass() const;
    inline Z::PointTS calculateResonator() const;
};

#endif // BEAM_VARIATION_FUNCTION_H
