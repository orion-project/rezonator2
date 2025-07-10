#include "ParamEditor.h"

#include "ParamsListWidget.h"
#include "UnitWidgets.h"
#include "../app/Appearance.h"
#include "../core/Format.h"
#include "../math/tinyexpr.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>

//------------------------------------------------------------------------------
//                                ValueEdit
//------------------------------------------------------------------------------

ValueEdit::ValueEdit() : QLineEdit()
{
    setProperty("role", "value-editor");
    setAlignment(Qt::AlignRight);
    setSizePolicy(QSizePolicy::Preferred, sizePolicy().verticalPolicy());
    connect(this, &QLineEdit::textChanged, this, &ValueEdit::onTextEdited);
    connect(this, &QLineEdit::textEdited, this, &ValueEdit::onTextEdited);
}

void ValueEdit::setValue(double v)
{
    _skipProcessing = true;
    
    _value = v;
    _ok = !qIsNaN(_value);
    indicateValidation();
    
    QLocale loc(QLocale::C);
    QString s = loc.toString(_value, 'g', _numberPrecision);
    // thousand separator for C-locale is comma, need not it
    s = s.replace(loc.groupSeparator(), QString());
    setText(s);

    _skipProcessing = false;
}

QString ValueEdit::expr() const
{
    return text().trimmed();
}

void ValueEdit::setExpr(const QString &expr)
{
    setText(expr);
}

void ValueEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit focused(true);
}

void ValueEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit focused(false);
}

void ValueEdit::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(e);
    emit keyPressed(e->key());
}

void ValueEdit::onTextEdited(const QString& text)
{
    processInput(text);

    if (_ok)
        emit valueEdited(_value);
}

void ValueEdit::processInput(const QString& text)
{
    if (_skipProcessing) return;

    // TODO: refine built-in functions and constants
    // TODO: don't parse "1,2" as "2" (why parse lists?)
    // TODO: add the ** operator for pow() unstead of ^
    static double inf = qInf();
    static const int varCount = 3;
    static te_variable vars[varCount] = {{"inf", &inf}, {"Inf", &inf}, {"INF", &inf}};
    
    bool ok = true;
    auto expr = text.toStdString();
    te_expr *c = te_compile(expr.c_str(), vars, varCount, nullptr);
    if (c) {
        _value = te_eval(c);
        ok = !qIsNaN(_value);
        setToolTip(Z::format(_value));
        te_free(c);
    } else {
        ok = false;
        setToolTip("");
    }
    if (ok != _ok) {
        _ok = ok;
        indicateValidation();
    }
}

void ValueEdit::indicateValidation()
{
    setProperty("status", _ok ? "ok" : "invalid");
    style()->unpolish(this);
    style()->polish(this);
}

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
    setIcon(QIcon(":/toolbar/menu"));
    setFixedWidth(24);

    _menu = new QMenu(this);
    for (auto action : actions)
        if (action)
            _menu->addAction(action);
        else _menu->addSeparator();

    connect(_menu, &QMenu::aboutToShow, this, [this](){ _isMenuOpened = true; });
    connect(_menu, &QMenu::aboutToHide, this, [this](){ _isMenuOpened = false; });
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
int countSuitableGlobalParams(const Z::Parameters* globalParams, const Z::Parameter* param)
{
    auto dim = param->dim();
    int count = 0;
    foreach (auto p, *globalParams)
        if (p->dim() == dim)
        {
            // Parameters of fixed dim can only be linked unit-to-unit
            if (dim == Z::Dims::fixed() &&
                p->value().unit() != param->value().unit())
                continue;

            count++;
        }
    return count;
}

Z::Parameters getSuitableGlobalParams(const Z::Parameters* globalParams, const Z::Parameter* param)
{
    auto dim = param->dim();
    Z::Parameters params;
    foreach (auto p, *globalParams)
        if (p->dim() == dim)
        {
            // Parameters of fixed dim can only be linked unit-to-unit
            if (dim == Z::Dims::fixed() &&
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
    _param(opts.param), _globalParams(opts.globalParams), _paramLinks(opts.paramLinks),
    _ownParam(opts.ownParam), _checkChanges(opts.checkChanges)
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
    layout->setContentsMargins(0, 0, 0, 0);
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

    if (opts.allowLinking && countSuitableGlobalParams(_globalParams, _param))
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
    setFocusProxy(_valueEditor);
    setFocusPolicy(Qt::StrongFocus);

    _unitsSelector = new UnitComboBox(_param->dim(), opts.units);
    layout->addWidget(_valueEditor);
    layout->addSpacing(3);
    layout->addWidget(_unitsSelector);

    if (!opts.menuButtonActions.isEmpty())
    {
        auto menuButton = new MenuButton(opts.menuButtonActions);
        connect(menuButton, &MenuButton::focused, this, &ParamEditor::editorFocused);
        connect(menuButton, &QPushButton::clicked, this, [this, menuButton](){
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
    connect(_unitsSelector, &UnitComboBox::unitChanged, this, &ParamEditor::unitChangedRaw);
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
    showValue(_param, false);
}

void ParamEditor::showValue(Z::Parameter *param, bool ignoreExpr)
{
    QString expr = param->expr();
    if (expr.isEmpty() || ignoreExpr)
        _valueEditor->setValue(param->value().value());
    else _valueEditor->setExpr(expr);
    _unitsSelector->setSelectedUnit(param->value().unit());
}

void ParamEditor::setIsLinked(bool on)
{
    setReadonly(on, on);
}

void ParamEditor::setReadonly(bool valueOn, bool unitOn)
{
    if ((!valueOn || !unitOn) && _linkSource)
    {
        qWarning() << "Parameter editor" << _param->alias() << "Unable to set readonly=false when link source is set";
        return;
    }
    _valueEditor->setReadOnly(valueOn);
    _unitsSelector->setEnabled(!unitOn);
}

Z::Value ParamEditor::getValue() const
{
    return Z::Value(_valueEditor->value(), _unitsSelector->selectedUnit());
}

QString ParamEditor::verify() const
{
    if (!_valueEditor->ok())
        return tr("Ivalid value expression");

    Z::Value value = getValue();

    return _param->verify(value);
}

void ParamEditor::apply()
{
    if (!_valueEditor->ok()) return;

    Z::Value value = getValue();

    if(_checkChanges && value == _param->value())
        return;

    auto res = _param->verify(value);
    if (!res.isEmpty())
    {
        // TODO let know to the user somehow about this
        qWarning() << "Parameter value should be verified before applying";
        return;
    }
    
    Z::ParamLink *newLink = nullptr;

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
                newLink = new Z::ParamLink(_linkSource, _param);
                _paramLinks->append(newLink);
            }
            // Else link has not been changed
        }
        // New link has been added
        else if (_linkSource)
        {
            newLink = new Z::ParamLink(_linkSource, _param);
            _paramLinks->append(newLink);
        }
    }

    if (newLink)
        newLink->apply();
    else {
        _paramChangedHandlerEnabled = false;
        _param->setValue(value);
        _param->setExpr(_valueEditor->expr());
        _paramChangedHandlerEnabled = true;
    }
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
    else emit unfocused();
}

void ParamEditor::editorKeyPressed(int key)
{
    switch (key)
    {
    case Qt::Key_Enter:
    case Qt::Key_Return: emit enterPressed(); break;
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
    showValue(_linkSource ? _linkSource : _param, _linkSource);
}

void ParamEditor::unitChangedRaw(Z::Unit unit)
{
    if (rescaleOnUnitChange)
        _param->setValue(_param->value().toUnit(unit));

    emit unitChanged(unit);
}
