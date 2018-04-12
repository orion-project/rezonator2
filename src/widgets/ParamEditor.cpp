#include "Appearance.h"
#include "ParamEditor.h"
#include "UnitWidgets.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "helpers/OriTools.h"
#include "widgets/OriValueEdit.h"

#include <QDebug>
#include <QLabel>

ParamEditor::ParamEditor(Z::Parameter *param, bool showName) : QWidget()
{
    _param = param;
    _param->addListener(this);

    int def_spacing = Ori::Gui::layoutSpacing();

    auto label = param->label();
    if (label.isEmpty())
        label = param->alias();

    auto layout = Ori::Layouts::LayoutH({})
            .setMargin(0)
            .setSpacing(0)
            .useFor(this);

#ifdef Q_OS_MACOS
    // It looks ok on other platforms, but lack of spacing is evident on mac
    layout.add(new QLabel(" "))
#endif

    if (showName)
        layout.add({
                       _labelName = new QLabel(param->name()),
                       Ori::Layouts::Space(def_spacing * 2)
                   });
    layout.add({
                   _labelLabel = Z::Gui::symbolLabel(label % " = "),
                   _valueEditor = new Ori::Widgets::ValueEdit,
                   Ori::Layouts::Space(3),
                   _unitsSelector = new UnitComboBox(_param->dim())
               });

    Z::Gui::setValueFont(_valueEditor);

    // make some room around widgets to make highlighting visible
    int hs = def_spacing / 2 + 1;
    setContentsMargins(def_spacing, hs, hs, hs);

    populate();

    _valueEditor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    connect(_valueEditor, SIGNAL(focused(bool)), this, SLOT(editorFocused(bool)));
    connect(_valueEditor, SIGNAL(keyPressed(int)), this, SLOT(editorKeyPressed(int)));
    connect(_unitsSelector, SIGNAL(focused(bool)), this, SLOT(editorFocused(bool)));

    _valueEditor->selectAll();
}

ParamEditor::~ParamEditor()
{
    _param->removeListener(this);
}

void ParamEditor::populate()
{
    _valueEditor->setValue(_param->value().value());
    _unitsSelector->setSelectedUnit(_param->value().unit());
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
        qWarning() << "Parameter value should be verified before applying";
        return;
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
