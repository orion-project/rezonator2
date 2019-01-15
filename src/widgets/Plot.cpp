#include "Plot.h"

#include "Appearance.h"
#include "../AppSettings.h"
#include "helpers/OriDialogs.h"
#include "widgets/OriValueEdit.h"

#include <QFormLayout>
#include <QLabel>

Plot::Plot() :
        _safeMarginsX(Settings::instance().plotSafeMarginsPercentX/100.0),
        _safeMarginsY(Settings::instance().plotSafeMarginsPercentY/100.0),
        _zoomStepX(Settings::instance().plotZoomStepPercentX/100.0),
        _zoomStepY(Settings::instance().plotZoomStepPercentY/100.0),
        _numberPrecision(Settings::instance().plotNumberPrecision)
{
    yAxis->setNumberPrecision(_numberPrecision);
    xAxis->setNumberPrecision(_numberPrecision);

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
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(yAxis->orientation());
    else
        axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);

    QCustomPlot::mousePressEvent(event);
}

void Plot::wheelEvent(QWheelEvent *event)
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(yAxis->orientation());
    else
        axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    QCustomPlot::wheelEvent(event);
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
    if (!sanitizeAxisRange(xAxis))
        extendLimits(xAxis, _safeMarginsX, false);
    if (!sanitizeAxisRange(yAxis))
        extendLimits(yAxis, _safeMarginsY, false);
    if (replot) this->replot();
}

bool Plot::sanitizeAxisRange(QCPAxis* axis)
{
    auto range = axis->range();
    if (sanitizeRange(range, safeMargins(axis)))
    {
        axis->setRange(range);
        return true;
    }
    return false;
}

bool Plot::sanitizeRange(QCPRange& range, double safeMargin)
{
    auto epsilon = std::numeric_limits<double>::epsilon();
    if (range.size() <= epsilon)
    {
        if (qAbs(range.lower) <= epsilon)
        {
            range.lower = -1;
            range.upper = 1;
        }
        else
        {
            double delta = range.lower * safeMargin;
            range.lower -= delta;
            range.upper += delta;
        }
        return true;
    }
    return false;
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

void Plot::extendLimits(double factor, bool replot)
{
    extendLimitsX(factor, false);
    extendLimitsY(factor, false);
    if (replot) this->replot();
}

void Plot::extendLimits(QCPAxis* axis, double factor, bool replot)
{
    auto range = axis->range();
    auto delta = (range.upper - range.lower) * factor;
    range.upper += delta;
    range.lower -= delta;
    setAxisRange(axis, range);
    if (replot) this->replot();
}

void Plot::setLimits(QCPAxis* axis, double min, double max, bool replot)
{
    QCPRange range(min, max);
    range.normalize();
    setAxisRange(axis, range);
    if (replot) this->replot();
}

QPair<double, double> Plot::limits(QCPAxis* axis) const
{
    auto range = axis->range();
    return QPair<double, double>(range.lower, range.upper);
}

double Plot::safeMargins(QCPAxis* axis)
{
    return axis == xAxis ? _safeMarginsX : _safeMarginsY;
}

void Plot::setAxisRange(QCPAxis* axis, const QCPRange& range)
{
    auto r = range;
    sanitizeRange(r, safeMargins(axis));
    axis->setRange(r);
}

bool Plot::setLimitsDlg()
{
    auto range = xAxis->range();
    if (setLimitsDlg(range, tr("Limits for X and Y")))
    {
        setAxisRange(xAxis, range);
        setAxisRange(yAxis, range);
        replot();
        return true;
    }
    return false;
}

bool Plot::setLimitsDlg(QCPAxis* axis)
{
    auto range = axis->range();
    if (setLimitsDlg(range, tr("%1-axis Limits").arg(axis == xAxis ? "X" : "Y")))
    {
        setAxisRange(axis, range);
        replot();
        return true;
    }
    return false;
}

bool Plot::setLimitsDlg(QCPRange& range, const QString& title)
{
    auto editorMin = new Ori::Widgets::ValueEdit;
    auto editorMax = new Ori::Widgets::ValueEdit;
    Z::Gui::setValueFont(editorMin);
    Z::Gui::setValueFont(editorMax);
    editorMin->setNumberPrecision(_numberPrecision);
    editorMax->setNumberPrecision(_numberPrecision);
    editorMin->setValue(range.lower);
    editorMax->setValue(range.upper);
    editorMin->selectAll();

    QWidget w;

    auto layout = new QFormLayout(&w);
    layout->addRow(new QLabel("Min"), editorMin);
    layout->addRow(new QLabel("Max"), editorMax);

    if (Ori::Dlg::Dialog(&w).withTitle(title).exec())
    {
        range.lower = editorMin->value();
        range.upper = editorMax->value();
        range.normalize();
        return true;
    }
    return false;
}
