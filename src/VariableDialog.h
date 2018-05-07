#ifndef VARIABLE_DLG_H
#define VARIABLE_DLG_H

#include "RezonatorDialog.h"

class Schema;
namespace Z {
    struct Variable;
}

//------------------------------------------------------------------------------

class VariableDialog : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit VariableDialog(QWidget*, Schema*, Z::Variable*);

    static bool show(QWidget *parent, Schema *schema, Z::Variable *var,
                     const QString& title, const QString& recentKey = QString());
private:
    Z::Variable* _var;
    class VariableEditor* _varEditor;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------

class VariableDialog2 : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit VariableDialog2(QWidget*, Schema*, Z::Variable*, Z::Variable*);

    static bool show(QWidget *parent, Schema *schema, Z::Variable *var1, Z::Variable *var2,
                     const QString& title, const QString& recentKey = QString());
private:
    Z::Variable *_var1, *_var2;
    class VariableEditor *_varEditor1, *_varEditor2;

protected slots:
    void collect();
};

//------------------------------------------------------------------------------

class VariableDialog_ElementRange : public RezonatorDialog
{
    Q_OBJECT

public:
    explicit VariableDialog_ElementRange(QWidget*, Schema*, Z::Variable*);

    static bool show(QWidget *parent, Schema *schema, Z::Variable *var,
                     const QString& title, const QString& recentKey = QString());
private:
    Z::Variable* _var;
    class VariableEditor_ElementRange* _varEditor;

protected slots:
    void collect();
};

#endif // VARIABLE_DLG_H
