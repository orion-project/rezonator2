#ifndef PY_RUNNER_H
#define PY_RUNNER_H

#include <QMap>
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
    QVector<QString> funcNames;
    
    // output
    QMap<QString, QString> funcTitles;
    
    bool load();
    bool run(const QString &funcName);
    
private:
    void handleError(const QString& msg);
    
    QVector<void*> _refs;
    QMap<QString, void*> _funcRefs;
};

#endif // PY_RUNNER_H
