#include "ElementFormula.h"

#include "../core/PyRunner.h"

#include <QApplication>

#define FUNC_CALC_MATRIX QStringLiteral("calc_matrix")
#define RES_MT QStringLiteral("Mt")
#define RES_MS QStringLiteral("Ms")

ElemFormula::ElemFormula()
{
}

ElemFormula::~ElemFormula()
{
}

void ElemFormula::calcMatrixInternal()
{
    _error.clear();
    _errorLog.clear();
    _errorLine = 0;

    PyRunner py;
    py.code = _formula;
    py.moduleName = QString("elem_%1").arg(_id);
    py.funcNames = { FUNC_CALC_MATRIX };
    py.printFunc = _printFunc;

    if (!py.load({})) {
        showError(&py);
        return;
    }

    PyRunner::Args args {
        { PyRunner::atElement, QVariant::fromValue((void*)this) },
    };

    auto res = py.run(FUNC_CALC_MATRIX, args, {
        { RES_MT, PyRunner::ftMatrix },
        { RES_MS, PyRunner::ftMatrix },
    });
    if (!res) {
        showError(&py);
        return;
    }

    const auto &matrixMap = res->first();
    _matrs[MatrixKind::T] = matrixMap[RES_MT].value<Z::Matrix>();
    _matrs[MatrixKind::S] = matrixMap[RES_MS].value<Z::Matrix>();
}

void ElemFormula::showError(PyRunner *py)
{
    _error = py->errorText();
    _errorLog = py->errorLog;
    _errorLine = py->errorLine;
    for (auto it = _matrs.begin(); it != _matrs.end(); it++)
        it->second.unity();
}

void ElemFormula::assign(const ElemFormula* other)
{
    qDeleteAll(_params);
    _params.clear();

    for (const auto p : other->params())
    {
        auto paramCopy = new Z::Parameter(p->dim(),
                                          p->alias(),
                                          p->label(),
                                          p->name(),
                                          p->description(),
                                          p->category(),
                                          p->visible());
        paramCopy->setValue(p->value());
        addParam(paramCopy);
    }
    _formula = other->formula();
}
