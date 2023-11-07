#include "BeamParamsAtElemsWindow.h"

BeamParamsAtElemsWindow::BeamParamsAtElemsWindow(Schema* schema) :
    TableFuncWindow(new BeamParamsAtElemsFunction(schema))
{
}
