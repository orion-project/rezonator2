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

//using namespace Ori::Gui;


//class BeamPlotter
//{
//public:
//    Z::Units::Set* units;

//    double lambda;
//    double w0;
//    double z_max;
//    double M2 = 1;

//    QVector<double> valuesZ;
//    QVector<double> valuesW;

//    void calculate();
//};

using GaussCalcSetValueFunc = std::mem_fun1_t<void, GaussCalculator, const double&>;
using GaussCalcGetValueFunc = std::const_mem_fun_t<double, GaussCalculator>;

class GaussCalcParam : public QObject
{
    Q_OBJECT

public:
    GaussCalcParam(GaussCalculator *calc,
        Z::Dim dim, const QString& alias, const QString& name,
        GaussCalcSetValueFunc setValue,
        GaussCalcGetValueFunc getValue);
    ~GaussCalcParam();

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
    GaussCalcSetValueFunc _setValueToCalculator;
    GaussCalcGetValueFunc _getValueFromCalculator;
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
    Plot* _plot;
    QCPGraph *_graphPlusW, *_graphMinusW;
    QVector<GaussCalcParam*> _params;
    QGridLayout *_paramsLayout;
    Ori::Widgets::ExclusiveActionGroup *_plotPlusMinusZ, *_plotPlusMinusW;

    void addParam(int row, int col, GaussCalcParam *param);
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
