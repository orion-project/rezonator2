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
    
    void foo();
    void foo1();
    void foo2();
    void foo3(Schema *schema);
    void foo4(Schema *schema, const QString &code);
    
    Logger log;

private:
    void handleError();
};

#endif // PYHELPER_H
