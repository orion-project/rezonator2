#include "PlotHelpers.h"

#include "../app/AppSettings.h"
#include "../app/PersistentState.h"
#include "../core/Format.h"
#include "../math/FunctionGraph.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "widgets/OriOptionsGroup.h"

#include "qcpl_plot.h"
#include "qcpl_cursor.h"
#include "qcpl_cursor_panel.h"
#include "qcpl_format_editors.h"

#include <QLabel>
#include <QCheckBox>

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

//------------------------------------------------------------------------------
//                                Export
//------------------------------------------------------------------------------

QCPL::GraphDataExportSettings makeExportSettings()
{
    QCPL::GraphDataExportSettings es;
    const auto& as = AppSettings::instance();
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
    if (graphs.empty()) return;
    auto settings = makeExportSettings();
    settings.mergePoints = true;
    QCPL::GraphDataExporter exporter(settings);
    foreach (auto g, graphs)
        for (auto d : *(g->data().data()))
            exporter.add(d.key, d.value);
    exporter.toClipboard();
}

struct ExportGraphsParams
{
    enum { PLANE_T, PLANE_S, PLANE_BOTH };
    enum { SEGMENT_SELECTED, SEGMENT_ALL };
    enum { GRAPH_SELECTED, GRAPH_ALL };
    enum { FORMAT_CSV, FORMAT_TXT };
    enum { LOCALE_SYSTEM, LOCALE_C };
    int plane = PLANE_BOTH;
    int graph = GRAPH_ALL;
    int segment = SEGMENT_ALL;
    int format = FORMAT_CSV;
    int locale = LOCALE_C;
    int precision = 6;
    QString empty;

    ExportGraphsParams()
    {
        auto state = RecentData::getObj("export_graph_params");
        plane = state["plane"].toInt(PLANE_BOTH);
        graph = state["graph"].toInt(GRAPH_ALL);
        segment = state["segment"].toInt(SEGMENT_ALL);
        format = state["format"].toInt(FORMAT_CSV);
        locale = state["locale"].toInt(LOCALE_C);
        precision = state["precision"].toInt(6);
        empty = state["empty"].toString();
    }

    ~ExportGraphsParams()
    {
        RecentData::setObj("export_graph_params", {
            {"plane", plane},
            {"graph", graph},
            {"segment", segment},
            {"format", format},
            {"locale", locale},
            {"precision", precision},
            {"empty", empty},
        });
    }
};

void exportGraphsData(FunctionGraphSet* graphs, QCPGraph* selectedGraph, const ExportGraphsParams& params)
{
    FunctionGraphSet::ExportParams ep;
    ep.useT = params.plane == params.PLANE_T || params.plane == params.PLANE_BOTH;
    ep.useS = params.plane == params.PLANE_S || params.plane == params.PLANE_BOTH;
    ep.graph = params.graph == params.GRAPH_SELECTED ? selectedGraph : nullptr;
    ep.segment = params.segment == params.SEGMENT_SELECTED ? selectedGraph : nullptr;
    auto d = graphs->exportData(ep);

    //qApp->clipboard()->setText(graphs->str());
    //qApp->clipboard()->setText(d.str());
    //return;

    QCPL::GraphDataExportSettings es;
    es.csv = params.format == params.FORMAT_CSV;
    es.systemLocale = params.locale == params.LOCALE_SYSTEM;
    es.numberPrecision = params.precision;

    QCPL::BaseGraphDataExporter exporter(es);

    exporter.addValue("x");
    foreach (const auto& col, d.cols)
    {
        exporter.addSeparator();
        exporter.addValue(col);
    }
    exporter.addNewline();

    int colCount = d.cols.size();
    auto it = d.data.constBegin();
    while (it != d.data.constEnd())
    {
        double x = it.key();
        const auto& y = d.data[x];
        exporter.addValue(x);
        for (int c = 0; c < colCount; c++)
        {
            exporter.addSeparator();
            exporter.addValue(y.contains(c) ? exporter.format(y[c]) : params.empty);
        }
        exporter.addNewline();
        it++;
    }
    exporter.toClipboard();
}

