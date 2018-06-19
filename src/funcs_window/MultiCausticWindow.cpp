#include "MultiCausticWindow.h"

MultiCausticWindow::MultiCausticWindow(Schema *schema): PlotFuncWindowStorable(new MultiCausticFunction(schema))
{
    setTitleAndIcon(function()->name(), ":/toolbar/func_multi_caustic");
}
