#include "CustomPlotFunction.h"

#include "../core/PyRunner.h"

#define FUNC_FIGURE QStringLiteral("figure")
#define FUNC_CALC QStringLiteral("calculate")
#define PROP_X_DIM QStringLiteral("x_dim")
#define PROP_Y_DIM QStringLiteral("y_dim")

static int __funcCount = 0;

CustomPlotFunction::CustomPlotFunction(Schema *schema) : PlotFunction(schema)
{
    _moduleName = QString("customplot%1").arg(++__funcCount);
}

QString CustomPlotFunction::name() const
{
    if (_customTitle.isEmpty())
        return QT_TRANSLATE_NOOP("Function Name", "Custom Plot");
    return _customTitle;
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
    py->funcNames = { FUNC_FIGURE, FUNC_CALC };
    py->printFunc = _printFunc;
    
    if (!py->load()) {
        showError(py.get());
        return false;
    }
    
    _customTitle = py->codeTitle;
    
    auto res = py->run(FUNC_FIGURE, {}, {
        { PROP_X_DIM, PyRunner::ftUnitDim },
        { PROP_Y_DIM, PyRunner::ftUnitDim },
    });
    if (!res) {
        showError(py.get());
        return false;
    }
    if (res->size() != 1) {
        showError(FUNC_FIGURE + ": bad result");
        return false;
    }
    auto fig = res->first();
    _dimX = fig[PROP_X_DIM].value<Z::Dim>();
    _dimY = fig[PROP_Y_DIM].value<Z::Dim>();
    
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