#include "ElementFormula.h"

#include "Utils.h"
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
    PyRunner py;
    py.code = _formula;
    py.moduleName = QString("elem_%1").arg(_id);
    py.funcNames = { FUNC_CALC_MATRIX };
    //py->printFunc = _printFunc;

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

void ElemFormula::addParam(Z::Parameter* param, int index)
{
    if (_params.indexOf(param) >= 0)
    {
        qWarning() << "ElemFormula::addParam: invalid parameter, it's already in the parameters list";
        return;
    }
    Element::addParam(param, index);
}

void ElemFormula::removeParam(Z::Parameter* param)
{
    for (int i = 0; i < _params.size(); i++)
    {
        Z::Parameter *p = _params.at(i);
        if (p == param)
        {
            _params.removeAt(i);
            delete p;
            return;
        }
    }
    qWarning() << "ElemFormula::removeParam: invalid parameter, it's not in the parameters list";
}

void ElemFormula::moveParamUp(Z::Parameter* param)
{
    int index = _params.indexOf(param);
    if (index < 0)
    {
        qWarning() << "ElemFormula::moveParamUp: invalid parameter, it's not in the parameters list";
        return;
    }
    if (index == 0)
    {
        _params.removeAt(0);
        _params.append(param);
    }
    else
        swapItems(_params, index, index-1);
}

void ElemFormula::moveParamDown(Z::Parameter* param)
{
    int index = _params.indexOf(param);
    if (index < 0)
    {
        qWarning() << "ElemFormula::moveParamDown: invalid parameter, it's not in the parameters list";
        return;
    }
    if (index == _params.size()-1)
    {
        _params.removeAt(index);
        _params.insert(0, param);
    }
    else
        swapItems(_params, index, index+1);
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