void exportGraphsData(FunctionGraphSet* graphs, QCPGraph* selectedGraph)
{
    ExportGraphsParams params;

    auto plane = new OptionsGroup(qApp->translate("exportGraphsData", "Work Plane"), true);
    plane->addOption(ExportGraphsParams::PLANE_T, qApp->translate("exportGraphsData", "Tangential"));
    plane->addOption(ExportGraphsParams::PLANE_S, qApp->translate("exportGraphsData", "Sagittal"));
    plane->addOption(ExportGraphsParams::PLANE_BOTH, qApp->translate("exportGraphsData", "Both"));
    if (!graphs->multiGraphs().isEmpty())
    {
        plane->setVisible(false);
    }
    else if (graphs->T()->isEmpty())
    {
        params.plane = params.PLANE_S;
        plane->setEnabled(false);
    }
    else if (graphs->S()->isEmpty())
    {
        params.plane = params.PLANE_T;
        plane->setEnabled(false);
    }
    plane->setOption(params.plane);

    OptionsGroup::Params p1;
    p1.title = qApp->translate("exportGraphsData", "Graph Line");
    p1.horizontal = true;
    auto graph = new OptionsGroup(p1);
    graph->addOption(ExportGraphsParams::SEGMENT_ALL, qApp->translate("exportGraphsData", "All"));
    graph->addOption(ExportGraphsParams::SEGMENT_SELECTED, qApp->translate("exportGraphsData", "Selected"));
    graph->setVisible(!graphs->multiGraphs().isEmpty());
    graph->setDisabled(!selectedGraph);
    graph->setOption(params.graph);

    OptionsGroup::Params p2;
    p2.title = qApp->translate("exportGraphsData", "Line Segment");
    p2.horizontal = true;
    auto segment = new OptionsGroup(p2);
    segment->addOption(ExportGraphsParams::SEGMENT_ALL, qApp->translate("exportGraphsData", "All"));
    segment->addOption(ExportGraphsParams::SEGMENT_SELECTED, qApp->translate("exportGraphsData", "Selected"));
    segment->setDisabled(!selectedGraph);
    segment->setOption(params.segment);

    OptionsGroup::Params p3;
    p3.title = qApp->translate("exportGraphsData", "Data Format");
    p3.horizontal = true;
    auto format = new OptionsGroup(p3);
    format->addOption(ExportGraphsParams::FORMAT_CSV, qApp->translate("exportGraphsData", "CSV"));
    format->addOption(ExportGraphsParams::FORMAT_TXT, qApp->translate("exportGraphsData", "Plain text"));
    format->setOption(params.format);

    OptionsGroup::Params p4;
    p4.title = qApp->translate("exportGraphsData", "Number Format");
    p4.horizontal = true;
    auto locale = new OptionsGroup(p4);
    locale->addOption(ExportGraphsParams::LOCALE_C, qApp->translate("exportGraphsData", "C"));
    locale->addOption(ExportGraphsParams::LOCALE_SYSTEM, qApp->translate("exportGraphsData", "System"));
    locale->setOption(params.locale);

    auto precision = new QSpinBox();
    precision->setRange(1, 16);
    precision->setFixedWidth(QFontMetrics(precision->font()).boundingRect("00").width() * 4);
    precision->setValue(params.precision);

    auto empty = new QLineEdit;
    empty->setFixedWidth(precision->width());
    empty->setText(params.empty);

    auto options = new QGroupBox(qApp->translate("exportGraphsData", "Options"));
    options->setLayout(LayoutV({
        LayoutH({ new QLabel(qApp->translate("exportGraphsData", "Number precision")), precision }),
        LayoutH({ new QLabel(qApp->translate("exportGraphsData", "Replace NaN with")), empty }),
    }).boxLayout());

    auto dlg = Dialog(LayoutV({plane, graph, segment, format, locale, options}).setMargin(0).makeWidget(), true)
            .withTitle(qApp->translate("exportGraphsData", "Graphs Data to Clipboard"))
            .withContentToButtonsSpacingFactor(2);
    if (dlg.exec())
    {
        params.plane = plane->option();
        params.graph = graph->option();
        params.segment = segment->option();
        params.format = format->option();
        params.locale = locale->option();
        params.precision = precision->value();
        params.empty = empty->text();

        exportGraphsData(graphs, selectedGraph, params);
    }
}

