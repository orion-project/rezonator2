#ifndef ELEMENT_FORMULA_H
#define ELEMENT_FORMULA_H

#include "Element.h"

class PyRunner;

DECLARE_ELEMENT(ElemFormula, Element)
    ElemFormula();
    ~ElemFormula() override;
    DEFAULT_LABEL("C")
    CALC_MATRIX

    QString typeName() const override;
    QString customTypeName() const { return _typeName; } // for element saving
    void setTypeName(const QString &v) { _typeName = v; } // for element loading
    
    QString formula() const { return _formula; }
    void setFormula(const QString& formula) { _formula = formula; }

    void assign(const ElemFormula* other);
    
    /// Explicitly makes a locked element to recalculate its matrices
    /// In general it's done in parameter on-change handler
    /// but formula elem can have no params but still provide matrices
    /// that must be recalculated on schema loading.
    void scheduleCalcMatrix() { _calcMatrixNeeded = true; }
    
    QStringList errorLog() const { return _errorLog; }
    int errorLine() const { return _errorLine; }

    void setPrintFunc(std::function<void(const QString&)> printFunc) { _printFunc = printFunc; }
private:
    QString _typeName;
    QString _formula;
    QStringList _errorLog;
    int _errorLine;
    std::function<void(const QString&)> _printFunc;

    void showError(PyRunner *py);
DECLARE_ELEMENT_END

#endif // ELEMENT_FORMULA_H
