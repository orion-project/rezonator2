#ifndef FUNC_WINDOW_HELPERS_H
#define FUNC_WINDOW_HELPERS_H

#include <QString>

class FunctionBase;

namespace FuncWindowHelpers {

QString makeWindowTitle(FunctionBase* func);
QString makeWindowTitle(const QString &funcAlias, const QString &funcName);

} // namespace FuncWindowHelpers

#endif // FUNC_WINDOW_HELPERS_H
