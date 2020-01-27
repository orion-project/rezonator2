#ifndef ELEMENT_FORMULA_H
#define ELEMENT_FORMULA_H

#include "Element.h"

#include <QApplication>

DECLARE_ELEMENT(ElemFormula, Element)
    ElemFormula();
    TYPE_NAME(qApp->translate("Elements", "Formula element"))
    DEFAULT_LABEL("C")
    CALC_MATRIX
    bool hasMatricesTS() const { return _hasMatricesTS; }
    void setHasMatricesTS(bool on) { _hasMatricesTS = on; }
    QString formula() const { return _formula; }
    void setFormula(const QString& formula) { _formula = formula; }
    void removeParams();
    void addParam(Z::Parameter* param);
    void removeParam(Z::Parameter* param);
private:
    bool _hasMatricesTS = true;
    QString _formula;
DECLARE_ELEMENT_END

#endif // ELEMENT_FORMULA_H
