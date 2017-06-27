#include "VariableDialog.h"
#include "core/Schema.h"
#include "core/Variable.h"
#include "widgets/ElemSelectorWidget.h"
#include "widgets/VariableRangeWidget.h"
#include "widgets/VariableEditor.h"
#include "helpers/OriDialogs.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>

namespace Z {
namespace Dlgs {

bool editVariable(QWidget *parent, Schema *schema, Variable *var, const QString& title)
{
    VariableDialog dialog(parent, schema, var);
    dialog.setWindowTitle(title);
    dialog.exec();
    return dialog.result() == QDialog::Accepted;
}

bool editVariables(QWidget *parent, Schema *schema, Variable *var1, Variable *var2, const QString& title)
{
    VariableDialog2 dialog(parent, schema, var1, var2);
    dialog.setWindowTitle(title);
    dialog.exec();
    return dialog.result() == QDialog::Accepted;
}

bool editVariable_ElementRange(QWidget *parent, Schema *schema, Variable *var, const QString& title)
{
    VariableDialog_ElementRange dialog(parent, schema, var);
    dialog.setWindowTitle(title);
    dialog.exec();
    return dialog.result() == QDialog::Accepted;
}

} // namespace Dlgs
} // namespace Z

//------------------------------------------------------------------------------
//                              VariableDialog

VariableDialog::VariableDialog(QWidget *parent, Schema *schema, Z::Variable *var)
    : RezonatorDialog(parent), _var(var)
{
    setObjectName("VariableDialog");

    _varEditor = new VariableEditor(schema);
    _varEditor->populate(var);

    mainLayout()->addLayout(_varEditor);
    mainLayout()->addSpacing(8);
    mainLayout()->addStretch();
}

void VariableDialog::collect()
{
    auto res = _varEditor->verify();
    if (!res) return res.show(this);

    _varEditor->collect(_var);
    accept();
}

//------------------------------------------------------------------------------
//                              VariableDialog2

VariableDialog2::VariableDialog2(QWidget *parent, Schema *schema, Z::Variable *var1, Z::Variable *var2)
    : RezonatorDialog(parent), _var1(var1), _var2(var2)
{
    setObjectName("VariableDialog2");

    _varEditor1 = new VariableEditor(schema);
    _varEditor1->populate(var1);

    _varEditor2 = new VariableEditor(schema);
    _varEditor2->populate(var2);

    QGroupBox *groupVar1 = new QGroupBox(tr("Variable 1 (X)"));
    groupVar1->setLayout(_varEditor1);

    QGroupBox *groupVar2 = new QGroupBox(tr("Variable 2 (Y)"));
    groupVar2->setLayout(_varEditor2);

    QHBoxLayout *layoutRanges = new QHBoxLayout;
    layoutRanges->addWidget(groupVar1);
    layoutRanges->addWidget(groupVar2);

    mainLayout()->addLayout(layoutRanges);
    mainLayout()->addSpacing(8);
    mainLayout()->addStretch();
}

void VariableDialog2::collect()
{
    auto res1 = _varEditor1->verify();
    if (!res1) return res1.show(this);

    auto res2 = _varEditor2->verify();
    if (!res2) return res2.show(this);

    Z::Variable tmp1, tmp2;
    _varEditor1->collect(&tmp1);
    _varEditor2->collect(&tmp2);
    if (tmp1.element == tmp2.element && tmp1.parameter == tmp2.parameter)
        return Ori::Dlg::warning(tr("Variables must be different"));

    _varEditor1->collect(_var1);
    _varEditor2->collect(_var2);
    accept();
}

//------------------------------------------------------------------------------
//                            VariableDialog_range

VariableDialog_ElementRange::VariableDialog_ElementRange(QWidget *parent, Schema *schema, Z::Variable *var)
    : RezonatorDialog(parent), _var(var)
{
    setObjectName("VariableDialog_ElementRange");

    _varEditor = new VariableEditor_ElementRange(schema);
    _varEditor->populate(var);

    mainLayout()->addLayout(_varEditor);
    mainLayout()->addSpacing(8);
    mainLayout()->addStretch();
}

void VariableDialog_ElementRange::collect()
{
    auto res = _varEditor->verify();
    if (!res) return res.show(this);

    _varEditor->collect(_var);
    accept();
}
