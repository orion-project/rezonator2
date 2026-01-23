#include "ParamsEditor.h"

#include "ParamEditor.h"
#include "ValuesEditorTS.h"
#include "../app/Appearance.h"
#include "../core/Utils.h"

#include "widgets/OriLabels.h"
#include "helpers/OriLayouts.h"
#include "widgets/OriInfoPanel.h"
#include "widgets/OriValueEdit.h"

//------------------------------------------------------------------------------
//                                ParamsEditor
//------------------------------------------------------------------------------

ParamsEditor::ParamsEditor(const Z::Parameters &params, const Options opts, QWidget *parent) : QWidget(parent), _options(opts)
{
    // Parameters layout should not be used as the widget's main layout
    // because parameters can be added in runtime and it should be easy to call `addWidget()`
    // and not be bothered that someting other might be at the bottom (e.g. auxControl, _infoPanel).
    _paramsLayout = Ori::Layouts::LayoutV({}).setMargin(0).setSpacing(0).boxLayout();
    populateEditors(params);

    auto mainLayout = Ori::Layouts::LayoutV({ _paramsLayout, Ori::Layouts::Stretch() }).setSpacing(0).setMargin(0).boxLayout();

    if (opts.auxControl)
        mainLayout->addWidget(opts.auxControl);

    if (opts.showInfoPanel)
    {
        _infoPanel = new Ori::Widgets::InfoPanel;
        _infoPanel->setProperty("role", "info_panel");
        _infoPanel->setMargin(6); // this margin is inside of the frame (it's padding)
        _infoPanel->setSpacing(12);
        mainLayout->addLayout(Ori::Layouts::LayoutV({_infoPanel}).setMargin(3).boxLayout());
    }

    setLayout(mainLayout);
}

void ParamsEditor::populateEditors(const Z::Parameters &params)
{
    if (!_editors.isEmpty())
    {
        qWarning() << "ParamsEditor::populateEditors: already populated, use removeEditors first";
        return;
    }
    for (auto param : params)
        addEditor(param);
}

void ParamsEditor::removeEditors()
{
    qDeleteAll(_editors);
    _editors.clear();
}

ParamEditor *ParamsEditor::addEditor(Z::Parameter* param, const QVector<Z::Unit> &units, int index)
{
    if (_options.filter && !_options.filter->check(param))
        return nullptr;

    ParamEditor::Options o(param);
    o.allowLinking = _options.globalParams;
    o.globalParams = _options.globalParams;
    o.paramLinks = _options.paramLinks;
    o.menuButtonActions = _options.menuButtonActions;
    o.ownParam = _options.ownParams;
    o.checkChanges = _options.checkChanges;
    o.useExpression = _options.useExpression;
    o.units = units;
    if (_options.makeAuxControl)
        o.auxControl = _options.makeAuxControl(param);
    auto editor = new ParamEditor(o);
    connect(editor, &ParamEditor::focused, this, &ParamsEditor::paramFocused);
    connect(editor, &ParamEditor::unfocused, this, &ParamsEditor::paramUnfocused);
    connect(editor, &ParamEditor::goingFocusNext, this, &ParamsEditor::focusNextParam);
    connect(editor, &ParamEditor::goingFocusPrev, this, &ParamsEditor::focusPrevParam);
    connect(editor, &ParamEditor::valueEdited, this, &ParamsEditor::paramValueEdited);
    connect(editor, &ParamEditor::unitChanged, this, &ParamsEditor::paramUnitChanged);
    connect(editor, &ParamEditor::enterPressed, this, &ParamsEditor::paramEnterPressed);

    if (index < 0) {
        _editors.append(editor);
        _paramsLayout->addWidget(editor);
    }
    else {
        _editors.insert(index, editor);
        _paramsLayout->insertWidget(index, editor);
    }

    return editor;
}

void ParamsEditor::addSeparator(const QString& title, bool flat)
{
    if (title.isEmpty())
    {
        auto label = new Ori::Widgets::LabelSeparator;
        label->flat = flat;
        _paramsLayout->addWidget(label);
    }
    else
    {
        auto label = new QLabel(title);
        auto font = Z::Gui::ParamLabelFont().get();
        label->setStyleSheet(QString("font-size: %1pt; font-weight: bold; color: SteelBlue; padding: 0.1em").arg(font.pointSize()));
        _paramsLayout->addWidget(label);
    }
}

void ParamsEditor::removeEditor(Z::Parameter* param)
{
    for (int i = 0; i < _editors.size(); i++)
    {
        auto editor = _editors.at(i);
        if (editor->parameter() == param)
        {
            _editors.removeAt(i);
            delete editor;
            break;
        }
    }
}

void ParamsEditor::populateEditor(Z::Parameter* param)
{
    for (int i = 0; i < _editors.size(); i++)
    {
        auto editor = _editors.at(i);
        if (editor->parameter() == param)
        {
            editor->populate();
            break;
        }
    }
}

