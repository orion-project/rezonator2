#ifndef MULTI_CAUSTIC_FUNCTION_H
#define MULTI_CAUSTIC_FUNCTION_H

#include "CausticFunction.h"

class CausticFunction;

class MultiCausticFunction : public PlotFunction
{
public:
    FUNC_ALIAS("Multicaustic")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Multicaustic"))

    MultiCausticFunction(Schema *schema) : PlotFunction(schema) {}
    ~MultiCausticFunction();

    void calculate() override;
    bool hasOptions() const override { return true; }
    int resultCount(Z::WorkPlane plane) const override;
    const PlotFuncResult& result(Z::WorkPlane plane, int index) const override;

    QVector<Z::Variable> args() const;
    void setArgs(const QVector<Z::Variable>& args);

    QList<CausticFunction*> funcs() { return _funcs; }

    CausticFunction::Mode mode() const;
    void setMode(CausticFunction::Mode mode);

private:
    QList<CausticFunction*> _funcs;
};

#endif // MULTI_CAUSTIC_FUNCTION_H
