#ifndef STABILITY_MAP_FUNCTION_H
#define STABILITY_MAP_FUNCTION_H

#include "../core/CommonTypes.h"
#include "../math/PlotFunction.h"

class StabilityMapFunction : public PlotFunction
{
public:
    enum SpecPointParam { spUnitX };

    FUNC_ALIAS("StabMap")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Stability Map"))
    FUNC_ICON(":/toolbar/func_stab_map")

    StabilityMapFunction(Schema *schema) : PlotFunction(schema) {}

    void calculate() override;
    bool hasOptions() const override { return true; }
    bool hasSpecPoints() const override { return true; }
    void loadPrefs() override;
    QString calculateSpecPoints(const SpecPointParams& params) override;

    Z::PointTS calculateAt(const Z::Value& v) override;
    QVector<Z::RangeSi> calcStabilityBounds(Z::WorkPlane ts) const;

    Z::Enums::StabilityCalcMode stabilityCalcMode() const { return _stabilityCalcMode; }
    void setStabilityCalcMode(Z::Enums::StabilityCalcMode mode) { _stabilityCalcMode = mode; }

private:
    Z::Enums::StabilityCalcMode _stabilityCalcMode = Z::Enums::StabilityCalcMode::Normal;

    Z::PlottingRange _plotRange;

    struct ApproxBound { double value; bool exact; };
    struct ApproxRange { ApproxBound start; ApproxBound stop; };
    Z::PairTS<QVector<ApproxRange>> _approxStabBounds;
};

#endif // STABILITY_MAP_FUNCTION_H
