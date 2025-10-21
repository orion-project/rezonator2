#include "ParamEditor.h"

#include "ParamsListWidget.h"
#include "UnitWidgets.h"
#include "../app/Appearance.h"
#include "../app/PersistentState.h"
#include "../core/Format.h"
#include "../math/tinyexpr.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriValueEdit.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QRegularExpression>

//------------------------------------------------------------------------------
//                                ValueEdit
//------------------------------------------------------------------------------

class ValueEdit : public Ori::Widgets::ValueEdit
{
public:
    ValueEdit(bool useExpression) : Ori::Widgets::ValueEdit(), _useExpression(useExpression)
    {
        setProperty("role", "value-editor");

        if (_useExpression)
            setValidator(nullptr);
    }

    QString expr() const
    {
        return text().trimmed();
    }
    
    void setExpr(const QString &expr)
    {
        setText(expr);
    }
    
    void indicateValidation(bool ok) override
    {
        setProperty("status", ok ? "ok" : "invalid");
        style()->unpolish(this);
        style()->polish(this);
    }

    bool skipProcessing = false;

protected:
    bool _useExpression = false;

    void processInput(const QString& text) override
    {
        if (!_useExpression) {
            Ori::Widgets::ValueEdit::processInput(text);
            return;
        }
    
        if (skipProcessing) return;
        
        bool ok = true;
        
        if (text.contains(',')) {
            // Since now we allow any text to be typed,
            // check for numbers with comma decimal separator (European format)
            // This catches patterns like "1.125,5e3" or "1,5" that might
            // look like valid numbers but are not valid in C-locale format
            static QRegularExpression r(R"(^\s*[+-]?\d*\.?\d*,\d+([eE][+-]?\d+)?\s*$)");
            if (r.match(text.trimmed()).hasMatch()) {
                ok = false;
            }
        }
        
        if (ok) {
            double num = text.toDouble(&ok);
            if (ok) {
                _value = num;
                setToolTip("");
            } else {
                static double inf = qInf();
                static const int varCount = 3;
                static te_variable vars[varCount] = {{"inf", &inf}, {"Inf", &inf}, {"INF", &inf}};
                auto expr = text.toStdString();
                auto c = te_compile(expr.c_str(), vars, varCount, nullptr);
                if (c) {
                    _value = te_eval(c);
                    ok = !qIsNaN(_value);
                    setToolTip(Z::format(_value));
                    te_free(c);
                } else {
                    ok = false;
                    setToolTip("");
                }
            }
        }
    
        if (ok != _ok) {
            _ok = ok;
            indicateValidation(ok);
        }
    }
};

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

// namespace {
// int countSuitableGlobalParams(const Z::Parameters* globalParams, const Z::Parameter* param)
// {
//     auto dim = param->dim();
//     int count = 0;
//     foreach (auto p, *globalParams)
//         //if (p->dim() == dim) don't care about dimension match
//         {
//             // Parameters of fixed dim can only be linked unit-to-unit
//             if (dim == Z::Dims::fixed() &&
//                 p->value().unit() != param->value().unit())
//                 continue;

//             count++;
//         }
//     return count;
// }

// Z::Parameters getSuitableGlobalParams(const Z::Parameters* globalParams, const Z::Parameter* param)
// {
//     auto dim = param->dim();
//     Z::Parameters params;
//     foreach (auto p, *globalParams)
//         //if (p->dim() == dim)
//         {
//             // Parameters of fixed dim can only be linked unit-to-unit
//             if (dim == Z::Dims::fixed() &&
//                 p->value().unit() != param->value().unit())
//                 continue;

//             params.append(p);
//         }
//     return params;
// }
// }

//------------------------------------------------------------------------------
//                              ParamEditor
//------------------------------------------------------------------------------

