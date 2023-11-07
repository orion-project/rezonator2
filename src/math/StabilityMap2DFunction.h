#ifndef STABILITY_MAP_2D_FUNCTION_H
#define STABILITY_MAP_2D_FUNCTION_H

#include "PlotFunction.h"

class StabilityMap2DFunction : public PlotFunction
{
public:
    FUNC_ALIAS("StabMap2D")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "2D-Stability"))
    FUNC_ICON(":/toolbar/func_stab_map_2d")

    StabilityMap2DFunction(Schema *schema) : PlotFunction(schema) {}

    Z::Variable* paramX() { return &_paramX; }
    Z::Variable* paramY() { return &_paramY; }

    const Z::PlottingRange& rangeX() const { return _rangeX; }
    const Z::PlottingRange& rangeY() const { return _rangeY; }
    const QVector<double>& resultsT() const { return _resultsT; }
    const QVector<double>& resultsS() const { return _resultsS; }

    void calculate() override;
    bool hasOptions() const override { return true; }
    bool hasDataTable() const override { return false; }
    void loadPrefs() override;

    Z::PointTS calculateAtXY(const Z::Value& x, const Z::Value& y);

    Z::Enums::StabilityCalcMode stabilityCalcMode() const { return _stabilityCalcMode; }
    void setStabilityCalcMode(Z::Enums::StabilityCalcMode mode) { _stabilityCalcMode = mode; }

private:
    Z::Variable _paramX, _paramY;
    Z::Enums::StabilityCalcMode _stabilityCalcMode = Z::Enums::StabilityCalcMode::Normal;
    QVector<double> _resultsT, _resultsS;
    Z::PlottingRange _rangeX, _rangeY;

    bool checkArg(Z::Variable* arg);
};

#endif // STABILITY_MAP_2D_FUNCTION_H
