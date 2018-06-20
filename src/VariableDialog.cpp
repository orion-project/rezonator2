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

namespace VariableDialog {

//------------------------------------------------------------------------------
//                              VariableDlg

VariableDlg::VariableDlg(const QString& windowTitle, const QString& recentKey)
    : RezonatorDialog(DontDeleteOnClose), _recentKey(recentKey)
{
    setWindowTitle(windowTitle);
    setObjectName(recentKey);
}

bool VariableDlg::run()
{
    return exec() == QDialog::Accepted;
}

//------------------------------------------------------------------------------
//                              ElementDlg

ElementDlg::ElementDlg(Schema *schema, Z::Variable *var,
                       const QString &title, const QString &recentKey)
    : VariableDlg(title, recentKey), _var(var)
{
    if (!var->element && !recentKey.isEmpty())
        Z::IO::Json::readVariablePref(CustomPrefs::recentObj(recentKey), var, schema);

    _varEditor = new VariableEditor::ElementEd(schema);
    _varEditor->populate(var);

    mainLayout()->addLayout(_varEditor);
    mainLayout()->addSpacing(8);
    mainLayout()->addStretch();
}

void ElementDlg::collect()
{
    auto res = _varEditor->verify();
    if (!res) return res.show(this);

    _varEditor->collect(_var);
    accept();

    if (!_recentKey.isEmpty())
        CustomPrefs::setRecentObj(_recentKey, Z::IO::Json::writeVariablePref(_var));
}

//------------------------------------------------------------------------------
//                              TwoElemensDlg

TwoElemensDlg::TwoElemensDlg(Schema *schema, Z::Variable *var1, Z::Variable *var2,
                             const QString& title, const QString& recentKey)
    : VariableDlg(title, recentKey), _var1(var1), _var2(var2)
{
    if (!var1->element && !recentKey.isEmpty())
    {
        auto recentObj = CustomPrefs::recentObj(recentKey);
        Z::IO::Json::readVariablePref(recentObj["var1"].toObject(), var1, schema);
        Z::IO::Json::readVariablePref(recentObj["var2"].toObject(), var2, schema);
    }

    _varEditor1 = new VariableEditor::ElementEd(schema);
    _varEditor1->populate(var1);

    _varEditor2 = new VariableEditor::ElementEd(schema);
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

void TwoElemensDlg::collect()
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

    if (!_recentKey.isEmpty())
        CustomPrefs::setRecentObj(_recentKey, QJsonObject({
            { "var1", Z::IO::Json::writeVariablePref(_var1) },
            { "var2", Z::IO::Json::writeVariablePref(_var2) },
        }));
}

//------------------------------------------------------------------------------
//                         ElementRangeDlg

ElementRangeDlg::ElementRangeDlg(Schema *schema, Z::Variable *var,
                                 const QString& title, const QString& recentKey)
    : VariableDlg(title, recentKey), _var(var)
{
    if (!var->element && !recentKey.isEmpty())
        Z::IO::Json::readVariablePref(CustomPrefs::recentObj(recentKey), var, schema);

    _varEditor = new VariableEditor::ElementRangeEd(schema);
    _varEditor->populate(var);

    mainLayout()->addLayout(_varEditor);
    mainLayout()->addSpacing(8);
    mainLayout()->addStretch();
}

void ElementRangeDlg::collect()
{
    auto res = _varEditor->verify();
    if (!res) return res.show(this);

    _varEditor->collect(_var);
    accept();

    if (!_recentKey.isEmpty())
        CustomPrefs::setRecentObj(_recentKey, Z::IO::Json::writeVariablePref(_var));
}

//------------------------------------------------------------------------------
//                              MultiElementRangeDlg

MultiElementRangeDlg::MultiElementRangeDlg(Schema *schema, QVector<Z::Variable>& vars,
                                           const QString &title, const QString &recentKey)
    : VariableDlg(title, recentKey), _vars(vars)
{
    _varEditor = new VariableEditor::MultiElementRangeEd(schema);
    _varEditor->populateVars(vars);

    mainLayout()->addLayout(_varEditor);
    mainLayout()->addSpacing(8);
}

void MultiElementRangeDlg::collect()
{
    auto res = _varEditor->verify();
    if (!res) return res.show(this);

    _vars = _varEditor->collectVars();
    accept();
}

} // namespace VariableDialog
