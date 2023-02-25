#include "PlotHelpers.h"

#include "../AppSettings.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "widgets/OriOptionsGroup.h"

#include "qcpl_plot.h"
#include "qcpl_cursor.h"

#include <QLabel>

using namespace Ori::Dlg;
using namespace Ori::Layouts;
using namespace Ori::Widgets;

namespace PlotHelpers {

void rescaleLimits(QCPL::Plot* plot, PlotAxis axis, Z::Unit unitFrom, Z::Unit unitTo)
{
    auto limits = axis == PlotAxis::X
            ? plot->limitsX()
            : plot->limitsY();
    limits.min = unitTo->fromSi(unitFrom->toSi(limits.min));
    limits.max = unitTo->fromSi(unitFrom->toSi(limits.max));
    if (axis == PlotAxis::X)
        plot->setLimitsX(limits, false);
    else
        plot->setLimitsY(limits, false);
}

void rescaleCursor(QCPL::Cursor* cursor, PlotAxis axis, Z::Unit unitFrom, Z::Unit unitTo)
{
    auto pos = axis == PlotAxis::X
            ? cursor->position().x()
            : cursor->position().y();
    pos = unitTo->fromSi(unitFrom->toSi(pos));
    if (axis == PlotAxis::X)
        cursor->setPositionX(pos, false);
    else
        cursor->setPositionY(pos, false);
}

QCPL::GraphDataExportSettings makeExportSettings()
{
    QCPL::GraphDataExportSettings es;
    auto as = AppSettings::instance();
    es.csv = as.exportAsCsv;
    es.systemLocale = as.exportSystemLocale;
    es.numberPrecision = as.exportNumberPrecision;
    es.transposed = as.exportTransposed;
    return es;
}

void toClipboard(QCPGraph* graph)
{
    if (!graph) return;
    QCPL::GraphDataExporter exporter(makeExportSettings());
    for (auto d : *(graph->data().data()))
        exporter.add(d.key, d.value);
    exporter.toClipboard();
}

void toClipboard(const QVector<QCPGraph*>& graphs)
{
    auto settings = makeExportSettings();
    settings.mergePoints = true;
    QCPL::GraphDataExporter exporter(settings);
    foreach (auto g, graphs)
        for (auto d : *(g->data().data()))
            exporter.add(d.key, d.value);
    exporter.toClipboard();
}

void exportGraphsData()
{
    auto optsPlane = new OptionsGroup(qApp->translate("exportGraphsData", "Work Plane"), {
                                          qApp->translate("exportGraphsData", "Tangential"),
                                          qApp->translate("exportGraphsData", "Sagittal"),
                                          qApp->translate("exportGraphsData", "Both")
                                      }, true);
    auto optsSegment = new OptionsGroup(qApp->translate("exportGraphsData", "Graph Segment"), {
                                            qApp->translate("exportGraphsData", "Selected"),
                                            qApp->translate("exportGraphsData", "All"),
                                        }, true);

    auto labelSettings = new QLabel(qApp->translate("exportGraphsData", "Check also <a href='s'>Global Export Settings</a>"));
    labelSettings->connect(labelSettings, &QLabel::linkActivated, []{
        AppSettings::instance().edit(AppSettings::PageExport);
    });

    optsPlane->setOption(2);
    optsSegment->setOption(1);

    auto dlg = Dialog(LayoutV({optsPlane, optsSegment, labelSettings}).setMargin(0).makeWidget(), true)
            .withTitle(qApp->translate("exportGraphsData", "Export Graphs Data"))
            .withContentToButtonsSpacingFactor(3);
    if (dlg.exec())
    {
        // TODO
    }
}

} // namespace PlotHelpers