//------------------------------------------------------------------------------
//                               Format
//------------------------------------------------------------------------------

bool formatPenDlg(const QPen& pen, const FormatPenDlgProps& props)
{
    QPen oldPen = pen;

    QDialog dlg(qApp->activeWindow());
    dlg.setWindowTitle(props.title);

    QCPL::PenEditorWidgetOptions opts;
    opts.enableNoPen = false;
    auto editor = new QCPL::PenEditorWidget(opts);
    editor->setValue(pen);

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->connect(buttons, &QDialogButtonBox::accepted, &dlg, [&dlg, props, editor](){
        props.onApply(editor->value());
        dlg.accept();
    });
    buttons->connect(buttons, &QDialogButtonBox::rejected, &dlg, [&dlg](){
        dlg.reject();
    });
    if (props.onReset)
    {
        auto resetBtn = buttons->addButton(dlg.tr("Reset"), QDialogButtonBox::ResetRole);
        resetBtn->connect(resetBtn, &QPushButton::pressed, &dlg, [&dlg, props](){
            props.onReset();
            dlg.accept();
        });
    }
    LayoutV({editor, SpaceV(2), buttons}).useFor(&dlg);
    if (dlg.exec() == QDialog::Accepted)
        return editor->value() != oldPen;
    return false;
}

//------------------------------------------------------------------------------
//                            Multi-graph helpers
//------------------------------------------------------------------------------

int applyGraphPen(QCPL::Plot* plot, const QString &name, const QPen &pen)
{
    int count = 0;
    for (int i = 0; i < plot->graphCount(); i++)
    {
        auto g = plot->graph(i);
        if (g->name() == name) {
            g->setPen(pen);
            count++;
        }
    }
    return count;
}

int graphCount(QCPL::Plot* plot, const QString &name)
{
    int count = 0;
    for (int i = 0; i < plot->graphCount(); i++)
    {
        auto g = plot->graph(i);
        if (g->name() == name)
            count++;
    }
    return count;
}

QVector<QCPGraph*> graphs(QCPL::Plot* plot, const QString &name)
{
    QVector<QCPGraph*> res;
    for (int i = 0; i < plot->graphCount(); i++)
    {
        auto g = plot->graph(i);
        if (g->name() == name)
            res << g;
    }
    return res;
}

} // namespace PlotHelpers

//------------------------------------------------------------------------------
//                               PlotCursorInfo
//------------------------------------------------------------------------------

QString PlotCursorInfo::format() const
{
    QString str;
    QTextStream info(&str);
    for (int i = 0; i < items.size(); i++)
    {
        const auto& it = items.at(i);
        switch (it.kind)
        {
        case SIMPLE:
            info << it.name << " = " << Z::format(it.value);
            break;
        case CURSOR_X:
            info << QCPL::CursorPanel::formatLinkX(Z::format(it.value));
            break;
        case CURSOR_Y:
            info << QCPL::CursorPanel::formatLinkY(Z::format(it.value));
            break;
        case VALUE_SI:
            info << it.name << " = " << Z::format(unitY->fromSi(it.value));
            break;
        case SECTION:
            info << "<b>" << it.name << ":</b> ";
            break;
        }
        if (!it.note.isEmpty())
            info << ' ' << it.note;
        if (it.kind != SECTION && i < items.size()-1)
            info << "; ";
    }
    return str;
}
