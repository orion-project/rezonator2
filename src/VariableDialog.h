#ifndef VARIABLE_DLG_H
#define VARIABLE_DLG_H

#include "RezonatorDialog.h"

class Schema;
class VariableEditor;
class VariableEditor_ElementRange;
class VariableEditor_MultiElementRange;
namespace Z {
struct Variable;
}

//------------------------------------------------------------------------------
/**
    The function parameters dialog that can choose one of parameters
    and set variation of this parameter and number of points for plotting.
*/
class VariableDialog : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool show(Schema *schema, Z::Variable *var,
                     const QString& title, const QString& recentKey);
private:
    explicit VariableDialog(Schema*, Z::Variable*);

    Z::Variable* _var;
    VariableEditor* _varEditor;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------
/**
    The function parameters dialog that can choose two different parameters
    and set variation of each parameter and number of points for plotting.
*/
class VariableDialog2 : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool show(Schema *schema, Z::Variable *var1, Z::Variable *var2,
                     const QString& title, const QString& recentKey);
private:
    explicit VariableDialog2(Schema*, Z::Variable*, Z::Variable*);

    Z::Variable *_var1, *_var2;
    VariableEditor *_varEditor1, *_varEditor2;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------
/**
    The function parameters dialog that can choose one of range elements
    and set number of points for plotting inside selected element.
*/
class VariableDialog_ElementRange : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool show(Schema *schema, Z::Variable *var,
                     const QString& title, const QString& recentKey);
private:
    explicit VariableDialog_ElementRange(Schema*, Z::Variable*);

    Z::Variable* _var;
    VariableEditor_ElementRange* _varEditor;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------
/**
    The function parameters dialog that can choose several of range elements
    and set number of points for plotting inside each of selected elements.
*/
class VariableDialog_MultiElementRange : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool show(Schema *schema/*, Z::Variable *var*/,
                     const QString& title, const QString& recentKey);
private:
    explicit VariableDialog_MultiElementRange(Schema*/*, Z::Variable**/);

    //Z::Variable* _var;
    VariableEditor_MultiElementRange *_varEditor;

protected slots:
    void collect();
};

#endif // VARIABLE_DLG_H
