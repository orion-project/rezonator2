#ifndef PYRUNNER_H
#define PYRUNNER_H

#include <QString>
#include <QVector>

#include <functional>

class Schema;

class PyRunner
{
public:
    PyRunner();
    ~PyRunner();
    
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
    void handleError(const QString& msg);
    
    QVector<void*> _refs;
};

#endif // PYRUNNER_H
