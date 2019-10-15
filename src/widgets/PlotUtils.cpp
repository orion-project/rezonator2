#include "PlotUtils.h"

#include "../../libs/qcustomplot/qcustomplot.h"

#include <cmath>

bool AxisLimits::isInvalid() const
{
    return std::isnan(min) or std::isnan(max);
}

QString AxisLimits::str() const
{
    return QString("%1 - %2").arg(min).arg(max);
}


namespace TitlePlaceholder {

QString defaultTitle() { return QStringLiteral("{default_title}"); }

} // namespace TitlePlaceholder


namespace PlotUtils {

bool correctZeroRange(QCPRange& range, double safeMargin)
{
    auto epsilon = std::numeric_limits<double>::epsilon();
    if (range.size() > epsilon) return false;

    // constant line at zero level
    if (qAbs(range.lower) <= epsilon)
    {
        range.lower = -1;
        range.upper = 1;
        return true;
    }

    // constant line at any level
    double delta = qAbs(range.lower) * safeMargin;
    range.lower -= delta;
    range.upper += delta;
    return true;
}

} // namespace PlotUtils
