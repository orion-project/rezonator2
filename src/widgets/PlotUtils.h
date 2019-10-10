#ifndef PLOT_UTILS_H
#define PLOT_UTILS_H

/**
    Internal utility functions to be used inside of Plot class.
    They are supposed to be domain-independet and can't include any rezonator types.
    @sa PlotHelpers.
*/

#include <QString>

struct AxisLimits
{
    double min;
    double max;

    AxisLimits() { min = 0; max = 0; }
    AxisLimits(double min, double max) { this->min = min; this->max = max; }

    bool isInvalid() const;

    QString str() const;
};

namespace TitlePlaceholder
{
    QString defaultTitle();
}

#endif // PLOT_UTILS_H
