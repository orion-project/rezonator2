#ifndef CUSTOM_CODE_RUNNER_H
#define CUSTOM_CODE_RUNNER_H

#include <QMap>
#include <QString>
#include <QVariant>
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
    /// A display name extracted from module docstring
    QString codeTitle;
    int errorLine = 0;
    QStringList errorLog;
    
    using Record = QHash<QString, QVariant>;
    using Records = QVector<Record>;
    using FuncResult = std::optional<Records>;
    enum FieldType { ftNumber, ftString, ftUnitDim };
    using ResultSpec = QHash<QString, FieldType>;
    enum ArgType { atElement, atBeamCalc };
    using Args = QVector<QPair<ArgType, void*>>;

    bool load();
    FuncResult run(const QString &funcName, const Args &args, const ResultSpec &resultSpec);
    QString errorText() const;
    
private:
    void handleError(const QString& msg, const QString &funcName = QString());
    
    QVector<void*> _refs;
    QMap<QString, void*> _funcRefs;
};

#endif // CUSTOM_CODE_RUNNER_H
