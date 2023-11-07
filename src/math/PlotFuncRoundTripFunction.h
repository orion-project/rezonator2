#ifndef PLOT_FUNC_ROUND_TRIP_FUNCTION_H
#define PLOT_FUNC_ROUND_TRIP_FUNCTION_H

#include "FunctionBase.h"

class PlotFunction;

class PlotFuncRoundTripFunction : public InfoFunction, public FunctionListener
{
public:
    explicit PlotFuncRoundTripFunction(const QString& funcTitle, PlotFunction* func);
    ~PlotFuncRoundTripFunction() override;

    FUNC_HELP("func_rt_plot.html")
    FUNC_NAME("Round-trip for " + _funcTitle)

    QString calculateInternal() override;

    void functionCalculated(FunctionBase*) override;
    void functionDeleted(FunctionBase*) override;

private:
    QString _funcTitle;
    PlotFunction* _function;
    bool _showElems = false;
};

#endif // PLOT_FUNC_ROUND_TRIP_FUNCTION_H
