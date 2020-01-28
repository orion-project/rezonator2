#include "ElementFormula.h"

ElemFormula::ElemFormula()
{

}

void ElemFormula::calcMatrixInternal()
{

}

void ElemFormula::removeParams()
{
    qDeleteAll(_params);
    _params.clear();
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
    int index1 = (index == 0) ? _params.size()-1 : index-1;
    _params.swap(index, index1);
}

void ElemFormula::moveParamDown(Z::Parameter* param)
{
    int index = _params.indexOf(param);
    if (index < 0)
    {
        qWarning() << "ElemFormula::moveParamDown: invalid parameter, it's not in the parameters list";
        return;
    }
    int index1 = (index == _params.size()-1) ? 0 : index+1;
    _params.swap(index, index1);
}
