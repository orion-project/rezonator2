#include "CustomTableFunction.h"

#include "BeamCalculator.h"
#include "CustomFuncUtils.h"
#include "RoundTripCalculator.h"
#include "../core/PyRunner.h"

#include <QApplication>

#define FUNC_COLUMNS QStringLiteral("columns")
#define FUNC_CALC QStringLiteral("calculate")
#define COL_LABEL QStringLiteral("label")
#define COL_TITLE QStringLiteral("title")
#define COL_DIM QStringLiteral("dim")
#define HELP_TOPIC "custom_table"

static int __funcCount = 0;

CustomTableFunction::CustomTableFunction(Schema *schema) : TableFunction(schema)
{
    _moduleName = QString("customtable%1").arg(++__funcCount);
}

QVector<Z::PointTS> CustomTableFunction::calculateInternal(const ResultElem &resultElem)
{
    PyRunner::ResultSpec resultSpec;
    for (const auto &col : std::as_const(_columns))
        resultSpec.insert(col.label, PyRunner::ftNumber);
        
    PyRunner::Args args {
        { PyRunner::atElement, QVariant::fromValue((void*)resultElem.elem) },
        { PyRunner::atInt, (int)resultPositionInfo(resultElem.pos).absPos },
        { PyRunner::atRoundTrip, QVariant::fromValue((void*)_beamCalc.get()) }
    };
    
    _beamCalc->setPlane(Z::T);
    auto resT = _runner->run(FUNC_CALC, args, resultSpec);
    if (!resT) {
        showError(_runner.get());
        return {};
    }
    
    _beamCalc->setPlane(Z::S);
    auto resS = _runner->run(FUNC_CALC, args, resultSpec);
    if (!resS) {
        showError(_runner.get());
        return {};
    }
    
    if (resT->isEmpty() && resS->isEmpty()) {
        // The function is not applicable for particular elem and position
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
    py->moduleName = _moduleName;
    py->funcNames = { FUNC_COLUMNS, FUNC_CALC };
    py->funcNamesOptional = { CustomFuncUtils::funcNameMeta() };
    py->printFunc = _printFunc;

    static PyRunner::ModuleProps props {
        .consts = {
            { "POS_LEFT", (int)ResultPositionAbs::LEFT },
            { "POS_BEG", (int)ResultPositionAbs::BEG },
            { "POS_MID", (int)ResultPositionAbs::MID },
            { "POS_END", (int)ResultPositionAbs::END },
            { "POS_RIGHT", (int)ResultPositionAbs::RIGHT },
        }
    };

    if (!py->load(props)) {
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

    if (!_helpTopic)
        _helpTopic = CustomFuncUtils::helpTopic(py.get(), HELP_TOPIC);

    _runner = py;
    return true;
}

void CustomTableFunction::unprepare()
{
    _runner.reset();
}

QString CustomTableFunction::helpTopic() const
{
    if (_helpTopic)
        return *_helpTopic;
    return CustomFuncUtils::helpTopic(schema(), _code, _moduleName, HELP_TOPIC);
}
