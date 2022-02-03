#ifndef PLOT_HELPERS_H
#define PLOT_HELPERS_H

/**
    Helper functions to interconnect plot with rezonator data types.
    They should not be used inside of Plot class.
    @sa PlotUtils.
*/

#include "../core/Units.h"

#include "qcpl_export.h"

namespace QCPL {
class Plot;
class Cursor;
}

class QCPGraph;

enum class PlotAxis { X, Y };

namespace PlotHelpers {

void rescaleLimits(QCPL::Plot* plot, PlotAxis axis, Z::Unit unitFrom, Z::Unit unitTo);
void rescaleCursor(QCPL::Cursor* cursor, PlotAxis axis, Z::Unit unitFrom, Z::Unit unitTo);

QCPL::GraphDataExportSettings makeExportSettings();

void toClipboard(QCPGraph* graph);
void toClipboard(const QVector<QCPGraph*>& graphs);

} // namespace PlotHelpers

#endif // PLOT_HELPERS_H
