#ifndef PLOT_UTILS_H
#define PLOT_UTILS_H

/**
    Internal utility functions to be used inside of Plot class.
    They are supposed to be domain-independet and can't include any rezonator types.
    @sa PlotHelpers.
*/

#include <QString>

class QCPRange;
class QCPAbstractPlottable;

namespace TitlePlaceholder {
QString defaultTitle();
}

namespace PlotUtils {

/// Returns true when range is corrected, false when it's unchanged.
bool correctZeroRange(QCPRange& range, double safeMargin);
}

#endif // PLOT_UTILS_H
