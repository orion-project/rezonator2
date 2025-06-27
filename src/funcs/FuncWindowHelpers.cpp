#include "FuncWindowHelpers.h"

#include "../math/FunctionBase.h"

namespace FuncWindowHelpers {

QString makeWindowTitle(FunctionBase* func)
{
    return makeWindowTitle(func->alias(), func->name());
}

QString makeWindowTitle(const QString &funcAlias, const QString &funcName)
{
    static QMap<QString, int> windowIndeces;

    int index = windowIndeces[funcAlias];
    windowIndeces[funcAlias] = index+1;

    if (index > 0)
        return QString("%1 (%2)").arg(funcName).arg(index);
    return funcName;
}

} // namespace FuncWindowHelpers
