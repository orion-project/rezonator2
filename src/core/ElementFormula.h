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
    void setFormula(const QString& formula) { _formula = formula; }

    void assign(const ElemFormula* other);
    
    QStringList errorLog() const { return _errorLog; }
    int errorLine() const { return _errorLine; }

    void setPrintFunc(std::function<void(const QString&)> printFunc) { _printFunc = printFunc; }
private:
    QString _formula;
    QStringList _errorLog;
    int _errorLine;
    std::function<void(const QString&)> _printFunc;

    void showError(PyRunner *py);
DECLARE_ELEMENT_END

#endif // ELEMENT_FORMULA_H
