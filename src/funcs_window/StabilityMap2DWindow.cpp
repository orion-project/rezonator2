#include "StabilityMap2DWindow.h"
#include "../VariableDialog.h"

StabilityMap2DWindow::StabilityMap2DWindow(Schema *schema) :
    PlotFuncWindowStorable(new StabilityMap2DFunction(schema))
{
    setTitleAndIcon(function()->name(), ":/toolbar/func_stab_map_2d");
}

bool StabilityMap2DWindow::configure(QWidget* parent)
{
    return Z::Dlgs::editVariables(parent, schema(), function()->paramX(), function()->paramY(),
        tr("Contour Stability Map Parameters"));
}
