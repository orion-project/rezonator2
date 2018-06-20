#include "StabilityMap2DWindow.h"

#include "../VariableDialog.h"

StabilityMap2DWindow::StabilityMap2DWindow(Schema *schema) :
    PlotFuncWindowStorable(new StabilityMap2DFunction(schema))
{
    setTitleAndIcon(function()->name(), ":/toolbar/func_stab_map_2d");
}

bool StabilityMap2DWindow::configureInternal()
{
    return VariableDialog2::show(schema(), function()->paramX(), function()->paramY(),
        tr("Contour Stability Map Parameters"), "func_stab_map_2d");
}
