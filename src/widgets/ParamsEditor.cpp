#include "ParamsEditor.h"
#include "helpers/OriLayouts.h"
#include "widgets/OriInfoPanel.h"
#include "widgets/OriValueEdit.h"

//------------------------------------------------------------------------------
//                                ParamsEditor
//------------------------------------------------------------------------------

ParamsEditor::ParamsEditor(Z::Parameters *params, QWidget *parent) : QWidget(parent), _params(params)
{
    auto layoutParams = Ori::Layouts::LayoutV({}).setMargin(0).setSpacing(0);
    for (auto param : *_params)
    {
        // TODO use parameters filter
        if (!param->visible()) continue;

        auto editor = new ParamEditor(param, false);
        connect(editor, &ParamEditor::focused, this, &ParamsEditor::paramFocused);
        connect(editor, &ParamEditor::goingFocusNext, this, &ParamsEditor::focusNextParam);
        connect(editor, &ParamEditor::goingFocusPrev, this, &ParamsEditor::focusPrevParam);

        _editors.append(editor);
        layoutParams.boxLayout()->addWidget(editor);
    }

    _infoPanel = new Ori::Widgets::InfoPanel;

    Ori::Layouts::LayoutV({
                              layoutParams,
                              Ori::Layouts::Stretch(),
                              Ori::Layouts::LayoutH({_infoPanel}).setMargin(3)
                          })
            .setMargin(0)
            .useFor(this);
}

void ParamsEditor::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    adjustEditors();
}

void ParamsEditor::adjustEditors()
{
    int w_name = 0, w_label = 0, w_unit = 0;
    for (auto editor: _editors)
    {
        if (editor->labelName())
            w_name = qMax(w_name, editor->labelName()->width());
        if (editor->labelLabel())
            w_label = qMax(w_label, editor->labelLabel()->width());
        w_unit = qMax(w_unit, editor->unitsSelector()->width());
    }
    for (auto editor: _editors)
    {
        if (w_name > 0) editor->labelName()->setFixedWidth(w_name);
        if (w_label > 0) editor->labelLabel()->setFixedWidth(w_label);
        if (w_unit > 0) editor->unitsSelector()->setFixedWidth(w_unit);
    }
}

void ParamsEditor::populate()
{
    for (auto editor : _editors) editor->populate();
}

void ParamsEditor::apply()
{
    for (auto editor : _editors) editor->apply();
}

void ParamsEditor::focus()
{
    if (_editors.size() > 0)
        _editors.at(0)->focus();
}

void ParamsEditor::focus(Z::Parameter *param)
{
    for (auto editor : _editors)
        if (editor->parameter() == param)
        {
            editor->focus();
            return;
        }
}

void ParamsEditor::paramFocused()
{
    for (auto editor : _editors)
        if (editor == sender())
        {
            auto p = editor->parameter();
            _infoPanel->setInfo("<b>" % p->name() % "</b><br>" % p->description());
            return;
        }
}

void ParamsEditor::focusNextParam()
{
    int count = _editors.size();
    if (count > 1)
    {
        void *param = sender();
        for (int i = 0; i < count; i++)
            if (_editors.at(i) == param)
            {
                int next = i+1;
                if (next > count-1) next = 0;
                _editors.at(next)->focus();
                return;
            }
    }
}

void ParamsEditor::focusPrevParam()
{
    int count = _editors.size();
    if (count > 1)
    {
        void *param = sender();
        for (int i = 0; i < count; i++)
            if (_editors.at(i) == param)
            {
                int prev = i-1;
                if (prev < 0) prev = count-1;
                _editors.at(prev)->focus();
                return;
            }
    }
}

QString ParamsEditor::verify() const
{
    QStringList errs;
    for (auto editor: _editors)
    {
        auto res = editor->verify();
        if (!res.isEmpty()) errs << res;
    }
    return errs.isEmpty()? QString(): errs.join('\n');
}

//------------------------------------------------------------------------------
//                              ParamsEditorAbcd
//------------------------------------------------------------------------------

ParamsEditorAbcd::ParamsEditorAbcd(const QString& title, const Z::Parameters &params) : QGroupBox(title, 0), _params(params)
{
    if (_params.size() != 4) return;

    auto editA = new Ori::Widgets::ValueEdit;
    auto editB = new Ori::Widgets::ValueEdit;
    auto editC = new Ori::Widgets::ValueEdit;
    auto editD = new Ori::Widgets::ValueEdit;

    _editors.append(editA);
    _editors.append(editB);
    _editors.append(editC);
    _editors.append(editD);

    auto layoutMain = new QGridLayout(this);
    layoutMain->addWidget(editA, 0, 0);
    layoutMain->addWidget(editB, 0, 1);
    layoutMain->addWidget(editC, 1, 0);
    layoutMain->addWidget(editD, 1, 1);
}

void ParamsEditorAbcd::populate()
{
    for (int i = 0; i < _params.size(); i++)
        _editors[i]->setValue(_params[i]->value().value());
}

void ParamsEditorAbcd::apply()
{
    for (int i = 0; i < _params.size(); i++)
        _params[i]->setValue(Z::Value(_editors[i]->value(), Z::Units::none()));
}

void ParamsEditorAbcd::focus()
{
    _editors.at(0)->setFocus();
}

