#include "ParamEditor.h"

#include "ParamsListWidget.h"
#include "UnitWidgets.h"
#include "../Appearance.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriValueEdit.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>

using Ori::Widgets::ValueEdit;

//------------------------------------------------------------------------------
//                              LinkButton
//------------------------------------------------------------------------------

LinkButton::LinkButton() : QToolButton()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFont(Z::Gui::ParamLabelFont().get());
    setStyleSheet("color:" + Z::Gui::globalParamColorHtml());
    showLinkSource(nullptr);
}

QSize LinkButton::sizeHint() const
{
    return QSize(10, 10);
}

void LinkButton::showLinkSource(Z::Parameter *param)
{
    QString text = param ? ("= " + param->alias() + " =") : QString("=");
    int w = fontMetrics().horizontalAdvance(text);
    setFixedWidth(w + 2 * Ori::Gui::layoutSpacing());
    setText(text);
}

void LinkButton::focusInEvent(QFocusEvent *e)
{
    QToolButton::focusInEvent(e);
    emit focused(true);
}

void LinkButton::focusOutEvent(QFocusEvent *e)
{
    QToolButton::focusOutEvent(e);
    emit focused(false);
}

//------------------------------------------------------------------------------
//                              LinkButton
//------------------------------------------------------------------------------

MenuButton::MenuButton(QList<QAction *> actions) : QPushButton()
{
    setFlat(true);
    setIcon(QIcon(":/toolbar16/menu"));
    setFixedWidth(24);

    _menu = new QMenu(this);
    for (auto action : actions)
        if (action)
            _menu->addAction(action);
        else _menu->addSeparator();

    connect(_menu, &QMenu::aboutToShow, [this](){ _isMenuOpened = true; });
    connect(_menu, &QMenu::aboutToHide, [this](){ _isMenuOpened = false; });
}

void MenuButton::focusInEvent(QFocusEvent *e)
{
    QPushButton::focusInEvent(e);
    emit focused(true);
}

void MenuButton::focusOutEvent(QFocusEvent *e)
{
    QPushButton::focusOutEvent(e);

    // Don't raise focus-lost event because after menu has closed focus returns to the button
    if (!_isMenuOpened)
        emit focused(false);
}

//------------------------------------------------------------------------------

namespace {
int countSuitableGlobalParams(Z::Parameters* globalParams, Z::Parameter* param)
{
    auto dim = param->dim();
    int count = 0;
    for (auto p : *globalParams)
        if (p->dim() == dim)
        {
            // Parameters of fixed dim can only be linked unit-to-unit
            if (dim == Z::Dims::fixed() and
                p->value().unit() != param->value().unit())
                continue;

            count++;
        }
    return count;
}

Z::Parameters getSuitableGlobalParams(Z::Parameters* globalParams, Z::Parameter* param)
{
    auto dim = param->dim();
    Z::Parameters params;
    for (auto p : *globalParams)
        if (p->dim() == dim)
        {
            // Parameters of fixed dim can only be linked unit-to-unit
            if (dim == Z::Dims::fixed() and
                p->value().unit() != param->value().unit())
                continue;

            params.append(p);
        }
    return params;
}
}

//------------------------------------------------------------------------------
//                              ParamEditor
//------------------------------------------------------------------------------

ParamEditor::ParamEditor(Options opts) : QWidget(),
    _param(opts.param), _globalParams(opts.globalParams), _paramLinks(opts.paramLinks), _ownParam(opts.ownParam)
{
    _param->addListener(this);

    if (_paramLinks)
    {
        auto link = _paramLinks->byTarget(_param);
        if (link) _linkSource = link->source();
    }

    int def_spacing = Ori::Gui::layoutSpacing();

    auto paramLabel = _param->label();
    if (paramLabel.isEmpty())
        paramLabel = _param->alias();

    auto layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);

    if (opts.showName)
    {
        _labelName = new QLabel(_param->name());
        layout->addWidget(_labelName);
        layout->addSpacing(def_spacing * 2);
    }

    _labelLabel = new QLabel(paramLabel % " = ");
    _labelLabel->setFont(Z::Gui::ParamLabelFont().get());
    layout->addWidget(_labelLabel);

    if (opts.allowLinking and countSuitableGlobalParams(_globalParams, _param))
    {
        _labelLabel->setText(paramLabel);
        _linkButton = new LinkButton;
        _linkButton->setToolTip(tr("Link to global parameter"));
        _linkButton->showLinkSource(_linkSource);
        connect(_linkButton, &QToolButton::clicked, this, &ParamEditor::linkToGlobalParameter);
        layout->addSpacing(def_spacing);
        layout->addWidget(_linkButton);
        layout->addSpacing(def_spacing);
    }

    _valueEditor = new ValueEdit;

    _unitsSelector = new UnitComboBox(_param->dim());
    layout->addWidget(_valueEditor);
    layout->addSpacing(3);
    layout->addWidget(_unitsSelector);

    if (!opts.menuButtonActions.isEmpty())
    {
        auto menuButton = new MenuButton(opts.menuButtonActions);
        connect(menuButton, &MenuButton::focused, this, &ParamEditor::editorFocused);
        connect(menuButton, &QPushButton::clicked, [this, menuButton](){
            // When menu is opened, each action has a pointer to the current param editor
            foreach (auto action, menuButton->menu()->actions())
                action->setData(QVariant::fromValue(this));

            // button->setMenu() crashes the app on MacOS when button is clicked, so show manually
            menuButton->menu()->popup(menuButton->mapToGlobal(menuButton->rect().bottomLeft()));
        });
        layout->addWidget(menuButton);
    }

    if (opts.auxControl)
    {
        layout->addSpacing(6);
        layout->addWidget(opts.auxControl);
    }

    // make some room around widgets to make highlighting visible
    int hs = def_spacing / 2 + 1;
    setContentsMargins(def_spacing, hs, hs, hs);

    setIsLinked(_linkSource);
    populate();

    _valueEditor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    connect(_valueEditor, &ValueEdit::focused, this, &ParamEditor::editorFocused);
    connect(_valueEditor, &ValueEdit::keyPressed, this, &ParamEditor::editorKeyPressed);
    connect(_valueEditor, &ValueEdit::valueEdited, this, &ParamEditor::valueEdited);
    connect(_unitsSelector, &UnitComboBox::focused, this, &ParamEditor::editorFocused);
    connect(_unitsSelector, &UnitComboBox::unitChanged, this, &ParamEditor::unitChanged);
    if (_linkButton)
        connect(_linkButton, &LinkButton::focused, this, &ParamEditor::editorFocused);

    _valueEditor->selectAll();
}

