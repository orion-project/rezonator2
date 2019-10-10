#ifndef PLOT_HELPERS_H
#define PLOT_HELPERS_H

/**
    Helper functions to interconnect plot with rezonator data types.
    They should not be used inside of Plot class.
    @sa PlotUtils.
*/

#include "../core/Units.h"

class Plot;

enum class PlotAxis { X, Y };

namespace PlotHelpers {

void rescaleLimits(Plot* plot, PlotAxis axis, Z::Unit unitFrom, Z::Unit unitTo);

} // namespace PlotHelpers

#endif // PLOT_HELPERS_H
