#include "PlotHelpers.h"

#include "qcpl_plot.h"

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

} // namespace PlotHelpers
