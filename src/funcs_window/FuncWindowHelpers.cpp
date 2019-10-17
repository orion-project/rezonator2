#include "FuncWindowHelpers.h"

#include "../funcs/FunctionBase.h"

namespace FuncWindowHelpers {

QString makeWindowTitle(FunctionBase* func)
{
    static QMap<QString, int> windowIndeces;

    int index = windowIndeces[func->alias()];
    windowIndeces[func->alias()] = index+1;

    if (index > 0)
        return QString("%1 (%2)").arg(func->name()).arg(index);
    return func->name();
}

} // namespace FuncWindowHelpers
