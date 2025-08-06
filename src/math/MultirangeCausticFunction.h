#ifndef MULTI_CAUSTIC_FUNCTION_H
#define MULTI_CAUSTIC_FUNCTION_H

#include "../math/CausticFunction.h"

class CausticFunction;

class MultirangeCausticFunction : public PlotFunction
{
public:
    FUNC_ALIAS("MultirangeCaustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "MR-Caustic"))
    FUNC_ICON(":/toolbar/func_multi_caustic")
    FUNC_HELP(help_topic())
    static QString help_topic() { return "func_caustic_mr.html"; }

    MultirangeCausticFunction(Schema *schema) : PlotFunction(schema) {}
    ~MultirangeCausticFunction() override;

    void calculate(CalculationMode calcMode = CALC_PLOT) override;
    bool hasOptions() const override { return true; }
    bool hasSpecPoints() const override { return true; }
    int resultCount(Z::WorkPlane plane) const override;
    const PlotFuncResult& result(Z::WorkPlane plane, int index) const override;
    QString calculateSpecPoints(const SpecPointParams& params) override;

    // Only needs for SP schemas
    void setPump(PumpParams* pump);
    PumpParams* pump() const;

    QVector<Z::Variable> args() const;
    void setArgs(const QVector<Z::Variable>& args);

    const QList<CausticFunction*>& funcs() { return _funcs; }

    CausticFunction::Mode mode() const;
    void setMode(CausticFunction::Mode mode);

    QString valueSymbol() const;

    Z::PointTS calculateAt(const Z::Value&arg) override;

private:
    QList<CausticFunction*> _funcs;
};

#endif // MULTI_CAUSTIC_FUNCTION_H
