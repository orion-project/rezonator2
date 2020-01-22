#include "Plot.h"

#include "../Appearance.h"
#include "../AppSettings.h"

#include "helpers/OriDialogs.h"
#include "widgets/OriValueEdit.h"

#include <QFormLayout>
#include <QLabel>

Plot::Plot() :
        _safeMarginsX(AppSettings::instance().plotSafeMarginsPercentX/100.0),
        _safeMarginsY(AppSettings::instance().plotSafeMarginsPercentY/100.0),
        _zoomStepX(AppSettings::instance().plotZoomStepPercentX/100.0),
        _zoomStepY(AppSettings::instance().plotZoomStepPercentY/100.0),
        _numberPrecision(AppSettings::instance().plotNumberPrecision)
{
    yAxis->setNumberPrecision(_numberPrecision);
    xAxis->setNumberPrecision(_numberPrecision);

    // Secondary axes only form frame rect
    xAxis2->setVisible(true);
    yAxis2->setVisible(true);
    xAxis2->setTicks(false);
    yAxis2->setTicks(false);
    xAxis2->setSelectableParts({});
    yAxis2->setSelectableParts({});

    legend->setVisible(true);

    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables |
                    QCP::iSelectAxes | QCP::iSelectItems | QCP::iSelectLegend | QCP::iSelectOther);
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(plotSelectionChanged()));
    connect(this, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));
    connect(this, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(setLimitsDlg(QCPAxis*)));

    // TODO make font customizable
    auto f = font();

#ifdef Q_OS_MAC
    f.setPointSize(16);
#else
    f.setPointSize(14);
#endif
    _title = new QCPTextElement(this);
    _title->setFont(f);
    _title->setSelectedFont(f);
    _title->setSelectable(true);
    _title->setMargins({10, 10, 10, 0});
    _title->setVisible(false);
    //TODO:
    //plotLayout()->insertRow(0);
    //plotLayout()->addElement(0, 0, _title);

#ifdef Q_OS_MAC
    f.setPointSize(14);
#else
    f.setPointSize(10);
#endif
    xAxis->setLabelFont(f);
    yAxis->setLabelFont(f);
    xAxis->setSelectedLabelFont(f);
    yAxis->setSelectedLabelFont(f);
}

Plot::PlotPart Plot::selectedPart() const
{
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        return PlotPart::AxisX;

    if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        return PlotPart::AxisY;

    return PlotPart::None;
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
    switch (selectedPart()) {
    case PlotPart::AxisX:
        axisRect()->setRangeDrag(xAxis->orientation());
        break;
    case PlotPart::AxisY:
        axisRect()->setRangeDrag(yAxis->orientation());
        break;
    default:
        axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    }
    QCustomPlot::mousePressEvent(event);
}

void Plot::wheelEvent(QWheelEvent *event)
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed
    switch (selectedPart()) {
    case PlotPart::AxisX:
        axisRect()->setRangeZoom(xAxis->orientation());
        break;
    case PlotPart::AxisY:
        axisRect()->setRangeZoom(yAxis->orientation());
        break;
    default:
        axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
    QCustomPlot::wheelEvent(event);
}

void Plot::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu* menu = nullptr;
    QPointF pos(event->x(), event->y());
    if (xAxis->getPartAt(pos) != QCPAxis::spNone)
        menu = menuAxisX;
    else if (yAxis->getPartAt(pos) != QCPAxis::spNone)
        menu = menuAxisY;
    if (menu) menu->popup(event->globalPos());
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
    auto g = dynamic_cast<QCPGraph*>(plottable);
    if (_serviceGraphs.contains(g)) g = nullptr;
    emit graphSelected(g);
}

