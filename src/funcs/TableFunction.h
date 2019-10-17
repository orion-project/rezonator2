#ifndef TABLEFUNCTION_H
#define TABLEFUNCTION_H

#include "FunctionBase.h"

class Schema;

class TableFunction : public FunctionBase
{
public:
    TableFunction(Schema *schema);

    /// Returns a path to function icon.
    /// Icon can be used to display in window title or menus.
    virtual const char* iconPath() const { return ""; }
};

#endif // TABLEFUNCTION_H
