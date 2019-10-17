#ifndef TABLEFUNCTION_H
#define TABLEFUNCTION_H

#include "FunctionBase.h"

class Schema;

class TableFunction : public FunctionBase
{
public:
    struct ColumnDef
    {
        QString titleT, titleS;
        Z::Unit unit = Z::Units::none();
    };

    enum class ResultPosition
    {
        ELEMENT,       //   ()
        LEFT,          // ->()
        RIGHT,         //   ()->
        LEFT_OUTSIDE,  // ->[   ]
        LEFT_INSIDE,   //   [-> ]
        MIDDLE,        //   [ + ]
        RIGHT_INSIDE,  //   [ ->]
        RIGHT_OUTSIDE, //   [   ]->
    };

    struct Result
    {
        Element* element;
        ResultPosition position = ResultPosition::ELEMENT;
        QVector<Z::PointTS> values;
    };

public:
    TableFunction(Schema *schema);

    /// Returns a path to function icon.
    /// Icon can be used to display in window title or menus.
    virtual const char* iconPath() const { return ""; }

    virtual void calculate() {}

    bool ok() const { return _errorText.isEmpty(); }
    const QString& errorText() const { return _errorText; }

    virtual QVector<ColumnDef> columns() const { return QVector<ColumnDef>(); }

    const QVector<Result>& results() const { return _results; }

protected:
    QString _errorText;
    QVector<Result> _results;
};

#endif // TABLEFUNCTION_H
