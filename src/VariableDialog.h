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

namespace VariableDialog {

//------------------------------------------------------------------------------
/**
    The function parameters dialog that can choose one of parameters
    and set variation of this parameter and number of points for plotting.
*/
class ElementDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool show(Schema *schema, Z::Variable *var,
                     const QString& title, const QString& recentKey);
private:
    explicit ElementDlg(Schema*, Z::Variable*);

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
class TwoElemensDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool show(Schema *schema, Z::Variable *var1, Z::Variable *var2,
                     const QString& title, const QString& recentKey);
private:
    explicit TwoElemensDlg(Schema*, Z::Variable*, Z::Variable*);

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
class ElementRangeDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool show(Schema *schema, Z::Variable *var,
                     const QString& title, const QString& recentKey);
private:
    explicit ElementRangeDlg(Schema*, Z::Variable*);

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
class MultiElementRangeDlg : public RezonatorDialog
{
    Q_OBJECT

public:
    static bool show(Schema *schema/*, Z::Variable *var*/,
                     const QString& title, const QString& recentKey);
private:
    explicit MultiElementRangeDlg(Schema*/*, Z::Variable**/);

    //Z::Variable* _var;
    VariableEditor_MultiElementRange *_varEditor;

protected slots:
    void collect();
};

} // namespace VariableDialog

#endif // VARIABLE_DLG_H
