#include "BeamOverStabWindow.h"

BeamOverStabWindow::BeamOverStabWindow(Schema *schema)
    : PlotFuncWindowStorable(new BeamOverStabFunction(schema))
{

}

bool BeamOverStabWindow::configureInternal()
{
    return true;
}
