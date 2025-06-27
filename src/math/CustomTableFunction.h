#ifndef CUSTOM_TABLE_FUNCTION_H
#define CUSTOM_TABLE_FUNCTION_H

#include "TableFunction.h"

class CustomTableFunction : public TableFunction
{
public:
    FUNC_ALIAS("CustomTable")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Custom Table"))
    FUNC_ICON(":/toolbar/table_py")
    FUNC_HELP("func_custom_table.html")
    
    CustomTableFunction(Schema *schema);

    QVector<TableFunction::ColumnDef> columns() const override;
    int columnCount() const override;
    QString columnTitle(const ColumnId &id) const override;
    
    QString code() const { return _code; }
    void setCode(const QString &code) { _code = code; }
    
    bool prepare() override;
    
    QStringList errorLog() const { return _errorLog; }
    int errorLine() const { return _errorLine; }
    
    void setPrintFunc(std::function<void(const QString&)> printFunc) { _printFunc = printFunc; }

protected:
    QVector<Z::PointTS> calculatePumpBeforeSchema(Element *elem) override;
    QVector<Z::PointTS> calculateSinglePass(Element *elem, RoundTripCalculator* calc, double ior) const override;
    QVector<Z::PointTS> calculateResonator(Element *elem, RoundTripCalculator* calc, double ior) const override;
    
private:
    QString _code;
    QStringList _errorLog;
    int _errorLine;
    std::function<void(const QString&)> _printFunc;
};

#endif // CUSTOM_TABLE_FUNCTION_H
