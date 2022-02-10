#ifndef GAUSS_CALCULATOR_WINDOW_H
#define GAUSS_CALCULATOR_WINDOW_H

#include <QWidget>

#include <memory>

#include "core/Parameters.h"

QT_BEGIN_NAMESPACE
class QGridLayout;
class QLabel;
QT_END_NAMESPACE

class QCPGraph;

class ParamEditor;

namespace QCPL {
class Plot;
}

namespace Ori {
namespace Widgets {
class ExclusiveActionGroup;
}}

class GaussCalculator;
class GaussGraphZ0;
class GaussGraphV;
class GaussGraphW;
class GaussGraphR;
class GaussPlotter;

class GaussCalcParamEditor : public QObject
{
    Q_OBJECT

public:
    using GaussCalcGetValueFunc = double (GaussCalculator::*)() const;
    using GaussCalcSetValueFunc = void (GaussCalculator::*)(const double&);

    GaussCalcParamEditor(Z::Parameter *param,
                         GaussCalculator *calc,
                         GaussCalcGetValueFunc getValue,
                         GaussCalcSetValueFunc setValue,
                         bool invertedUnit = false);
    ~GaussCalcParamEditor() override;

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

    double paramValueSI() const;
};


class GaussCalculatorWindow : public QWidget, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit GaussCalculatorWindow(QWidget *parent = nullptr);
    ~GaussCalculatorWindow() override;

    static void showWindow();

private:
    QCPL::Plot* _plot;
    std::shared_ptr<GaussCalculator> _calc;
    std::shared_ptr<GaussPlotter> _plotter;
    std::shared_ptr<GaussGraphZ0> _graphZ0;
    std::shared_ptr<GaussGraphV> _graphV;
    std::shared_ptr<GaussGraphW> _graphW;
    std::shared_ptr<GaussGraphR> _graphR;
    QVector<GaussCalcParamEditor*> _paramEditors;
    Ori::Widgets::ExclusiveActionGroup *_calcModeLock, *_calcModeZone,  *_plotPlusMinusZ,
        *_plotPlusMinusW, *_plotWR;
    Z::Parameter *_lambda, *_MI, *_w0, *_z, *_z0, *_Vs, *_w, *_R, *_reQ, *_imQ, *_reQ1, *_imQ1;
    QVector<Z::Parameter*> _params;
    QAction *_plotV, *_plotZ0;
    QLabel *_warning;
    bool _updatesEnabled = false;

    void makeParams(QGridLayout *paramsLayout);
    QWidget* makeToolbar();

    void recalc();

    void restoreState();
    void storeState();

private slots:
    void updatePlot();
    void zoneSelected(int zone);
    void lockSelected(int lock);
    void showCalculator();
    void showHelp();
    void copyGraphData();
};

#endif // GAUSS_CALCULATOR_WINDOW_H
