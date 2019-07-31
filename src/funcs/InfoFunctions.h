#ifndef INFO_FUNCTIONS_H
#define INFO_FUNCTIONS_H

#include <QApplication>

#include "FunctionBase.h"
#include "../core/Element.h"

class Schema;

QString matrixFuncsStyleSheet();

//------------------------------------------------------------------------------

class InfoFuncMatrix : public InfoFunction
{
public:
    InfoFuncMatrix(Schema*, Element*);
    QString calculate() override;
    QString styleSheet() const override { return matrixFuncsStyleSheet(); }
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
    QString calculate() override;
    QString styleSheet() const override { return matrixFuncsStyleSheet(); }
    FunctionState elementDeleting(Element*) override;
    FUNC_NAME(qApp->translate("Func", "Elements' Matrices"))
protected:
    Elements _elements;
};

//------------------------------------------------------------------------------

class InfoFuncMatrixMultFwd : public InfoFuncMatrices
{
public:
    InfoFuncMatrixMultFwd(Schema*, const Elements&);
    QString calculate() override;
    QString styleSheet() const override { return matrixFuncsStyleSheet(); }
    FUNC_NAME(qApp->translate("Func", "Matrix Product"))
};

class InfoFuncMatrixMultBkwd : public InfoFuncMatrixMultFwd
{
public:
    InfoFuncMatrixMultBkwd(Schema*, const Elements&);
};

//------------------------------------------------------------------------------

class InfoFuncMatrixRT : public InfoFunction
{
public:
    InfoFuncMatrixRT(Schema*, Element*);
    QString calculate() override;
    QString styleSheet() const override { return matrixFuncsStyleSheet(); }
    FunctionState elementDeleting(Element*) override;
    FUNC_NAME(qApp->translate("Func", "Round-trip Matrix"))
private:
    Element* _element;
    QString formatStability(char plane, double value);
};

//------------------------------------------------------------------------------

class InfoFuncRepetitionRate : public InfoFunction
{
public:
    InfoFuncRepetitionRate(Schema *schema) : InfoFunction(schema) {}
    QString calculate() override;
    QString helpTopic() const override { return QStringLiteral("func_reprate.html"); }
    FUNC_NAME(qApp->translate("Func", "Intermode Beats Frequency"))
    double result() const { return _result; }
private:
    double _result;
};

//------------------------------------------------------------------------------

class InfoFuncSummary : public InfoFunction
{
public:
    InfoFuncSummary(Schema *schema) : InfoFunction(schema) {}
    QString calculate() override;
    FUNC_NAME(qApp->translate("Func", "Schema Summary"))
};

#endif // INFO_FUNCTIONS_H
