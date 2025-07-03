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
    FUNC_HELP("func_custom_table.html")
    
    CustomTableFunction(Schema *schema);

    QString code() const { return _code; }
    void setCode(const QString &code) { _code = code; }
    
    bool prepare() override;
    void unprepare() override;
    
    QStringList errorLog() const { return _errorLog; }
    int errorLine() const { return _errorLine; }
    
    void setPrintFunc(std::function<void(const QString&)> printFunc) { _printFunc = printFunc; }
    
    QString customTitle() const { return _customTitle; }

protected:
    QVector<Z::PointTS> calculatePumpBeforeSchema(Element *elem) override;
    QVector<Z::PointTS> calculateSinglePass(Element *elem, RoundTripCalculator* calc, double ior) override;
    QVector<Z::PointTS> calculateResonator(Element *elem, RoundTripCalculator* calc, double ior) override;
    
private:
    QString _code;
    QString _customTitle;
    QStringList _errorLog;
    int _errorLine;
    std::function<void(const QString&)> _printFunc;
    std::shared_ptr<PyRunner> _runner;
    
    void showError(PyRunner *py);
    void showError(const QString &err);
};

#endif // CUSTOM_TABLE_FUNCTION_H
