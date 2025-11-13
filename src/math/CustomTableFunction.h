#ifndef CUSTOM_TABLE_FUNCTION_H
#define CUSTOM_TABLE_FUNCTION_H

#include "TableFunction.h"

class PyRunner;

class CustomTableFunction : public TableFunction
{
public:
    FUNC_ALIAS("CustomTable")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Custom Table"))
    FUNC_ICON(":/toolbar/table_py")
    QString helpTopic() const override;
    
    CustomTableFunction(Schema *schema);

    QString code() const { return _code; }
    void setCode(const QString &code) { _code = code; }
    
    bool prepare() override;
    void unprepare() override;
    
    QString customTitle() const { return _customTitle; }
    QString moduleName() const { return _moduleName; }
    QStringList errorLog() const { return _errorLog; }
    int errorLine() const { return _errorLine; }
    
    void setPrintFunc(std::function<void(const QString&)> printFunc) { _printFunc = printFunc; }

protected:
    QVector<Z::PointTS> calculateInternal(const ResultElem &resultElem) override;
    
private:
    QString _code;
    QString _customTitle;
    QString _moduleName;
    std::optional<QString> _helpTopic;
    QStringList _errorLog;
    int _errorLine;
    std::function<void(const QString&)> _printFunc;
    std::shared_ptr<PyRunner> _runner;
    
    void showError(PyRunner *py);
    void showError(const QString &err);
};

#endif // CUSTOM_TABLE_FUNCTION_H
