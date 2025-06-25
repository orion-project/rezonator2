#ifndef PYHELPER_H
#define PYHELPER_H

#include <QString>

#include <functional>

class Schema;

class PyHelper
{
public:
    PyHelper();
    
    struct Logger {
        std::function<void(const QString&)> info;
        std::function<void(const QString&)> error;
    };
    Logger log;
    
    void run(Schema *schema, const QString &code, const QString &moduleName);
    
private:
    void handleError();
};

#endif // PYHELPER_H
