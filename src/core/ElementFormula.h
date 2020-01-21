#ifndef ELEMENT_FORMULA_H
#define ELEMENT_FORMULA_H

#include "Element.h"

#include <QApplication>

DECLARE_ELEMENT(ElemFormula, Element)
    ElemFormula();
    TYPE_NAME(qApp->translate("Elements", "Formula element"))
    DEFAULT_LABEL("C")
    CALC_MATRIX
private:
    QString _code;
DECLARE_ELEMENT_END

#endif // ELEMENT_FORMULA_H
