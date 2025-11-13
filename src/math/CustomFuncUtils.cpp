#include "CustomFuncUtils.h"

#include "../core/PyRunner.h"

#include <QDebug>

namespace CustomFuncUtils {

QString helpTopic(Schema *schema, const QString &code, const QString &moduleName, const QString &defaultTopic)
{
    PyRunner py;
    py.schema = schema;
    py.code = code;
    py.moduleName = moduleName;
    py.funcNamesOptional = { funcNameMeta() };
    
    if (!py.load()) {
        qWarning() << "Failed to get help topic from custom script" << py.errorLog;
        return defaultTopic;
    }
    
    return helpTopic(&py, defaultTopic);
}

QString helpTopic(PyRunner *py, const QString &defaultTopic)
{
    if (!py->hasFunction(funcNameMeta()))
        return defaultTopic;
    
    auto res = py->run(funcNameMeta(), {}, {
        { funcResHelp(), PyRunner::ftString },
    });
    if (!res) {
        qWarning() << "Failed to get help topic from custom script" << py->errorLog;
        return defaultTopic;
    }
    if (res->isEmpty())
        return defaultTopic;
    
    return res->first()[funcResHelp()].toString();
}

} // namespace CustomFuncUtils
