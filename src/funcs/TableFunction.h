#ifndef TABLEFUNCTION_H
#define TABLEFUNCTION_H

#include "FunctionBase.h"

class Schema;

class TableFunction : public FunctionBase
{
public:
    struct ColumnDef
    {
        enum { WIDTH_STRETCH = -1, WIDTH_AUTO = 0 };
        QString title, titleT, titleS;
        Z::Unit unit = Z::Units::none();
        bool isHtml = false;
        int width = WIDTH_AUTO;
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

private:
    QString _errorText;
};

#endif // TABLEFUNCTION_H
