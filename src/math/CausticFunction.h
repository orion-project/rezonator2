#ifndef CAUSTIC_FUNCTION_H
#define CAUSTIC_FUNCTION_H

#include "PlotFunction.h"

#include <memory>

class AbcdBeamCalculator;
class PumpCalculator;
class PumpParams;

class CausticFunction : public PlotFunction
{
    Q_GADGET

public:
    enum Mode { BeamRadius, FrontRadius, HalfAngle, };
    enum SpecPointParam { spUnitX, spUnitW, spUnitR };
    Q_ENUM(Mode)

    FUNC_ALIAS("Caustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Caustic"))
    FUNC_HELP("func_caustic.html")
    FUNC_ICON(":/toolbar/func_caustic")

    CausticFunction(Schema *schema) : PlotFunction(schema) {}

    // Only needs for SP schemas
    void setPump(PumpParams* pump) { _pump = pump; }

    Z::PointTS calculateAt(const Z::Value& arg) override;

    void calculate() override;
    bool hasOptions() const override { return true; }
    bool hasSpecPoints() const override { return true; }
    QString calculateSpecPoints(const SpecPointParams& params) override;

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

    static QString modeAlias(Mode mode);
    static QString modeDisplayName(Mode mode);

private:
    /// Wich type of result the function should compute.
    Mode _mode = Mode::BeamRadius;

    /// Index of refraction of an element in which the function is calculated
    double _ior = 0;

    PumpParams* _pump = nullptr;
    std::shared_ptr<PumpCalculator> _pumpCalc;
    std::shared_ptr<AbcdBeamCalculator> _beamCalc;
    bool _writeProtocol = false;

    bool prepareSinglePass(Element *ref);
    bool prepareResonator();
    inline Z::PointTS calculateSinglePass() const;
    inline Z::PointTS calculateResonator() const;
    Z::VariableRange givenRange();
};

#endif // CAUSTIC_FUNCTION_H
