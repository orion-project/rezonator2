#ifndef WINDOW_STABILITY_MAP_2D_H
#define WINDOW_STABILITY_MAP_2D_H

#include "PlotFuncWindowStorable.h"
#include "../funcs_meat/StabilityMap2DFunction.h"

class StabilityMap2DWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit StabilityMap2DWindow(Schema*);

    bool configure(QWidget* parent) override;

    StabilityMap2DFunction* function() const { return (StabilityMap2DFunction*)_function; }
};

#endif // WINDOW_STABILITY_MAP_2D_H