void ParamsEditor::populateValues()
{
    foreach (auto editor, _editors) editor->populate();
}

void ParamsEditor::applyValues()
{
    foreach (auto editor, _editors)
        editor->apply();
}

void ParamsEditor::focus()
{
    if (_editors.size() > 0)
        _editors.at(0)->focus();
}

void ParamsEditor::focus(Z::Parameter *param)
{
    foreach (auto editor, _editors)
        if (editor->parameter() == param)
        {
            editor->focus();
            return;
        }
}

void ParamsEditor::paramFocused()
{
    if (!_infoPanel) return;

    foreach (auto editor, _editors)
        if (editor == sender())
        {
            auto p = editor->parameter();
            QString descr = p->description();
            if (_options.showPythonCode && p->label() != p->alias())
                descr += QString("<p>Python: <code>elem.param('%2')</code>").arg(p->alias());
            _infoPanel->setInfo(p->name(), descr);
            return;
        }
}

void ParamsEditor::paramUnfocused()
{
    if (_options.applyMode == Options::ApplyEnter)
    {
        auto editor = qobject_cast<ParamEditor*>(sender());
        if (!editor) return;

        editor->apply();
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

void ParamsEditor::paramValueEdited(double value)
{
    auto editor = qobject_cast<ParamEditor*>(sender());
    if (!editor) return;

    if (_options.applyMode == Options::ApplyInstant)
        editor->apply();

    emit paramChanged(editor->parameter(), Z::Value(value, editor->getValue().unit()));
}

void ParamsEditor::paramUnitChanged(Z::Unit unit)
{
    auto editor = qobject_cast<ParamEditor*>(sender());
    if (!editor) return;

    if (_options.applyMode == Options::ApplyInstant || _options.applyMode == Options::ApplyManual)
        editor->apply();

    emit paramChanged(editor->parameter(), Z::Value(editor->getValue().value(), unit));
}

void ParamsEditor::paramEnterPressed()
{
    if (_options.applyMode == Options::ApplyEnter)
    {
        auto editor = qobject_cast<ParamEditor*>(sender());
        if (!editor) return;

        editor->apply();
    }
}

void ParamsEditor::moveEditorUp(Z::Parameter* param)
{
    if (_editors.size() < 2) return;
    for (int i = 0; i < _editors.size(); i++)
    {
        ParamEditor *editor = _editors.at(i);
        if (editor->parameter() == param)
        {
            _paramsLayout->removeWidget(editor);
            if (i == 0)
            {
                _paramsLayout->addWidget(editor);
                _editors.removeAt(0);
                _editors.append(editor);
            }
            else
            {
                _paramsLayout->insertWidget(i-1, editor);
                swapItems(_editors, i, i-1);
            }
            break;
        }
    }
}

void ParamsEditor::moveEditorDown(Z::Parameter* param)
{
    if (_editors.size() < 2) return;
    for (int i = 0; i < _editors.size(); i++)
    {
        ParamEditor *editor = _editors.at(i);
        if (editor->parameter() == param)
        {
            _paramsLayout->removeWidget(editor);
            if (i == _editors.size()-1)
            {
                _paramsLayout->insertWidget(0, editor);
                _editors.removeAt(i);
                _editors.insert(0, editor);
            }
            else
            {
                _paramsLayout->insertWidget(i+1, editor);
                swapItems(_editors, i, i+1);
            }
            break;
        }
    }
}

ParamEditor* ParamsEditor::selectedEditor()
{
    for (auto ed : std::as_const(_editors))
        if (ed->hasFocus())
            return ed;
    return nullptr;
}

//------------------------------------------------------------------------------
//                               ParamsEditorTS
//------------------------------------------------------------------------------

ParamsEditorTS::ParamsEditorTS(Z::ParametersTS *params, QWidget *parent) : QWidget(parent), _params(params)
{
    _valuesEditor = new ValuesEditorTS();

    foreach (Z::ParameterTS *param, *params)
    {
        auto editor = new ValueEditorTS(param->name(), param->label(), param->value());
        _editorsMap.insert(param, editor);
        _valuesEditor->addEditor(editor);
    }

    setLayout(_valuesEditor);
}

void ParamsEditorTS::adjustSymbolsWidth()
{
    _valuesEditor->adjustSymbolsWidth();
}

void ParamsEditorTS::collect()
{
    QMapIterator<Z::ParameterTS*, ValueEditorTS*> it(_editorsMap);
    while (it.hasNext())
    {
        it.next();
        it.key()->setValue(it.value()->value());
    }
}

//------------------------------------------------------------------------------
//                              ParamsEditorAbcd
//------------------------------------------------------------------------------

ParamsEditorAbcd::ParamsEditorAbcd(const QString& title, const Z::Parameters &params) : QGroupBox(title, nullptr), _params(params)
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
