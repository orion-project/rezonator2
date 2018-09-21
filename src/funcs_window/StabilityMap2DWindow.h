#ifndef WINDOW_STABILITY_MAP_2D_H
#define WINDOW_STABILITY_MAP_2D_H

#include "PlotFuncWindowStorable.h"
#include "../funcs/StabilityMap2DFunction.h"

class StabilityMap2DWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit StabilityMap2DWindow(Schema*);

    StabilityMap2DFunction* function() const { return dynamic_cast<StabilityMap2DFunction*>(_function); }

protected:
    // Implementation of PlotFuncWindow
    bool configureInternal() override;
};

#endif // WINDOW_STABILITY_MAP_2D_H
