#ifndef CUSTOM_FUNC_UTILS_H
#define CUSTOM_FUNC_UTILS_H

#include <QString>

class Schema;
class PyRunner;

namespace CustomFuncUtils
{

inline QString funcNameCalculate() { return QStringLiteral("calculate"); }
inline QString funcNameColumns() { return QStringLiteral("columns"); }
inline QString funcNameFigure() { return QStringLiteral("figure"); }
inline QString funcNameMeta() { return QStringLiteral("meta"); }
inline QString funcResHelp() { return QStringLiteral("help_topic"); }

QString helpTopic(Schema *schema, const QString &code, const QString &moduleName, const QString &defaultTopic);
QString helpTopic(PyRunner *py, const QString &defaultTopic);

}

#endif // CUSTOM_FUNC_UTILS_H