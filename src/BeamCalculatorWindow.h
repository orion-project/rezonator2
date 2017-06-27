#ifndef BEAMCALCULATORWINDOW_H
#define BEAMCALCULATORWINDOW_H

#include <QWidget>

#include "core/BeamCalculator.h"
#include "core/Units.h"


QT_BEGIN_NAMESPACE
class QBoxLayout;
class QLabel;
QT_END_NAMESPACE

class QCPGraph;
class Plot;

namespace Ori {
namespace Gui {
    class ValueEdit;
    class ExclusiveActionGroup;
}}

using namespace Ori::Gui;

////////////////////////////////////////////////////////////////////////////////

class BeamPlotter
{
public:
    Z::Units::Set* units;

    double lambda;
    double w0;
    double z_max;
    double M2 = 1;

    QVector<double> valuesZ;
    QVector<double> valuesW;

    void calculate();
};

////////////////////////////////////////////////////////////////////////////////

class BeamCalculatorWindow : public QWidget
{
    Q_OBJECT
public:
    explicit BeamCalculatorWindow(QWidget *parent = 0);

private:
    BeamCalculator _calc;
    BeamPlotter _plotter;
    Z::Units::Set _units;
    Plot* _plot;
    QCPGraph *_graphPlusW, *_graphMinusW;

    ValueEdit *_editorLambda, *_editorW0, *_editorM2, *_editorZ0,
        *_editorVs, *_editorZ, *_editorW, *_editorR, *_editorReQ, *_editorImQ,
        *_editorReQ1, *_editorImQ1;

    QLabel *_labelLambda, *_labelW0, *_labelM2, *_labelZ0, *_labelVs, *_labelZ,
        *_labelW, *_labelR, *_labelQ, *_labelQ1;

    Ori::Gui::ExclusiveActionGroup *_plotPlusMinusZ, *_plotPlusMinusW;

    QWidget* makeToolbar();
    QWidget* makePlot();
    QLayout* makeParamEditors();
    ValueEdit* makeEditor();
    QLabel* makeLabel(const char *text, bool indentLeft = true);
    QLayout* makeEditor(ValueEdit** re, ValueEdit** im);

    void populateUnitLabels();
    void populateValues(QObject* exclude = nullptr);

private slots:
    void updatePlot();
    void valueChanged(double value);
    void zoneSelected(int zone);
    void lockSelected(int lock);
//    void plotPlusMinusSelected(int zone);
};

#endif // BEAMCALCULATORWINDOW_H
