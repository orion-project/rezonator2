#include "VariableDialog.h"

#include "CustomPrefs.h"
#include "core/Schema.h"
#include "core/Variable.h"
#include "io/z_io_json.h"
#include "widgets/ElemSelectorWidget.h"
#include "widgets/VariableRangeWidget.h"
#include "widgets/VariableEditor.h"
#include "helpers/OriDialogs.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>

//------------------------------------------------------------------------------
//                              VariableDialog

bool VariableDialog::show(QWidget *parent, Schema *schema, Z::Variable *var,
                          const QString& title, const QString &recentKey)
{
    if (!var->element && !recentKey.isEmpty())
        Z::IO::Json::readVariablePref(CustomPrefs::recentObj(recentKey), var, schema);

    VariableDialog dialog(parent, schema, var);
    dialog.setWindowTitle(title);
    dialog.exec();
    bool ok = dialog.result() == QDialog::Accepted;

    if (ok && !recentKey.isEmpty())
        CustomPrefs::setRecentObj(recentKey, Z::IO::Json::writeVariablePref(var));
    return ok;
}

VariableDialog::VariableDialog(QWidget *parent, Schema *schema, Z::Variable *var)
    : RezonatorDialog(DontDeleteOnClose, parent), _var(var)
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

bool VariableDialog2::show(QWidget *parent, Schema *schema, Z::Variable *var1, Z::Variable *var2,
                           const QString& title, const QString &recentKey)
{
    if (!var1->element && !recentKey.isEmpty())
    {
        auto recentObj = CustomPrefs::recentObj(recentKey);
        Z::IO::Json::readVariablePref(recentObj["var1"].toObject(), var1, schema);
        Z::IO::Json::readVariablePref(recentObj["var2"].toObject(), var2, schema);
    }

    VariableDialog2 dialog(parent, schema, var1, var2);
    dialog.setWindowTitle(title);
    dialog.exec();
    bool ok = dialog.result() == QDialog::Accepted;

    if (ok && !recentKey.isEmpty())
        CustomPrefs::setRecentObj(recentKey, QJsonObject({
            { "var1", Z::IO::Json::writeVariablePref(var1) },
            { "var2", Z::IO::Json::writeVariablePref(var2) },
        }));
    return ok;
}

VariableDialog2::VariableDialog2(QWidget *parent, Schema *schema, Z::Variable *var1, Z::Variable *var2)
    : RezonatorDialog(DontDeleteOnClose, parent), _var1(var1), _var2(var2)
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
//                         VariableDialog_ElementRange

bool VariableDialog_ElementRange::show(QWidget *parent, Schema *schema, Z::Variable *var,
                                       const QString& title, const QString &recentKey)
{
    if (!var->element && !recentKey.isEmpty())
        Z::IO::Json::readVariablePref(CustomPrefs::recentObj(recentKey), var, schema);

    VariableDialog_ElementRange dialog(parent, schema, var);
    dialog.setWindowTitle(title);
    dialog.exec();
    bool ok = dialog.result() == QDialog::Accepted;

    if (ok && !recentKey.isEmpty())
        CustomPrefs::setRecentObj(recentKey, Z::IO::Json::writeVariablePref(var));
    return ok;
}

VariableDialog_ElementRange::VariableDialog_ElementRange(QWidget *parent, Schema *schema, Z::Variable *var)
    : RezonatorDialog(DontDeleteOnClose, parent), _var(var)
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

//------------------------------------------------------------------------------
//                         VariableDialog_ElementRanges

bool VariableDialog_MultiElementRange::show(QWidget *parent, Schema *schema/*, Z::Variable *var*/,
                                            const QString& title, const QString &recentKey)
{
    //if (!var->element && !recentKey.isEmpty())
      //  Z::IO::Json::readVariablePref(CustomPrefs::recentObj(recentKey), var, schema);

    VariableDialog_MultiElementRange dialog(parent, schema/*, var*/);
    dialog.setWindowTitle(title);
    dialog.exec();
    bool ok = dialog.result() == QDialog::Accepted;

    //if (ok && !recentKey.isEmpty())
      //  CustomPrefs::setRecentObj(recentKey, Z::IO::Json::writeVariablePref(var));
    return ok;
}

VariableDialog_MultiElementRange::VariableDialog_MultiElementRange(QWidget *parent, Schema *schema/*, Z::Variable *var*/)
    : RezonatorDialog(DontDeleteOnClose, parent)//, _var(var)
{
    setObjectName("VariableDialog_ElementRanges");

    _varEditor = new VariableEditor_MultiElementRange(schema);
    //_varEditor->populate(var);

    mainLayout()->addLayout(_varEditor);
    mainLayout()->addSpacing(8);
}

void VariableDialog_MultiElementRange::collect()
{
    /*auto res = _varEditor->verify();
    if (!res) return res.show(this);

    _varEditor->collect(_var);*/
    accept();
}
