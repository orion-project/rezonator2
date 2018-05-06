#ifndef VARIABLE_DLG_H
#define VARIABLE_DLG_H

#include "RezonatorDialog.h"

#include <QVBoxLayout>

class Schema;
namespace Z {
    struct Variable;
}

class VariableEditor;
class VariableEditor_ElementRange;

//------------------------------------------------------------------------------

class VariableDialog : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit VariableDialog(QWidget*, Schema*, Z::Variable*);

private:
    Z::Variable* _var;
    VariableEditor* _varEditor;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------

class VariableDialog2 : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit VariableDialog2(QWidget*, Schema*, Z::Variable*, Z::Variable*);

private:
    Z::Variable *_var1, *_var2;
    VariableEditor *_varEditor1, *_varEditor2;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------

class VariableDialog_ElementRange : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit VariableDialog_ElementRange(QWidget*, Schema*, Z::Variable*);

private:
    Z::Variable* _var;
    VariableEditor_ElementRange* _varEditor;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------

namespace Z {
namespace Dlgs {

bool editVariable(QWidget *parent, Schema *schema, Variable *var, const QString& title, const QString& recentKey = QString());
bool editVariables(QWidget *parent, Schema *schema, Variable *var1, Variable *var2, const QString& title);
bool editVariable_ElementRange(QWidget *parent, Schema *schema, Variable *var, const QString& title);

} // namespace Dlgs
} // namespace Z


#endif // VARIABLE_DLG_H
