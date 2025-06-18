#ifndef INFO_FUNCTIONS_H
#define INFO_FUNCTIONS_H

#include <QApplication>

#include "FunctionBase.h"
#include "../core/Element.h"

class Schema;

//------------------------------------------------------------------------------

class InfoFuncMatrix : public InfoFunction
{
public:
    InfoFuncMatrix(Schema*, Element*);
    QString calculateInternal() override;
    FunctionState elementDeleting(Element *elem) override;
    FUNC_NAME(qApp->translate("Func", "Element's Matrices"))
private:
    Element* _element;
};

//------------------------------------------------------------------------------

class InfoFuncMatrices : public InfoFunction
{
public:
    InfoFuncMatrices(Schema*, const Elements&);
    QString calculateInternal() override;
    FunctionState elementDeleting(Element*) override;
    FUNC_NAME(qApp->translate("Func", "Elements' Matrices"))
protected:
    Elements _elements;
};

//------------------------------------------------------------------------------

class InfoFuncMatrixMult : public InfoFuncMatrices
{
public:
    InfoFuncMatrixMult(Schema*, const Elements&);
    QString calculateInternal() override;
    FUNC_HELP("func_matrix_mult.html")
    FUNC_NAME(qApp->translate("Func", "Matrix Product"))
private:
    bool _useInvMatrs = false;
    bool _useSelectionOrder = false;
    bool _useReverseOrder = false;
    bool _useDisabledElems = false;
};

//------------------------------------------------------------------------------

class InfoFuncMatrixRT : public InfoFunction
{
public:
    InfoFuncMatrixRT(Schema*, Element*);
    QString calculateInternal() override;
    FunctionState elementDeleting(Element*) override;
    FUNC_HELP("func_rt.html")
    FUNC_NAME(qApp->translate("Func", "Round-trip Matrix"))

    static QString format(RoundTripCalculator *c, bool showElems);
    static QString formatStability(char plane, const Z::Complex& v);
private:
    Element* _element;
    bool _showElems = false;
    bool _useRefOffset = false;
    Z::Value _refOffset;
    void setRefOffset();
};

//------------------------------------------------------------------------------

class InfoFuncRepetitionRate : public InfoFunction
{
public:
    InfoFuncRepetitionRate(Schema *schema) : InfoFunction(schema) {}
    QString calculateInternal() override;
    FUNC_HELP("func_reprate.html")
    FUNC_NAME(qApp->translate("Func", "Intermode Beats Frequency"))
    double repetitonRate() const { return _repetitonRate; }
private:
    double _repetitonRate;
};

//------------------------------------------------------------------------------

class InfoFuncSummary : public InfoFunction
{
public:
    InfoFuncSummary(Schema *schema) : InfoFunction(schema) {}
    QString calculateInternal() override;
    FUNC_NAME(qApp->translate("Func", "Schema Summary"))
};

#endif // INFO_FUNCTIONS_H
