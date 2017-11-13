#ifndef CAUSTIC_FUNCTION_H
#define CAUSTIC_FUNCTION_H

#include "../funcs/PlotFunction.h"

class CausticFunction : public PlotFunction
{
public:
    enum Mode { Beamsize, CurvatureRadius, Angle, };

    FUNC_ALIAS("Caustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Caustic"))

    CausticFunction(Schema *schema) : PlotFunction(schema) {}

    void calculate() override;
    //bool hasOptions() const override { return true; }
    QString calculatePoint(const double& arg) override;

    Mode mode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }

private:
    enum { Pump_Gauss, Pump_Ray } _pumpMode;

    Mode _mode = Mode::Beamsize;
    Z::Complex _q_in_t, _q_in_s; // input complex ROC
    Z::RayVector _ray_in_t, _ray_in_s; // input ray vector
    ElementRange* _elem;

    bool prepareSP();
    void prepareSP_vector();
    void prepareSP_sections();
    Z::PointTS calculateSinglePass();
    Z::PointTS calculateResonator();
};

#endif // CAUSTIC_FUNCTION_H
