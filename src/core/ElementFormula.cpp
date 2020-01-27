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

void ElemFormula::addParam(Z::Parameter* param)
{
    Element::addParam(param);
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
    qWarning() << "ElemFormula::removeParam: invalid parameter, it's not in the perameters list";
}
