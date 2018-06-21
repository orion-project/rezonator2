#include "MultiCausticWindow.h"

#include "../VariableDialog.h"
#include "../funcs/CausticFunction.h"
#include "../io/z_io_utils.h"
#include "../io/z_io_json.h"
#include "../widgets/Plot.h"

MultiCausticWindow::MultiCausticWindow(Schema *schema): PlotFuncWindowStorable(new MultiCausticFunction(schema))
{
    setTitleAndIcon(function()->name(), ":/toolbar/func_multi_caustic");
}

bool MultiCausticWindow::configureInternal()
{
    auto args = function()->args();
    bool ok = VariableDialog::MultiElementRangeDlg(schema(), args, tr("Ranges"), "func_multi_caustic").run();
    if (ok)
        function()->setArgs(args);
    return ok;
}

void MultiCausticWindow::fillGraphWithFunctionResults(Z::WorkPlane plane, Graph *graph, int resultIndex)
{
    int index1 = 0;
    double offset = 0;
    for (CausticFunction* func : function()->funcs())
    {
        int index2 = index1 + func->resultCount(plane);
        if (resultIndex >= index1 && resultIndex < index2)
        {
            auto result = func->result(plane, resultIndex - index1);
            for (int i = 0; i < result.x.size(); i++)
                graph->addData(result.x.at(i) + offset, result.y.at(i));
            return;
        }
        index1 = index2;
        offset += func->argumentUnit()->fromSi(func->arg()->range.stop.toSi());
    }
}
