#ifndef CAUSTIC_FUNCTION_H
#define CAUSTIC_FUNCTION_H

#include "../funcs/PlotFunction.h"

class CausticFunction : public PlotFunction
{
public:
    enum Mode { BeamRadius, FontRadius, HalfAngle, };

    FUNC_ALIAS("Caustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Caustic"))

    CausticFunction(Schema *schema) : PlotFunction(schema) {}

    void calculate() override;
    bool hasOptions() const override { return true; }
    QString calculatePoint(const double& arg) override;

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

private:
    Mode _mode = Mode::BeamRadius;
    Z::Complex _q_in_t, _q_in_s; // input complex ROC
    Z::RayVector _ray_in_t, _ray_in_s; // input ray vector
    Z::Unit _beamsizeUnit = Z::Units::mkm(); // TODO: make configurable
    Z::Unit _curvatureUnit = Z::Units::m(); // TODO: make configurable
    Z::Unit _angleUnit = Z::Units::deg(); // TODO: make configurable
    double _wavelenSI = 0;

    bool prepareSP();
    void prepareSP_vector();
    void prepareSP_sections();
    inline Z::PointTS calculateSinglePass() const;
    inline Z::PointTS calculateResonator() const;
    double calculateResonator_beamRadius(const Z::Matrix& m) const;
    double calculateResonator_frontRadius(const Z::Matrix &m) const;
    double calculateResonator_halfAngle(const Z::Matrix &m) const;
    void convertFromSiToModeUnits(Z::PointTS& point) const;
};

#endif // CAUSTIC_FUNCTION_H
