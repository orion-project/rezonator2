#ifndef CUSTOM_TABLE_FUNCTION_H
#define CUSTOM_TABLE_FUNCTION_H

#include "TableFunction.h"

class CustomTableFunction : public TableFunction
{
public:
    FUNC_ALIAS("CustomTable")
    FUNC_NAME(QT_TRANSLATE_NOOP("Function Name", "Custom Table"))
    FUNC_ICON(":/toolbar/table")
    FUNC_HELP("func_custom_table.html")
    
    CustomTableFunction(Schema *schema);

    QVector<TableFunction::ColumnDef> columns() const override;
    int columnCount() const override;
    QString columnTitle(const ColumnId &id) const override;

protected:
    QVector<Z::PointTS> calculatePumpBeforeSchema(Element *elem) override;
    QVector<Z::PointTS> calculateSinglePass(Element *elem, RoundTripCalculator* calc, double ior) const override;
    QVector<Z::PointTS> calculateResonator(Element *elem, RoundTripCalculator* calc, double ior) const override;
};

#endif // CUSTOM_TABLE_FUNCTION_H
