#ifndef CUSTOM_CODE_RUNNER_H
#define CUSTOM_CODE_RUNNER_H

#include <QMap>
#include <QString>
#include <QVariant>
#include <QVector>

#include <functional>
#ifdef Q_OS_LINUX
#include <optional>
#endif

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
    /// A name of python module should be dfferent for different custom functions
    /// Otherwise they will see objects defined in another custom functions
    QString moduleName;
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

    /// Types of fields in structs that are returned from the py funcs
    enum FieldType { ftNumber, ftNumberArray, ftString, ftStringOptional, ftUnitDim };
    using ResultSpec = QHash<QString, FieldType>;

    /// Types of arguments that are passed to the py funcs
    enum ArgType { atElement, atRoundTrip, atInt };
    using Args = QVector<QPair<ArgType, QVariant>>;

    struct ModuleProps
    {
        /// Constant that should be added to a custom module.
        /// They only make sense for particular module type (like table function's POS_*)
        /// and there is no need to make them globals (like Z.PLANE_*)
        QHash<const char*, int> consts;
    };

    /// Load custom code module
    bool load(const ModuleProps &props = {});
    
    /// Call a function defined in python code, e.g. `calculate`
    /// Arguments are given in generic form and converted into a proper python type before calling
    FuncResult run(const QString &funcName, const Args &args, const ResultSpec &resultSpec);

    QString errorText() const;
    
    struct TmpRef
    {
        QString id;
        void *ref;
    };

private:
    void handleError(const QString& msg, const QString &funcName = QString());
    
    QVector<TmpRef> _refs;
    QHash<QString, void*> _funcRefs;
};

#endif // CUSTOM_CODE_RUNNER_H
