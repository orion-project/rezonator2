#include "Appearance.h"
#include "ParamEditor.h"
#include "ParamsListWidget.h"
#include "UnitWidgets.h"
#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriTools.h"
#include "widgets/OriValueEdit.h"

#include <QDebug>
#include <QLabel>
#include <QPushButton>

//------------------------------------------------------------------------------
//                              LinkButton
//------------------------------------------------------------------------------

class LinkButton : public QPushButton
{
public:
    LinkButton() : QPushButton()
    {
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        Z::Gui::setSymbolFont(this);
        showLinkSource(nullptr);
    }

    QSize sizeHint() const override
    {
        return QSize(10, 10);
    }

    void showLinkSource(Z::Parameter *param)
    {
        QString text = param ? ("= " + param->alias() + " =") : QString("=");
        int w = fontMetrics().width(text);
        setFixedWidth(w + 2*Ori::Gui::layoutSpacing());
        setText(text);
    }
};

//------------------------------------------------------------------------------
//                              ParamEditor
//------------------------------------------------------------------------------

ParamEditor::ParamEditor(Options opts)
    : QWidget(), _param(opts.param), _globalParams(opts.globalParams), _paramLinks(opts.paramLinks)
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

    auto layout = Ori::Layouts::LayoutH({})
            .setMargin(0)
            .setSpacing(0)
            .useFor(this);

#ifdef Q_OS_MACOS
    // It looks ok on other platforms, but lack of spacing is evident on mac
    layout.add(new QLabel(" "));
#endif

    if (opts.showName)
       layout.add({
            _labelName = new QLabel(_param->name()),
            Ori::Layouts::Space(def_spacing * 2)
       });

    layout.add(
        _labelLabel = Z::Gui::symbolLabel(paramLabel % " = ")
    );

    if (opts.allowLinking)
    {
        _labelLabel->setText(paramLabel); // remove '='
        _linkButton = new LinkButton;
        _linkButton->setToolTip(tr("Link to global parameter"));
        _linkButton->showLinkSource(_linkSource);
        connect(_linkButton, &QPushButton::clicked, this, &ParamEditor::linkToGlobalParameter);
        layout.add({ Ori::Layouts::Space(3), _linkButton, Ori::Layouts::Space(3) });
    }

    layout.add({
        _valueEditor = new Ori::Widgets::ValueEdit,
        Ori::Layouts::Space(3),
        _unitsSelector = new UnitComboBox(_param->dim())
    });

    Z::Gui::setValueFont(_valueEditor);

    // make some room around widgets to make highlighting visible
    int hs = def_spacing / 2 + 1;
    setContentsMargins(def_spacing, hs, hs, hs);

    setIsLinked(_linkSource);
    populate();

    _valueEditor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    connect(_valueEditor, SIGNAL(focused(bool)), this, SLOT(editorFocused(bool)));
    connect(_valueEditor, SIGNAL(keyPressed(int)), this, SLOT(editorKeyPressed(int)));
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
    populate();
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

QString ParamEditor::verify() const
{
    if (!_valueEditor->ok())
        return tr("Ivalid number format");

    Z::Value value(_valueEditor->value(), _unitsSelector->selectedUnit());

    return _param->verify(value);
}

void ParamEditor::apply()
{
    if (!_valueEditor->ok()) return;

    Z::Value value(_valueEditor->value(), _unitsSelector->selectedUnit());

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

    _param->setValue(value);
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
        p.setColor(QPalette::Button, Ori::Color::blend(p.color(QPalette::Button), p.color(QPalette::Light), 0.5));
        setAutoFillBackground(true);
        setBackgroundRole(QPalette::Button);
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
