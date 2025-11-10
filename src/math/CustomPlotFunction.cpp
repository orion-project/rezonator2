#include "CustomPlotFunction.h"

#include "../core/PyRunner.h"

#define FUNC_FIGURE QStringLiteral("figure")
#define FUNC_CALC QStringLiteral("calculate")
#define PROP_X_DIM QStringLiteral("x_dim")
#define PROP_Y_DIM QStringLiteral("y_dim")
#define PROP_X_TITLE QStringLiteral("x_title")
#define PROP_Y_TITLE QStringLiteral("y_title")
#define RES_LABEL QStringLiteral("label")
#define RES_X QStringLiteral("x")
#define RES_Y QStringLiteral("y")

static int __funcCount = 0;

CustomPlotFunction::CustomPlotFunction(Schema *schema) : PlotFunctionV2(schema)
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
        { PROP_X_TITLE, PyRunner::ftStringOptional },
        { PROP_Y_DIM, PyRunner::ftUnitDim },
        { PROP_Y_TITLE, PyRunner::ftStringOptional },
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
    _titleX = fig[PROP_X_TITLE].toString(),
    _titleY = fig[PROP_Y_TITLE].toString(),
    
    _runner = py;
    return true;
}

void CustomPlotFunction::unprepare()
{
    _runner.reset();
}

void CustomPlotFunction::calculateInternal()
{
    auto res = _runner->run(FUNC_CALC, {}, {
        { RES_LABEL, PyRunner::ftString },
        { RES_X, PyRunner::ftNumberArray },
        { RES_Y, PyRunner::ftNumberArray },
    });
    if (!res) {
        showError(_runner.get());
        return;
    }
    
    // It's safe to get fields by [] because PyRunner ensures that
    // all fields mentioned in result-spec are present in the result
    for (const auto& line: std::as_const(res.value()))
    {
        auto id = line[RES_LABEL].toString();
        auto x = line[RES_X].value<QVector<double>>();
        auto y = line[RES_Y].value<QVector<double>>();
        if (x.size() != y.size()) {
            showError("Lengths of X and Y arrays do not match");
            return;
        }
        int cnt = x.size();
        for (int i = 0; i < cnt; i++)
            addPoint(id, x.at(i), y.at(i));
        endLine(id);
    }
}
