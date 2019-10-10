#include "MultibeamCausticFunction.h"

#include "../core/Schema.h"

void MultibeamCausticFunction::calculate()
{
    if (!_schema->isSP())
    {
         setError(qApp->translate("Calc error", "This function can only operate on SP schema"));
        return;
    }

    MultirangeCausticFunction::calculate();
}
