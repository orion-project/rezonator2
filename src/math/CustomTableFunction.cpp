#include "CustomTableFunction.h"

#include "BeamCalculator.h"
#include "RoundTripCalculator.h"
#include "../core/PyRunner.h"

#include <QApplication>

#define FUNC_COLUMNS QStringLiteral("describe_columns")
#define FUNC_CALC QStringLiteral("calculate")
#define COL_LABEL QStringLiteral("label")
#define COL_TITLE QStringLiteral("title")
#define COL_DIM QStringLiteral("dim")

CustomTableFunction::CustomTableFunction(Schema *schema) : TableFunction(schema)
{
}

QVector<Z::PointTS> CustomTableFunction::calculateInternal(const ResultElem &resultElem)
{
    PyRunner::ResultSpec resultSpec;
    for (const auto &col : std::as_const(_columns))
        resultSpec.insert(col.label, PyRunner::ftNumber);
    
    _beamCalc->setPlane(Z::T);
    auto resT = _runner->run(FUNC_CALC, {
        { PyRunner::atElement, resultElem.elem },
        { PyRunner::atRoundTrip, _beamCalc.get() }
    }, resultSpec);
    if (!resT) {
        showError(_runner.get());
        return {};
    }
    
    _beamCalc->setPlane(Z::S);
    auto resS = _runner->run(FUNC_CALC, {
        { PyRunner::atElement, resultElem.elem },
        { PyRunner::atRoundTrip, _beamCalc.get() }
    }, resultSpec);
    if (!resS) {
        showError(_runner.get());
        return {};
    }
    
    QVector<Z::PointTS> res;
    for (const auto &col : std::as_const(_columns)) {
        res << Z::PointTS(
            resT->value(0).value(col.label, qQNaN()).toDouble(),
            resS->value(0).value(col.label, qQNaN()).toDouble()
        );
    }
    
    return res;
}

void CustomTableFunction::showError(PyRunner *py)
{
    setError(py->errorText());
    _errorLog = py->errorLog;
    _errorLine = py->errorLine;
}

void CustomTableFunction::showError(const QString &err)
{
    setError(err);
    _errorLog << err;
}

bool CustomTableFunction::prepare()
{
    _errorLog.clear();
    _errorLine = 0;
    
    std::shared_ptr<PyRunner> py(new PyRunner);
    py->schema = schema();
    py->code = _code;
    py->funcNames = { FUNC_COLUMNS, "calculate" };
    py->printFunc = _printFunc;

    if (!py->load()) {
        showError(py.get());
        return false;
    }
    
    _customTitle = py->codeTitle;
    
    auto res = py->run(FUNC_COLUMNS, {}, {
        { COL_LABEL, PyRunner::ftString },
        { COL_TITLE, PyRunner::ftString },
        { COL_DIM, PyRunner::ftUnitDim }
    });
    if (!res) {
        showError(py.get());
        return false;
    }
    
    QVector <ColumnDef> columns;
    for (const auto &dict : std::as_const(*res)) {
        ColumnDef col {
            .label = dict[COL_LABEL].toString(),
            .title = dict[COL_TITLE].toString(),
            .dim = dict[COL_DIM].value<Z::Dim>(),
        };
        for (const auto &c : columns)
            if (c.label == col.label) {
                showError("Duplicated column label " + col.label);
                return false;
            }
        columns << col;
    }
    _columns = columns;

    _runner = py;
    return true;
}

void CustomTableFunction::unprepare()
{
    _runner.reset();
}