ParamEditor::~ParamEditor()
{
    if (_ownParam)
        delete _param;
    else
        _param->removeListener(this);
}

void ParamEditor::parameterChanged(Z::ParameterBase*)
{
    if (_paramChangedHandlerEnabled) populate();
}

void ParamEditor::populate()
{
    showValue(_param);
}

void ParamEditor::showValue(Z::Parameter *param)
{
    _valueEditor->setValue(param->value().value());
    _unitsSelector->setSelectedUnit(param->value().unit());
}

void ParamEditor::setIsLinked(bool on)
{
    _valueEditor->setReadOnly(on);
    _valueEditor->setFont(Z::Gui::ValueFont().readOnly(on).get());
    _unitsSelector->setEnabled(!on);
}

Z::Value ParamEditor::getValue() const
{
    return Z::Value(_valueEditor->value(), _unitsSelector->selectedUnit());
}

QString ParamEditor::verify() const
{
    if (!_valueEditor->ok())
        return tr("Ivalid number format");

    Z::Value value = getValue();

    return _param->verify(value);
}

void ParamEditor::apply()
{
    if (!_valueEditor->ok()) return;

    Z::Value value = getValue();

    auto res = _param->verify(value);
    if (!res.isEmpty())
    {
        // TODO let know to the user somehow about this
        qWarning() << "Parameter value should be verified before applying";
        return;
    }

    if (_paramLinks)
    {
        auto oldLink = _paramLinks->byTarget(_param);
        if (oldLink)
        {
            // Link has been removed
            if (!_linkSource)
            {
                _paramLinks->removeOne(oldLink);
                delete oldLink;
            }
            // Link has been changed
            else if (oldLink->source() != _linkSource)
            {
                _paramLinks->removeOne(oldLink);
                delete oldLink;
                auto newLink = new Z::ParamLink(_linkSource, _param);
                _paramLinks->append(newLink);
            }
            // Else link has not been changed
        }
        // New link has been added
        else if (_linkSource)
        {
            auto newLink = new Z::ParamLink(_linkSource, _param);
            _paramLinks->append(newLink);
        }
    }

    _paramChangedHandlerEnabled = false;
    _param->setValue(value);
    _paramChangedHandlerEnabled = true;
}

void ParamEditor::focus()
{
    _valueEditor->setFocus();
    _valueEditor->selectAll();
}

void ParamEditor::editorFocused(bool focus)
{
    Z::Gui::setFocusedBackground(this, focus);
    if (focus) emit focused();
}

void ParamEditor::editorKeyPressed(int key)
{
    switch (key)
    {
    case Qt::Key_Up: emit goingFocusPrev(); break;
    case Qt::Key_Down: emit goingFocusNext(); break;
    default:;
    }
}

void ParamEditor::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    focus();
}

QWidget* ParamEditor::labelName() const { return _labelName; }
QWidget* ParamEditor::labelLabel() const { return _labelLabel; }
QWidget* ParamEditor::valueEditor() const { return _valueEditor; }
QWidget* ParamEditor::unitsSelector() const { return _unitsSelector; }

void ParamEditor::linkToGlobalParameter()
{
    focus();

    Z::Parameters availableParams = getSuitableGlobalParams(_globalParams, _param);
    if (availableParams.isEmpty())
        return Ori::Dlg::info(tr("There are no suitable parameters to link to"));

    availableParams.insert(0, ParamsListWidget::noneParam());

    auto selected = _linkSource ? _linkSource : ParamsListWidget::noneParam();
    selected = ParamsListWidget::selectParamDlg(&availableParams, selected, _linkButton->toolTip());
    if (!selected) return;

    _linkSource = selected != ParamsListWidget::noneParam() ? selected : nullptr;
    _linkButton->showLinkSource(_linkSource);
    setIsLinked(_linkSource);
    showValue(_linkSource ? _linkSource : _param);
}
