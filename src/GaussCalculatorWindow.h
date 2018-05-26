#ifndef GAUSS_CALCULATOR_WINDOW_H
#define GAUSS_CALCULATOR_WINDOW_H

#include <QWidget>

#include <functional>

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

class GaussCalculator;
class GaussGraphV;
class GaussGraphW;
class GaussGraphR;
class GaussPlotter;

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

    /// Take related value from calculator, apply it to owned parameter and display in editor.
    void getValueFromCalculator();

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
    Plot* _plot;
    std::shared_ptr<GaussCalculator> _calc;
    std::shared_ptr<GaussPlotter> _plotter;
    std::shared_ptr<GaussGraphV> _graphV;
    std::shared_ptr<GaussGraphW> _graphW;
    std::shared_ptr<GaussGraphR> _graphR;
    QVector<GaussCalcParamEditor*> _paramEditors;
    Ori::Widgets::ExclusiveActionGroup *_calcModeLock, *_calcModeZone,  *_plotPlusMinusZ,
        *_plotPlusMinusW, *_plotWR;
    Z::Parameter *_lambda, *_MI, *_w0, *_z, *_z0, *_Vs, *_w, *_R, *_reQ, *_imQ, *_reQ1, *_imQ1;
    QAction *_plotV;

    void makeParams(QGridLayout *paramsLayout);
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
