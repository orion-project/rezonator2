#include "Plot.h"

#include "Appearance.h"
#include "helpers/OriDialogs.h"
#include "widgets/OriValueEdit.h"

#include <QFormLayout>
#include <QLabel>

Plot::Plot()
{
    legend->setVisible(true);
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables |
                    QCP::iSelectAxes | QCP::iSelectItems | QCP::iSelectLegend | QCP::iSelectOther);
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(plotSelectionChanged()));
    connect(this, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));
    connect(this, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(setLimitsDlg(QCPAxis*)));
}

void Plot::mouseDoubleClickEvent(QMouseEvent *event)
{
    QCustomPlot::mouseDoubleClickEvent(event);

    QCPLayerable *selectedLayerable = layerableAt(event->pos(), true);
    if (!selectedLayerable)
        emit emptySpaceDoubleClicked(event);
}

void Plot::mousePressEvent(QMouseEvent *event)
{
    QCustomPlot::mousePressEvent(event);

    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(yAxis->orientation());
    else
        axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
}

void Plot::wheelEvent(QWheelEvent *event)
{
    QCustomPlot::wheelEvent(event);

    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(yAxis->orientation());
    else
        axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}

void Plot::plotSelectionChanged()
{
    // handle axis and tick labels as one selectable object:
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
        xAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
        xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    if (yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
        yAxis->selectedParts().testFlag(QCPAxis::spTickLabels))
        yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
}

void Plot::graphClicked(QCPAbstractPlottable *plottable)
{
    auto g = dynamic_cast<Graph*>(plottable);
    if (_serviceGraphs.contains(g)) g = nullptr;
    emit graphSelected(g);
}

void Plot::autolimits(bool replot)
{
    bool onlyEnlarge = false;
    for (int i = 0; i < graphCount(); i++)
    {
        auto g = graph(i);
        if (g && !_serviceGraphs.contains(g))
        {
            g->rescaleAxes(onlyEnlarge);
            onlyEnlarge = true;
        }
    }
    // Apply safe margins
    extendLimitsX(0.01, false);
    extendLimitsY(0.01, false);
    if (replot) this->replot();
}

void Plot::autolimitsX(bool replot)
{
    auto range = yAxis->range();
    autolimits(false);
    yAxis->setRange(range);
    if (replot) this->replot();
}

void Plot::autolimitsY(bool replot)
{
    auto range = xAxis->range();
    autolimits(false);
    xAxis->setRange(range);
    if (replot) this->replot();
}

void Plot::extendLimits(QCPAxis* axis, double factor, bool replot)
{
    auto range = axis->range();
    auto delta = (range.upper - range.lower) * factor;
    range.upper += delta;
    range.lower -= delta;
    axis->setRange(range);
    if (replot) this->replot();
}

void Plot::setLimits(QCPAxis* axis, double min, double max, bool replot)
{
    axis->setRange(qMin(min, max), qMax(min, max));
    if (replot) this->replot();
}

QPair<double, double> Plot::limits(QCPAxis* axis) const
{
    auto range = axis->range();
    return QPair<double, double>(range.lower, range.upper);
}

bool Plot::setLimitsDlg(QCPAxis* axis)
{
    auto range = axis->range();
    auto editorMin = new Ori::Widgets::ValueEdit;
    auto editorMax = new Ori::Widgets::ValueEdit;
    Z::Gui::setValueFont(editorMin);
    Z::Gui::setValueFont(editorMax);
    editorMin->setValue(range.lower);
    editorMax->setValue(range.upper);
    editorMin->selectAll();

    QWidget w;

    auto layout = new QFormLayout(&w);
    layout->addRow(new QLabel("Min"), editorMin);
    layout->addRow(new QLabel("Max"), editorMax);

    if (Ori::Dlg::Dialog(&w)
            .withTitle(tr("%1-axis Limits").arg(axis == xAxis ? "X" : "Y"))
            .exec())
    {
        setLimits(axis, editorMin->value(), editorMax->value(), true);
        return true;
    }
    return false;
}
