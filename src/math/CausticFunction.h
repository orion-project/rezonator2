#ifndef CAUSTIC_FUNCTION_H
#define CAUSTIC_FUNCTION_H

#include "../math/PlotFunction.h"

#include <memory>

class AbcdBeamCalculator;
class PumpCalculator;
class PumpParams;

class CausticFunction : public PlotFunction
{
    Q_GADGET

public:
    enum Mode { BeamRadius, FrontRadius, HalfAngle, };
    Q_ENUM(Mode)
    enum SpecPointParam { spUnitX, spUnitW, spUnitR, spOffset };

    FUNC_ALIAS("Caustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Caustic"))
    FUNC_ICON(":/toolbar/func_caustic")
    FUNC_HELP(help_topic())
    static QString help_topic() { return "func_caustic.html"; }

    CausticFunction(Schema *schema) : PlotFunction(schema) {}

    // Only needs for SP schemas
    void setPump(PumpParams* pump) { _pump = pump; }
    PumpParams* pump() const { return _pump; }

    void calculate(CalculationMode calcMode = CALC_PLOT) override;
    Z::PointTS calculateAt(const Z::Value& arg) override;
    bool hasOptions() const override { return true; }
    bool hasSpecPoints() const override { return true; }
    QString calculateSpecPoints(const SpecPointParams& params) override;

    QString valueSymbol() const;
    QString beamsizeSymbol() const;

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