ParamEditor::ParamEditor(Options opts) : QWidget(),
    _param(opts.param), _globalParams(opts.globalParams), _paramLinks(opts.paramLinks),
    _ownParam(opts.ownParam), _checkChanges(opts.checkChanges)
{
    _param->addListener(this);
    _oldError = _param->error();

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

    //if (opts.allowLinking && countSuitableGlobalParams(_globalParams, _param))
    if (opts.allowLinking && !_globalParams->isEmpty())
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

    _useExpression = opts.useExpression;
    _valueEditor = new ValueEdit(_useExpression);
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

void ParamEditor::parameterFailed(Z::ParameterBase*)
{
    if (_paramChangedHandlerEnabled) populate();
}

void ParamEditor::populate()
{
    _oldError = _param->error();
    showValue(_param, false);
}

void ParamEditor::showValue(Z::Parameter *param, bool ignoreExpr)
{
    QString expr = param->expr();
    Z::Value value = param->value();
    QString error;
    if (param == _linkSource) {
        // When we just added a link, it's not applyed yet,
        // but we want to show the value as it would be after apply
        auto res = Z::ParamLink::getTargetValue(_linkSource, _param);
        if (res.ok())
            value = res.result();
        else {
            value = _param->value();
            error = res.error();
        }
    }
    if (!_useExpression || expr.isEmpty() || ignoreExpr) {
        _valueEditor->skipProcessing = true;
        _valueEditor->setValue(value.value());
        _valueEditor->skipProcessing = false;
    } else
        _valueEditor->setExpr(expr);
    _unitsSelector->setSelectedUnit(value.unit());
    if (error.isEmpty() && !_oldError.isEmpty())
        error = _oldError;
    if (!error.isEmpty()) {
        _valueEditor->setToolTip(error);
        _valueEditor->indicateValidation(false);
    } else
        _valueEditor->indicateValidation(true);
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
    
    Z::ParamLink *oldLink = nullptr;
    Z::ParamLink *newLink = nullptr;

    if (_paramLinks)
    {
        oldLink = _paramLinks->byTarget(_param);
        if (oldLink)
        {
            // Link has been removed
            if (!_linkSource)
            {
                _paramLinks->removeOne(oldLink);
                delete oldLink;
                oldLink = nullptr;
            }
            // Link has been changed
            else if (oldLink->source() != _linkSource)
            {
                _paramLinks->removeOne(oldLink);
                delete oldLink;
                oldLink = nullptr;
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
    else if (!oldLink) {
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

    //Z::Parameters availableParams = getSuitableGlobalParams(_globalParams, _param);
    Z::Parameters availableParams(*_globalParams);
    if (availableParams.isEmpty())
        return Ori::Dlg::info(tr("There are no suitable parameters to link to"));

    availableParams.insert(0, ParamsListWidget::noneParam());

    auto selected = _linkSource ? _linkSource : ParamsListWidget::noneParam();
    selected = ParamsListWidget::selectParamDlg(&availableParams, selected, _linkButton->toolTip());
    if (!selected) return;

    _linkSource = selected != ParamsListWidget::noneParam() ? selected : nullptr;
    _linkButton->showLinkSource(_linkSource);
    setIsLinked(_linkSource);
    // The param error in this editor could be only from failed link
    // If we unlinked, the error can be safely cleaned to show a state that will be after apply()
    // If we link again, a new error will be calculated in showValue()
    _oldError.clear();
    showValue(_linkSource ? _linkSource : _param, _linkSource != nullptr);
}

void ParamEditor::unitChangedRaw(Z::Unit unit)
{
    if (rescaleOnUnitChange)
        _param->setValue(_param->value().toUnit(unit));

    emit unitChanged(unit);
}

//------------------------------------------------------------------------------
//                              ParamSpecEditor
//------------------------------------------------------------------------------

ParamSpecEditor::ParamSpecEditor(Z::Parameter *param, const Options &opts) : QWidget(), _param(param), _opts(opts)
{
    _aliasEditor = new QLineEdit;
    _aliasEditor->setFont(Z::Gui::ValueFont().get());

    _dimEditor = new DimComboBox;
    
    _descrEditor = new QLineEdit;
    _descrEditor->setFont(Z::Gui::ValueFont().get());
    
    if (param) {
        _aliasEditor->setText(param->alias());
        _dimEditor->setSelectedDim(param->dim());
        _descrEditor->setText(param->description());
    } else if (!_opts.recentKeyPrefix.isEmpty()) {
        auto key = (_opts.recentKeyPrefix + "_dim").toLatin1();
        _recentDim = RecentData::getDim(key.constData());
        _dimEditor->setSelectedDim(_recentDim);
    }

    Ori::Layouts::LayoutV({
        tr("Name"), _aliasEditor, Ori::Layouts::Space(9),
        tr("Dimension"), _dimEditor, Ori::Layouts::Space(9),
        tr("Description"), _descrEditor, Ori::Layouts::Space(9),
    }).setSpacing(3).setMargin(0).useFor(this);
}

QString ParamSpecEditor::alias() const { return _aliasEditor->text().trimmed(); }
QString ParamSpecEditor::descr() const { return _descrEditor->text().trimmed(); }
Z::Dim ParamSpecEditor::dim() const { return _dimEditor->selectedDim(); }

bool ParamSpecEditor::exec(const QString &title)
{
    auto verifyFunc = [this](){
        auto alias = _aliasEditor->text().trimmed();
        if (alias.isEmpty())
            return qApp->tr("Parameter name can't be empty");
        if (_opts.existedParams)
            if (auto p = _opts.existedParams->byAlias(alias); p && _param && p != _param)
                return qApp->tr("Parameter <b>%1</b> already exists").arg(alias);
        if (!Z::Param::isValidAlias(alias))
            return qApp->tr("Parameter name <b>%1</b> is invalid").arg(alias);
        return QString();
    };
    bool ok = Ori::Dlg::Dialog(this, false)
        .withTitle(title)
        .withIconPath(":/window_icons/parameter")
        .withContentToButtonsSpacingFactor(3)
        .withVerification(verifyFunc)
        .exec();
    if (ok)
    {
        if (!_opts.recentKeyPrefix.isEmpty() && dim() != _recentDim) {
            auto key = (_opts.recentKeyPrefix + "_dim").toLatin1();
            RecentData::setDim(key.constData(), dim());
        }
    }
    return ok;
}

