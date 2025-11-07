#ifndef PLOT_HELPERS_H
#define PLOT_HELPERS_H

#include "../core/Units.h"

#include "qcpl_export.h"

namespace QCPL {
class Plot;
class Cursor;
}

class QCPGraph;
class FunctionGraphSet;

enum class PlotAxis { X, Y };

namespace PlotHelpers {

void rescaleLimits(QCPL::Plot* plot, PlotAxis axis, Z::Unit unitFrom, Z::Unit unitTo);
void rescaleCursor(QCPL::Cursor* cursor, PlotAxis axis, Z::Unit unitFrom, Z::Unit unitTo);

QCPL::GraphDataExportSettings makeExportSettings();

void toClipboard(QCPGraph* graph);
void toClipboard(const QVector<QCPGraph*>& graphs);
void exportGraphsData(FunctionGraphSet* graphs, QCPGraph* selectedGraph);

struct FormatPenDlgProps
{
    QString title;
    std::function<void(const QPen&)> onApply;
    std::function<void()> onReset;
};

bool formatPenDlg(const QPen& pen, const FormatPenDlgProps& props);

// Multi-graph helpers
int applyGraphPen(QCPL::Plot* plot, const QString &name, const QPen &pen);
int graphCount(QCPL::Plot* plot, const QString &name);
QVector<QCPGraph*> graphs(QCPL::Plot* plot, const QString &name);

} // namespace PlotHelpers

struct PlotCursorInfo
{
    enum ItemKind {
        SIMPLE, // no special formatting rules
        CURSOR_X, // format as set_x command
        CURSOR_Y, // format as set_y command
        VALUE_SI, // should value from SI to Y-units
        SECTION, // format as a kind of title for subsequent values
    };

    struct Item
    {
        QString name;
        double value;
        QString note; // a text added after the value of any kind
        ItemKind kind = SIMPLE;
        Item(ItemKind kind, const double& value): value(value), kind(kind) {}
    };
    
    QList<Item> items;
    Z::Unit unitY = Z::Units::none();
    
    QString format() const;
    
    void operator << (const Item &it) { items << it; }
};

#endif // PLOT_HELPERS_H
