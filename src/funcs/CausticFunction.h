#ifndef CAUSTIC_FUNCTION_H
#define CAUSTIC_FUNCTION_H

#include "../funcs/PlotFunction.h"

#include <memory>

class PumpCalculator;

class CausticFunction : public PlotFunction
{
    Q_GADGET

public:
    enum Mode { BeamRadius, FontRadius, HalfAngle, };
    Q_ENUM(Mode)

    FUNC_ALIAS("Caustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Caustic"))

    CausticFunction(Schema *schema) : PlotFunction(schema) {}

    void calculate() override;
    bool hasOptions() const override { return true; }
    QString calculatePoint(const double& arg) override;
    const char* iconPath() const override { return ":/toolbar/func_caustic"; }

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    Z::Unit argumentUnit() const { return _argumentUnit; }

private:
    /// Wich type of result the function should compute.
    Mode _mode = Mode::BeamRadius;

    Z::Unit _beamsizeUnit = Z::Units::mkm(); // TODO: make configurable
    Z::Unit _curvatureUnit = Z::Units::m(); // TODO: make configurable
    Z::Unit _angleUnit = Z::Units::deg(); // TODO: make configurable
    Z::Unit _argumentUnit = Z::Units::mm(); // TODO: make configurable

    /// Schema wavelength in SI units.
    double _wavelenSI = 0;

    Z::PairTS<std::shared_ptr<PumpCalculator>> _pumpCalc;

    bool prepareSP();
    inline Z::PointTS calculateSinglePass() const;
    inline Z::PointTS calculateResonator() const;
    double calculateResonator_beamRadius(const Z::Matrix& m) const;
    double calculateResonator_frontRadius(const Z::Matrix &m) const;
    double calculateResonator_halfAngle(const Z::Matrix &m) const;
    void convertFromSiToModeUnits(Z::PointTS& point) const;
};

#endif // CAUSTIC_FUNCTION_H
