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
    Schema *schema = nullptr;
    QString code;
    /// Top level functions, expected to exists in the code module
    QVector<QString> funcNames;
    /// Implementation of Z.print() function
    std::function<void(const QString&)> printFunc;
    
    // output
    int errorLine = 0;
    QStringList errorLog;
    /// Function display names extracted from docstrings
    QMap<QString, QString> funcTitles;
    
    bool load();
    bool run(const QString &funcName);
    
    QString errorText() const;
    
private:
    void handleError(const QString& msg);
    
    QVector<void*> _refs;
    QMap<QString, void*> _funcRefs;
};

#endif // PY_RUNNER_H
