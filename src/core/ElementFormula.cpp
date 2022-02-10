#include "ElementFormula.h"

#include "Utils.h"
#include "LuaHelper.h"

#include <QApplication>

ElemFormula::ElemFormula()
{
}

ElemFormula::~ElemFormula()
{
    if (_lua) delete _lua;
}

bool ElemFormula::reopenLua()
{
    if (_lua) delete _lua;

    _lua = new Z::Lua;
    _error = _lua->open();
    if (!_error.isEmpty())
    {
        delete _lua;
        _lua = nullptr;
        return false;
    }
    return true;
}

void ElemFormula::reset()
{
    if (!_lua) return;

    for (auto var : _lua->getGlobalVars().keys())
        _lua->removeGlobalVar(var);
}

void ElemFormula::calcMatrixInternal()
{
    if (_formula.isEmpty())
    {
        _error = qApp->translate("ElemFormula", "Formula is empty");
        setUnity();
        return;
    }

    if (!_lua && !reopenLua())
    {
        setUnity();
        return;
    }

    for (auto param : _params)
        _lua->setGlobalVar(param->alias(), param->value().toSi());

    _error = _lua->setCode(_formula);
    if (!_error.isEmpty())
    {
        setUnity();
        return;
    }

    _error = _lua->execute();
    if (!_error.isEmpty())
    {
        setUnity();
        return;
    }

    double A, B, C, D;

    auto results = _lua->getGlobalVars();
    if (_hasMatricesTS)
    {
        if (!getResult(results, QStringLiteral("At"), A)) return;
        if (!getResult(results, QStringLiteral("Bt"), B)) return;
        if (!getResult(results, QStringLiteral("Ct"), C)) return;
        if (!getResult(results, QStringLiteral("Dt"), D)) return;
        _mt.assign(A, B, C, D);
        if (!getResult(results, QStringLiteral("As"), A)) return;
        if (!getResult(results, QStringLiteral("Bs"), B)) return;
        if (!getResult(results, QStringLiteral("Cs"), C)) return;
        if (!getResult(results, QStringLiteral("Ds"), D)) return;
        _ms.assign(A, B, C, D);
    }
    else
    {
        if (!getResult(results, QStringLiteral("A"), A)) return;
        if (!getResult(results, QStringLiteral("B"), B)) return;
        if (!getResult(results, QStringLiteral("C"), C)) return;
        if (!getResult(results, QStringLiteral("D"), D)) return;
        _mt.assign(A, B, C, D);
        _ms.assign(A, B, C, D);
    }
}

bool ElemFormula::getResult(const QMap<QString, double>& results, const QString& name, double& result)
{
    if (!results.contains(name))
    {
        _error = qApp->translate("ElemFormula", "Formula doesn't contain an expression for '%1' or it is not a number").arg(name);
        setUnity();
        return false;
    }
    result = results[name];
    return true;
}

void ElemFormula::setUnity()
{
    _mt.unity();
    _ms.unity();
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
    _hasMatricesTS = other->hasMatricesTS();
}