void Plot::autolimits(QCPAxis* axis, bool replot)
{
    QCPRange totalRange;
    bool isTotalValid = false;
    for (int i = 0; i < graphCount(); i++)
    {
        auto g = graph(i);

        if (!g->visible()) continue;

        if (excludeServiceGraphsFromAutolimiting)
            if (_serviceGraphs.contains(g))
                continue;

        bool hasRange = false;
        auto range = axis == xAxis
                ? g->getKeyRange(hasRange, QCP::sdBoth)
                : g->getValueRange(hasRange, QCP::sdBoth, QCPRange());
        if (!hasRange) continue;

        if (!isTotalValid)
        {
            totalRange = range;
            isTotalValid = true;
        }
        else totalRange.expand(range);
    }

    if (!isTotalValid) return;

    bool corrected = PlotUtils::correctZeroRange(totalRange, safeMargins(axis));
    axis->setRange(totalRange);

    if (!corrected && useSafeMargins)
        extendLimits(axis, safeMargins(axis), false);

    if (replot) this->replot();
}

void Plot::extendLimits(double factor, bool replot)
{
    extendLimits(xAxis, factor, false);
    extendLimits(yAxis, factor, replot);
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

AxisLimits Plot::limits(QCPAxis* axis) const
{
    auto range = axis->range();
    return AxisLimits(range.lower, range.upper);
}

double Plot::safeMargins(QCPAxis* axis)
{
    return axis == xAxis ? _safeMarginsX : _safeMarginsY;
}

void Plot::setAxisRange(QCPAxis* axis, const QCPRange& range)
{
    QCPRange r = range;
    PlotUtils::correctZeroRange(r, safeMargins(axis));
    axis->setRange(r);
}

bool Plot::setLimitsDlg(QCPAxis* axis)
{
    QString unitStr;
    if (getAxisUnitString)
        unitStr = getAxisUnitString(axis);
    auto range = axis->range();
    if (setLimitsDlg(range, tr("%1 Limits").arg(getAxisTitle(axis)), unitStr))
    {
        setAxisRange(axis, range);
        replot();
        return true;
    }
    return false;
}

bool Plot::setLimitsDlg(QCPRange& range, const QString& title, const QString &unit)
{
    auto editorMin = new Ori::Widgets::ValueEdit;
    auto editorMax = new Ori::Widgets::ValueEdit;
    editorMin->setFont(Z::Gui::ValueFont().get());
    editorMax->setFont(Z::Gui::ValueFont().get());
    editorMin->setNumberPrecision(_numberPrecision);
    editorMax->setNumberPrecision(_numberPrecision);
    editorMin->setValue(range.lower);
    editorMax->setValue(range.upper);
    editorMin->selectAll();

    QWidget w;

    auto layout = new QFormLayout(&w);
    layout->addRow(new QLabel(unit.isEmpty() ? QString("Min") : QString("Min (%1)").arg(unit)), editorMin);
    layout->addRow(new QLabel(unit.isEmpty() ? QString("Max") : QString("Max (%1)").arg(unit)), editorMax);

    if (Ori::Dlg::Dialog(&w, false).withTitle(title).exec())
    {
        range.lower = editorMin->value();
        range.upper = editorMax->value();
        range.normalize();
        return true;
    }
    return false;
}

QString Plot::getAxisTitle(QCPAxis* axis) const
{
   if (axis == xAxis)
       return tr("X-axis");
   if (axis == yAxis)
       return tr("Y-axis");
   auto label = axis->label();
   return label.isEmpty() ? tr("Axis") : label;
}

// TODO invisible title takes a room anyway and one can see empty space above the graph
// so we need to extract the title from layout when it is invisible
//void Plot::setTitleVisible(bool on)
//{
//    if (_title && on) return;
//    if (on)
//    {
//        // There is nothing to show
//        if (_titleText.isEmpty()) return;

//        _title = new QCPTextElement(this, "", QFont("sans", 14, QFont::Bold)); // TODO make font customizable
//        _title->setSelectable(true);
//        // TODO restore title format

//        plotLayout()->insertRow(0);
//        plotLayout()->addElement(0, 0, _title);
//        //connect(_title, &QCPTextElement::doubleClicked, [this](){ emit editTitleRequest(); });
//    }
//    else
//    {
//        // TODO backup title format
//        plotLayout()->remove(_title);
//        plotLayout()->simplify();
//        _title = nullptr;
//    }
//}
