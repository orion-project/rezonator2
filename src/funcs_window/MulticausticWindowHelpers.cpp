#include "MulticausticWindowHelpers.h"

#include "InfoFuncWindow.h"
#include "../funcs/CausticFunction.h"
#include "../funcs/PlotFuncRoundTripFunction.h"
#include "../../libs/qcustomplot/qcpcursor.h"

namespace MulticausticWindowHelpers {

void showRoundTrip(Z::Unit unitX, QCPCursor* cursor, const QList<CausticFunction*>& funcs, const QString& baseTitle)
{
    auto currentX = unitX->toSi(cursor->position().x());
    double prevOffset = 0;
    for (auto func : funcs)
    {
        double nextOffset = prevOffset + func->arg()->range.stop.toSi();
        if (currentX >= prevOffset && currentX < nextOffset)
        {
            QString funcTitle = QString("%1 (inside of %2)").arg(baseTitle, func->arg()->element->displayLabel());
            InfoFuncWindow::open(new PlotFuncRoundTripFunction(funcTitle, func));
            return;
        }
        prevOffset = nextOffset;
    }
}

} // namespace MulticausticWindowHelpers
