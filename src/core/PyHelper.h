#ifndef PYHELPER_H
#define PYHELPER_H

#include <QString>

#include <functional>

class Schema;

class PyHelper
{
public:
    PyHelper();
    
    // input
    std::function<void(const QString&)> logInfo;
    std::function<void(const QString&)> logError;
    Schema *schema;
    QString code;
    QString moduleName;
    QString funcName;
    
    // output
    QString funcTitle;
    
    void run();
    
private:
    void handleError();
};

#endif // PYHELPER_H
