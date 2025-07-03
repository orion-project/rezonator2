#include "CustomTableFunction.h"

#include "RoundTripCalculator.h"
#include "../core/PyRunner.h"

#include <QApplication>

CustomTableFunction::CustomTableFunction(Schema *schema) : TableFunction(schema)
{
}

QVector<Z::PointTS> CustomTableFunction::calculatePumpBeforeSchema(Element *elem)
{
    Q_UNUSED(elem)

    return {};
}

QVector<Z::PointTS> CustomTableFunction::calculateSinglePass(Element *elem, RoundTripCalculator* calc, double ior) const
{
    Q_UNUSED(elem)
    Q_UNUSED(calc)
    Q_UNUSED(ior)

    return {};
}

QVector<Z::PointTS> CustomTableFunction::calculateResonator(Element *elem, RoundTripCalculator *calc, double ior) const
{
    Q_UNUSED(elem)
    Q_UNUSED(calc)
    Q_UNUSED(ior)
    
    return {};
}

bool CustomTableFunction::prepare()
{
    _errorLog.clear();
    _errorLine = 0;

    PyRunner py;
    py.schema = schema();
    py.code = _code;
    py.funcNames = { "describe_columns" };
    py.printFunc = _printFunc;

    if (!py.load()) {
        setError(py.errorText());
        _errorLog = py.errorLog;
        _errorLine = py.errorLine;
        return false;
    }
    
    _customTitle = py.codeTitle;
    
    auto res = py.run("describe_columns", {
        { "label", PyRunner::ftString },
        { "title", PyRunner::ftString },
        { "dim", PyRunner::ftUnitDim }
    });
    if (!res) {
        setError(py.errorText());
        _errorLog = py.errorLog;
        _errorLine = py.errorLine;
        return false;
    }
    
    QVector <ColumnDef> colDefs;
    for (const auto &col : std::as_const(*res)) {
        ColumnDef colDef {
            .label = col["label"].toString(),
            .title = col["title"].toString(),
            .dim = col["dim"].value<Z::Dim>(),
        };
        for (const auto &d : colDefs)
            if (d.label == colDef.label) {
                QString err = "Duplicated column label " + d.label;
                _errorLog << err;
                setError(err);
                return false;
            }
        colDefs << colDef;
    }
    _columns = colDefs;

    return true;
}
