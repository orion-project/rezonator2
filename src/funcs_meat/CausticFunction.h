#ifndef CAUSTIC_FUNCTION_H
#define CAUSTIC_FUNCTION_H

#include "../funcs/PlotFunction.h"

class CausticFunction : public PlotFunction
{
    Q_GADGET

public:
    enum Mode { BeamRadius, FontRadius, HalfAngle, };
    Q_ENUM(Mode)

    FUNC_ALIAS("Caustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Caustic"))

    CausticFunction(Schema *schema) : PlotFunction(schema) {}
    ~CausticFunction();

    void calculate() override;
    bool hasOptions() const override { return true; }
    QString calculatePoint(const double& arg) override;

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

private:
    /// Mode of input ray description for SP schema.
    enum class PumpMode
    {
        Gauss,    ///< Input beam is a Gaussian beam described by its complex ROC.
        RayVector ///< Inout beam is a geometric beam describied by it ray vector.
    };

    /// Wich type of result the function should compute.
    Mode _mode = Mode::BeamRadius;

    /// Input complex ROC for PumpMode::Gauss.
    Z::PairTS<Z::Complex> _inQ;

    /// Input ray vector for PumpMode::RayVector.
    Z::PairTS<Z::RayVector> _inRay;

    Z::Unit _beamsizeUnit = Z::Units::mkm(); // TODO: make configurable
    Z::Unit _curvatureUnit = Z::Units::m(); // TODO: make configurable
    Z::Unit _angleUnit = Z::Units::deg(); // TODO: make configurable

    /// Schema wavelength in SI units.
    double _wavelenSI = 0;

    /// Beam quality parameter for SP schemas.
    Z::PointTS _MI;

    /// A way to calcuate input ray for SP schema.
    PumpMode _pumpMode;

    bool prepareSP();
    inline Z::PointTS calculateSinglePass() const;
    inline Z::PointTS calculateResonator() const;
    double calculateResonator_beamRadius(const Z::Matrix& m) const;
    double calculateResonator_frontRadius(const Z::Matrix &m) const;
    double calculateResonator_halfAngle(const Z::Matrix &m) const;
    void convertFromSiToModeUnits(Z::PointTS& point) const;
};

#endif // CAUSTIC_FUNCTION_H
