#ifndef CAUSTIC_FUNCTION_H
#define CAUSTIC_FUNCTION_H

#include "PlotFunction.h"

#include <memory>

class PumpCalculator;
class AbcdBeamCalculator;
namespace Z {
class PumpParams;
}

class CausticFunction : public PlotFunction
{
    Q_GADGET

public:
    enum Mode { BeamRadius, FontRadius, HalfAngle, };
    Q_ENUM(Mode)

    FUNC_ALIAS("Caustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Caustic"))

    CausticFunction(Schema *schema) : PlotFunction(schema) {}

    // Only needs for SP schemas
    void setPump(Z::PumpParams* pump) { _pump = pump; }

    void calculate() override;
    bool hasOptions() const override { return true; }
    QString calculatePoint(const double& arg) override;
    const char* iconPath() const override { return ":/toolbar/func_caustic"; }
    Z::Unit defaultUnitX() const override;
    Z::Unit defaultUnitY() const override { return defaultUnitsForMode(_mode); }

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    static Z::Unit defaultUnitsForMode(CausticFunction::Mode);

private:
    /// Wich type of result the function should compute.
    Mode _mode = Mode::BeamRadius;

    /// Schema wavelength in SI units.
    double _wavelenSI = 0;

    Z::PumpParams* _pump = nullptr;
    Z::PairTS<std::shared_ptr<PumpCalculator>> _pumpCalc;
    std::shared_ptr<AbcdBeamCalculator> _beamCalc;

    bool prepareSinglePass(Element *ref);
    bool prepareResonator();
    void prepareDynamicElements(Element* ref);
    inline Z::PointTS calculateSinglePass() const;
    inline Z::PointTS calculateResonator() const;
};

#endif // CAUSTIC_FUNCTION_H
