#ifndef GAUSS_CALCULATOR_WINDOW_H
#define GAUSS_CALCULATOR_WINDOW_H

#include <QWidget>

#include <functional>

#include "funcs/GaussCalculator.h"
#include "core/Parameters.h"

QT_BEGIN_NAMESPACE
class QGridLayout;
QT_END_NAMESPACE

class QCPGraph;
class Plot;
class ParamEditor;

namespace Ori {
namespace Widgets {
class ExclusiveActionGroup;
}}

struct GaussPlotter
{
    double lambda;
    double w0;
    double maxZ;
    double MI;
    Z::Unit unitZ;
    Z::Unit unitW;

    const int points = 50;

    QVector<double> valuesZ;
    QVector<double> valuesW;

    void calculate();
};

class GaussCalcParamEditor : public QObject
{
    Q_OBJECT

public:
    using GaussCalcGetValueFunc = std::const_mem_fun_t<double, GaussCalculator>;
    using GaussCalcSetValueFunc = std::mem_fun1_t<void, GaussCalculator, const double&>;

    GaussCalcParamEditor(Z::Parameter *param,
                         GaussCalculator *calc,
                         GaussCalcGetValueFunc getValue,
                         GaussCalcSetValueFunc setValue,
                         bool invertedUnit = false);
    ~GaussCalcParamEditor();

    ParamEditor* editor() const { return _editor; }

    void populate();

signals:
    void calcNeeded();

private slots:
    void paramEdited();

private:
    GaussCalculator *_calc;
    Z::Parameter *_param;
    ParamEditor *_editor;
    GaussCalcGetValueFunc _getValueFromCalculator;
    GaussCalcSetValueFunc _setValueToCalculator;
    bool _invertedUnit;
};


class GaussCalculatorWindow : public QWidget, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit GaussCalculatorWindow(QWidget *parent = 0);
    ~GaussCalculatorWindow();

    static void showCalcWindow();

private:
    GaussCalculator _calc;
    GaussPlotter _plotter;
    Plot* _plot;
    QCPGraph *_graphPlusW, *_graphMinusW;
    QCPGraph *_graphAngle1, *_graphAngle2, *_graphAngle3, *_graphAngle4;
    QVector<GaussCalcParamEditor*> _params;
    QGridLayout *_paramsLayout;
    Ori::Widgets::ExclusiveActionGroup *_calcModeLock, *_calcModeZone,  *_plotPlusMinusZ,
        *_plotPlusMinusW, *_plotWR;
    Z::Parameter *_lambda, *_MI, *_w0, *_z, *_z0, *_Vs, *_w, *_R, *_reQ, *_imQ, *_reQ1, *_imQ1;
    QAction *_plotV;

    void makeParams();
    QWidget* makeToolbar();
    QWidget* makePlot();

    void recalc();

    static QString stateFileName();
    void restoreState();
    void storeState();

private slots:
    void updatePlot();
    void zoneSelected(int zone);
    void lockSelected(int lock);
};

#endif // GAUSS_CALCULATOR_WINDOW_H
