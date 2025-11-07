#include "PlotFunctionV2.h"

#include "../core/Protocol.h"

PlotFunctionV2::PlotFunctionV2(Schema *schema) : FunctionBase(schema)
{
}

void PlotFunctionV2::calculate()
{
    Z_REPORT("Calc:" << name())
    _lines.clear();
    _lineIndex.clear();
    _errorText.clear();

    calculateInternal();
    
    int totalPoints = 0;
    QHash<QString, int> points;
    for (const auto &line : std::as_const(_lines)) {
        points[line.id()] = points.value(line.id()) + line.size();
        totalPoints += line.size();
    }
    for (auto it = points.cbegin(); it != points.cend(); it++)
        Z_INFO("Total points count:" << it.key() << "=" << it.value());
    if (totalPoints == 0)
        setError(qApp->translate("Calc error", "No one valid point was calculated"));
}

void PlotFunctionV2::addPoint(const QString &id, double x, double y)
{
    if (std::isnan(x) || std::isinf(x)) {
        Z_WARNING(id << "invalid x value")
        return;
    }

    int index = _lineIndex.value(id, -1);

    if (std::isnan(y) || std::isinf(y)) {
        _lineIndex.remove(id);
        if (index >= 0)
            Z_INFO(id << "line segment ended at" << x << "points count:" << _lines.at(index).size())
        return;
    }

    if (index < 0) {
        _lines.append(Line(id));
        index = _lines.size()-1;
        _lineIndex[id] = index;
        Z_INFO(id << "new line segment started at" << x)
    }
    _lines[index].append(x, y);
}
