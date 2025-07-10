#include "ParamEditorEx.h"

#include "FormulaEditor.h"
#include "ParamEditor.h"
#include "../app/Appearance.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"

#include <QDebug>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>

using namespace Ori::Layouts;

ParamEditorEx::ParamEditorEx(Z::Parameter *param, Z::Formulas *formulas, Z::Parameters *globalParams, QWidget *parent)
    : QWidget(parent), _param(param), _formulas(formulas), _globalParams(globalParams)
{
    _tmpParam = new Z::Parameter(param->dim(), param->alias());
    _tmpParam->setValue(param->value());
    _tmpParam->setExpr(param->expr());

    _formula = _formulas->get(_param);
    _hasFormula = _formula;

    auto menu = new QMenu(this);
    _actnAddFormula = menu->addAction(QIcon(":/toolbar/param_formula"), tr("Add Formula"), this, &ParamEditorEx::addFormula);
    _actnRemoveFormula = menu->addAction(QIcon(":/toolbar/param_delete"), tr("Remove Formula"), this, &ParamEditorEx::removeFormula);
    _actnRemoveFormula->setVisible(false);

    auto menuButton = new QPushButton;
    menuButton->setFlat(true);
    menuButton->setIcon(QIcon(":/toolbar/menu"));
    menuButton->setFixedWidth(24);
    connect(menuButton, &QPushButton::clicked, this, [this, menu, menuButton](){
        this->_paramEditor->editorFocused(true);
        // button->setMenu() crashes the app on MacOS when button is clicked, so show manually
        menu->popup(menuButton->mapToGlobal(menuButton->rect().bottomLeft()));
    });

    ParamEditor::Options opts(_tmpParam);
    opts.auxControl = menuButton;

    _paramEditor = new ParamEditor(opts);

    connect(menu, &QMenu::aboutToHide, _paramEditor, &ParamEditor::focus);

    // Main layout
    LayoutV({
        // when append items here, add respective members to the enum in `createFormulaEditor()`
        _paramEditor,
        Space(6)
    }).setMargin(0).setSpacing(0).useFor(this);

    toggleFormulaView();
}

ParamEditorEx::~ParamEditorEx()
{
    // Delete editor manually to be sure it is deleted BEFORE _tmpParam
    // It's because of editor uses its param in destructor and param must be still valid.
    delete _paramEditor;
    delete _tmpParam;
    if (_tmpFormula)
        delete _tmpFormula;
}

void ParamEditorEx::addFormula()
{
    _hasFormula = true;
    toggleFormulaView();
    _formulaEditor->setFocus();
}

void ParamEditorEx::removeFormula()
{
    _hasFormula = false;
    toggleFormulaView();
    _paramEditor->setFocus();
}

void ParamEditorEx::createFormulaEditor()
{
    // This enum should match content of the main layout
    enum { ROW_VALUE, ROW_SPACER, ROW_CODE };

    _tmpFormula = new Z::Formula(_tmpParam);
    if (_formula)
    {
        _tmpFormula->setCode(_formula->code());
        _tmpFormula->assignDeps(_formula);
    }
    FormulaEditor::Options opts;
    opts.formula = _tmpFormula;
    opts.targetParam = _param;
    opts.globalParams = _globalParams;
    opts.formulas = _formulas;
    _formulaEditor = new FormulaEditor(opts);
    connect(_paramEditor, &ParamEditor::unitChanged, this, &ParamEditorEx::unitChanged);
    qobject_cast<QVBoxLayout*>(layout())->insertWidget(ROW_CODE, _formulaEditor);
}

void ParamEditorEx::toggleFormulaView()
{
    if (_hasFormula && !_tmpFormula)
        createFormulaEditor();

    if (_formulaEditor)
        _formulaEditor->setVisible(_hasFormula);

    _actnAddFormula->setVisible(!_hasFormula);
    _actnRemoveFormula->setVisible(_hasFormula);

    auto valueEditor = qobject_cast<QLineEdit*>(_paramEditor->valueEditor());
    valueEditor->setFont(Z::Gui::ValueFont().readOnly(_hasFormula).get());
    valueEditor->setReadOnly(_hasFormula);

    if (_formulaEditor)
    {
        _formulaEditor->calculate();
        _formulaEditor->setFocus();
    }
}

void ParamEditorEx::apply()
{
    if (_hasFormula)
    {
        if (!_formula)
        {
            _formula = new Z::Formula(_param);
            _formulas->put(_formula);
        }
        _formula->setCode(_tmpFormula->code());
        _formula->assignDeps(_tmpFormula);
    }
    else
    {
        if (_formula)
        {
            _formulas->free(_param);
            _formula = nullptr;
        }
        _paramEditor->apply();
    }
    _param->setExpr(_tmpParam->expr());
    _param->setValue(_tmpParam->value());
}

void ParamEditorEx::focus()
{
    _paramEditor->focus();
}

void ParamEditorEx::unitChanged(Z::Unit unit)
{
    // Assign new unit to the param in order to formula can consider it when calculate
    _tmpParam->setValue(Z::Value(_tmpParam->value().value(), unit));
    _formulaEditor->calculate();
}
