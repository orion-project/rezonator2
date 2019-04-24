#include "ParamEditor.h"

#include "Appearance.h"
#include "ParamsListWidget.h"
#include "UnitWidgets.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriTools.h"
#include "widgets/OriValueEdit.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

using Ori::Widgets::ValueEdit;

//------------------------------------------------------------------------------
//                              LinkButton
//------------------------------------------------------------------------------

// QPushButton looks ugly on "macintosh" style, it looses its standard view
// and can't calcutale its size propely (even fixed size).
// QToolButton can't be focused by tab but looks the same on all styles so use it.
// TODO: add Ctrl+= shortcut to invoke the button, it will be even more usable than tabbing it.
class LinkButton : public QToolButton
{
public:
    LinkButton();
    QSize sizeHint() const override;
    void showLinkSource(Z::Parameter *param);
};

LinkButton::LinkButton() : QToolButton()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    Z::Gui::setSymbolFont(this);
    showLinkSource(nullptr);
}

QSize LinkButton::sizeHint() const
{
    return QSize(10, 10);
}

void LinkButton::showLinkSource(Z::Parameter *param)
{
    QString text = param ? ("= " + param->alias() + " =") : QString("=");
    int w = fontMetrics().width(text);
    setFixedWidth(w + 2 * Ori::Gui::layoutSpacing());
    setText(text);
}

//------------------------------------------------------------------------------
//                              ParamEditor
//------------------------------------------------------------------------------

ParamEditor::ParamEditor(Options opts) : QWidget(),
    _param(opts.param), _globalParams(opts.globalParams), _paramLinks(opts.paramLinks)
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

    _labelLabel = Z::Gui::symbolLabel(paramLabel % " = ");
    layout->addWidget(_labelLabel);

    if (opts.allowLinking)
    {
        _labelLabel->setText(paramLabel); // remove '='
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

    Z::Gui::setValueFont(_valueEditor);

    // make some room around widgets to make highlighting visible
    int hs = def_spacing / 2 + 1;
    setContentsMargins(def_spacing, hs, hs, hs);

    setIsLinked(_linkSource);
    populate();

    _valueEditor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    connect(_valueEditor, &ValueEdit::focused, this, &ParamEditor::editorFocused);
    connect(_valueEditor, &ValueEdit::keyPressed, this, &ParamEditor::editorKeyPressed);
    connect(_valueEditor, &ValueEdit::valueEdited, this, &ParamEditor::valueEdited);
    connect(_unitsSelector, &UnitComboBox::focused, this,  &ParamEditor::editorFocused);
    connect(_unitsSelector, &UnitComboBox::unitChanged, this, &ParamEditor::unitChanged);

    _valueEditor->selectAll();
}

ParamEditor::~ParamEditor()
{
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
    _unitsSelector->setEnabled(!on);
    Z::Gui::setFontStyle(_valueEditor, false, on);
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
    QPalette p;

    if (focus)
    {
        p.setColor(QPalette::Background, Ori::Color::blend(p.color(QPalette::Button), p.color(QPalette::Highlight), 0.2));
        setAutoFillBackground(true);
        emit focused();
    }
    else
        setAutoFillBackground(false);

    setPalette(p);
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
    Z::Parameters availableParams;
    for (auto param : *_globalParams)
        if (param->dim() == _param->dim())
            availableParams.append(param);
    if (availableParams.isEmpty())
        return Ori::Dlg::info(tr("There are no parameters to link to"));
    availableParams.insert(0, ParamsListWidget::noneParam());

    auto selected = _linkSource ? _linkSource : ParamsListWidget::noneParam();
    selected = ParamsListWidget::selectParamDlg(&availableParams, selected, _linkButton->toolTip());
    if (!selected) return;

    _linkSource = selected != ParamsListWidget::noneParam() ? selected : nullptr;
    _linkButton->showLinkSource(_linkSource);
    setIsLinked(_linkSource);
    showValue(_linkSource ? _linkSource : _param);
}
