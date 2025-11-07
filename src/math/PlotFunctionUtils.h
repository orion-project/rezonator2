#ifndef PLOT_FUNCTION_UTILS_H
#define PLOT_FUNCTION_UTILS_H

#include <QVector>

#include "../core/Element.h"
#include "../core/Parameters.h"

struct PlotFuncDeps
{
    Elements elems;
    Z::Parameters params;
    
    bool check(Z::Parameter*) const;
    bool check(Element*) const;
    bool check(const Elements&) const;
};

#endif // PLOT_FUNCTION_UTILS_H