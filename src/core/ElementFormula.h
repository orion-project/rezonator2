#ifndef ELEMENT_FORMULA_H
#define ELEMENT_FORMULA_H

#include "Element.h"

#include <QApplication>

class PyRunner;

DECLARE_ELEMENT(ElemFormula, Element)
    ElemFormula();
    ~ElemFormula() override;
    TYPE_NAME(qApp->translate("Elements", "Formula element"))
    DEFAULT_LABEL("C")
    CALC_MATRIX
    QString formula() const { return _formula; }
    QString error() const { return _error; }
    bool ok() const { return _error.isEmpty(); }
    void setFormula(const QString& formula) { _formula = formula; }
    void addParam(Z::Parameter* param, int index = -1);
    void removeParam(Z::Parameter* param);
    void moveParamUp(Z::Parameter* param);
    void moveParamDown(Z::Parameter* param);
    void assign(const ElemFormula* other);
private:
    QString _formula;
    QString _error;
    QStringList _errorLog;
    int _errorLine;
    void showError(PyRunner *py);
DECLARE_ELEMENT_END

#endif // ELEMENT_FORMULA_H
