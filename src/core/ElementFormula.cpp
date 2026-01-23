#include "ElementFormula.h"

#include "Utils.h"
#include "LuaHelper.h"

#include <QApplication>

//------------------------------------------------------------------------------
//                             ElemFormulaImpl
//------------------------------------------------------------------------------

class ElemFormulaImpl
{
public:
    ElemFormulaImpl(ElemFormula *elem): _elem(elem)
    {
    
    }

    ~ElemFormulaImpl()
    {
        if (_lua) delete _lua;
    }
    
    QString error() const { return _error; }

    bool reopenLua()
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

    void calc()
    {
        if (_elem->_formula.isEmpty())
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
    
        auto vars = _lua->getGlobalVars();
        for (auto it = vars.cbegin(); it != vars.cend(); it++)
            _lua->removeGlobalVar(it.key());

        for (auto param : std::as_const(_elem->_params))
            _lua->setGlobalVar(param->alias(), param->value().toSi());
    
        _error = _lua->setCode(_elem->_formula);
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
        if (!getResult(results, QStringLiteral("At"), A)) return;
        if (!getResult(results, QStringLiteral("Bt"), B)) return;
        if (!getResult(results, QStringLiteral("Ct"), C)) return;
        if (!getResult(results, QStringLiteral("Dt"), D)) return;
        _elem->_matrs[MatrixKind::T].assign(A, B, C, D);
        if (!getResult(results, QStringLiteral("As"), A)) return;
        if (!getResult(results, QStringLiteral("Bs"), B)) return;
        if (!getResult(results, QStringLiteral("Cs"), C)) return;
        if (!getResult(results, QStringLiteral("Ds"), D)) return;
        _elem->_matrs[MatrixKind::S].assign(A, B, C, D);
    }
    
    void setUnity()
    {
        _elem->_matrs[MatrixKind::T].unity();
        _elem->_matrs[MatrixKind::S].unity();
    }

    bool getResult(const QMap<QString, double>& results, const QString& name, double& result)
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

private:
    Z::Lua *_lua = nullptr;
    ElemFormula *_elem;
    QString _error;
};

//------------------------------------------------------------------------------
//                               ElemFormula
//------------------------------------------------------------------------------

ElemFormula::ElemFormula()
{
    _impl = new ElemFormulaImpl(this);
}

ElemFormula::~ElemFormula()
{
    delete _impl;
}

QString ElemFormula::error() const
{
    return _impl->error();
}

bool ElemFormula::ok() const
{
    return _impl->error().isEmpty();
}

void ElemFormula::calcMatrixInternal()
{
    _impl->calc();
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
