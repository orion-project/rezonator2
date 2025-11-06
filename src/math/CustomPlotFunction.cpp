#include "CustomPlotFunction.h"

#include "../core/PyRunner.h"

#define FUNC_CALC QStringLiteral("calculate")

static int __funcCount = 0;

CustomPlotFunction::CustomPlotFunction(Schema *schema) : PlotFunction(schema)
{
    _moduleName = QString("customplot%1").arg(++__funcCount);
}

void CustomPlotFunction::showError(PyRunner *py)
{
    setError(py->errorText());
    _errorLog = py->errorLog;
    _errorLine = py->errorLine;
}

void CustomPlotFunction::showError(const QString &err)
{
    setError(err);
    _errorLog << err;
}

bool CustomPlotFunction::prepare()
{
    _errorLog.clear();
    _errorLine = 0;
    
    std::shared_ptr<PyRunner> py(new PyRunner);
    py->schema = schema();
    py->code = _code;
    py->moduleName = _moduleName;
    py->funcNames = { FUNC_CALC };
    py->printFunc = _printFunc;
    
    if (!py->load()) {
        showError(py.get());
        return false;
    }
    
    _customTitle = py->codeTitle;
    
    _runner = py;
    return true;
}

void CustomPlotFunction::unprepare()
{
    _runner.reset();
}

void CustomPlotFunction::calculate(CalculationMode calcMode)
{
    if (!prepare())
        return;

    // TODO: calc

    unprepare();
}