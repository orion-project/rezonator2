#include "ValuesEditorTS.h"
#include "widgets/OriValueEdit.h"

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QStyle>
#include <QToolButton>
#include <QBoxLayout>

#define EDITOR_W 128
#define EDITOR_SPACING 1

namespace {
QToolButton* makeButton(const QString& tooltip, const QString& icon, QObject* reciever, const char* slot)
{
    auto b = new QToolButton;
    b->setFixedWidth(18);
    b->setToolTip(tooltip);
    b->setFocusPolicy(Qt::NoFocus);
    b->setIcon(QPixmap(":/misc10/"+icon));
    b->setStyleSheet(
        "QToolButton{background-color:rgba(0,0,0,0);border:none}"
        "QToolButton:hover{background-color:rgba(0,0,0,30);border:none}");
    reciever->connect(b, SIGNAL(clicked()), reciever, slot);
    return b;
}
}

//------------------------------------------------------------------------------
//                                ValueEditorTS
//------------------------------------------------------------------------------

ValueEditorTS::ValueEditorTS(QWidget *parent) : QWidget(parent)
{
    _symbol = new QLabel;
    QFont f = _symbol->font();
    f.setBold(true);
    f.setPointSize(14);
    f.setFamily("Times New Roman");
    _symbol->setFont(f);
    _symbol->setAlignment(Qt::AlignVCenter);
    _symbol->setFixedWidth(30);

    _label = new QLabel;
    _label->setAlignment(Qt::AlignVCenter);

    auto layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(EDITOR_SPACING);
    layout->addWidget(_label);
    layout->addStretch();
    layout->addSpacing(6);
    layout->addWidget(_symbol);
    //layout->addSpacing(6);
    layout->addWidget(_editorT = new Ori::Widgets::ValueEdit);
    layout->addWidget(makeButton(tr("Assign T value to S"), "ts_t2s", this, SLOT(assignTtoS())));
    layout->addWidget(makeButton(tr("Swap values"), "ts_swap", this, SLOT(swapValues())));
    // TODO layout->addWidget(makeButton(tr("Change both values together"), "ts_link", this, SLOT(linkValues())));
    layout->addWidget(makeButton(tr("Assign S value to T"), "ts_s2t", this, SLOT(assignStoT())));
    layout->addWidget(_editorS = new Ori::Widgets::ValueEdit);
    setLayout(layout);

    _editorT->setFixedWidth(EDITOR_W);
    _editorS->setFixedWidth(EDITOR_W);
    connect(_editorT, SIGNAL(focused(bool)), this, SLOT(editorFocused(bool)));
    connect(_editorT, SIGNAL(keyPressed(int)), this, SLOT(editorKeyPressed(int)));
    connect(_editorS, SIGNAL(focused(bool)), this, SLOT(editorFocused(bool)));
    connect(_editorS, SIGNAL(keyPressed(int)), this, SLOT(editorKeyPressed(int)));

    int s = style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
    int hs = s / 2 + 1;
    setContentsMargins(s, hs, hs, hs);
    setAutoFillBackground(true);
}

void ValueEditorTS::collect()
{
    if (!_value) return;
    _value->T = valueT();
    _value->S = valueS();
}

double ValueEditorTS::valueT() const
{
    return _editorT->value();
}

double ValueEditorTS::valueS() const
{
    return _editorS->value();
}

void ValueEditorTS::setValue(Z::PointTS *value)
{
    _value = value;
    if (!value) return;
    setValueT(value->T);
    setValueS(value->S);
}

void ValueEditorTS::setValueT(const double& value)
{
    _editorT->setValue(value);
}

void ValueEditorTS::setValueS(const double& value)
{
    _editorS->setValue(value);
}

void ValueEditorTS::setLabel(const QString& label)
{
    _label->setText(label);
}

void ValueEditorTS::setSymbol(const QString& symbol)
{
    _symbol->setText(symbol);
}

void ValueEditorTS::swapValues()
{
    double value = _editorT->value();
    _editorT->setValue(_editorS->value());
    _editorS->setValue(value);
}

void ValueEditorTS::assignTtoS()
{
    _editorS->setValue(_editorT->value());
}

void ValueEditorTS::assignStoT()
{
    _editorT->setValue(_editorS->value());
}

void ValueEditorTS::linkValues()
{
    // TODO void ValueEditorTS::linkValues()
}

void ValueEditorTS::editorFocused(bool focus)
{
    setBackgroundRole(focus? QPalette::Light: QPalette::Window);
}

void ValueEditorTS::editorKeyPressed(int key)
{
    switch (key)
    {
    case Qt::Key_Up: emit goingFocusPrev(); break;
    case Qt::Key_Down: emit goingFocusNext(); break;
    default:;
    }
}

//------------------------------------------------------------------------------
//                                ValueEditorsTS
//------------------------------------------------------------------------------

QLabel* makeHeaderLabel(const QString& title)
{
    auto label = new QLabel("<b>" + title + "</b>");
    label->setFixedWidth(EDITOR_W);
    label->setAlignment(Qt::AlignHCenter);
    label->setMargin(0);
    return label;
}

ValuesEditorTS::ValuesEditorTS(QWidget *parent) : QVBoxLayout(parent)
{
    setMargin(0);
    setSpacing(0);

    auto header = new QHBoxLayout;
    header->setMargin(0);
    header->setSpacing(EDITOR_SPACING);
    header->addStretch();
    header->addWidget(makeHeaderLabel("T"));
    header->addWidget(makeButton(tr("Assign all T values to S"), "ts_t2s", this, SLOT(assignTtoS())));
    header->addWidget(makeButton(tr("Swap all values"), "ts_swap", this, SLOT(swapValues())));
    // TODO header->addWidget(makeButton(tr("Change both values together"), "ts_link", this, SLOT(linkValues())));
    header->addWidget(makeButton(tr("Assign all S values to T"), "ts_s2t", this, SLOT(assignStoT())));
    header->addWidget(makeHeaderLabel("S"));
    addLayout(header);

    int s = qApp->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
    int hs = s / 2 + 1; // should be same as margins of ValueEditorTS
    header->setContentsMargins(s, hs, hs, 0);
}

/*void ValuesEditorTS::adjustSymbolsWidth()
{
    int max_w = 0;
    for (ValueEditorTS* e : _editors.values())
    {
        e->symbolLabel()->setMinimumWidth(0);
        e->symbolLabel()->setMaximumWidth(QWIDGETSIZE_MAX);
        qDebug() << e->symbolLabel()->width();
        if (e->symbolLabel()->width() > max_w)
            max_w = e->symbolLabel()->width();
    }
    for (auto e : _editors.values())
        e->symbolLabel()->setFixedWidth(max_w);
}*/

void ValuesEditorTS::addEditor(int id, Z::PointTS *value = nullptr)
{
    if (!_editors.contains(id))
    {
        auto editor = new ValueEditorTS;
        _editors.insert(id, editor);
        addWidget(editor);
    }
    setValue(id, value);
}

double ValuesEditorTS::valueT(int id) const
{
    auto editor = getEditor(id);
    return editor? editor->valueT(): 0;
}

double ValuesEditorTS::valueS(int id) const
{
    auto editor = getEditor(id);
    return editor? editor->valueS(): 0;
}

void ValuesEditorTS::setValueT(int id, const double& value)
{
    auto editor = getEditor(id);
    if (editor) editor->setValueT(value);
}

void ValuesEditorTS::setValueS(int id, const double& value)
{
    auto editor = getEditor(id);
    if (editor) editor->setValueS(value);
}

void ValuesEditorTS::setValue(int id, Z::PointTS *value)
{
    if (!value) return;
    auto editor = getEditor(id);
    if (editor) editor->setValue(value);
}

void ValuesEditorTS::setLabel(int id, const QString& label)
{
    auto editor = getEditor(id);
    if (editor) editor->setLabel(label);
}

void ValuesEditorTS::setSymbol(int id, const QString& symbol)
{
    auto editor = getEditor(id);
    if (editor) editor->setSymbol(symbol);
}

void ValuesEditorTS::setVisible(int id, bool visible)
{
    auto editor = getEditor(id);
    if (editor) editor->setVisible(visible);
}

ValueEditorTS* ValuesEditorTS::getEditor(int id) const
{
    if (!_editors.contains(id))
    {
        qCritical() << "Unable to find editor with id" << id;
        return nullptr;
    }
    return _editors[id];
}

void ValuesEditorTS::collect()
{
    for (auto e : _editors.values()) e->collect();
}

void ValuesEditorTS::swapValues()
{
    for (auto e : _editors.values()) e->swapValues();
}

void ValuesEditorTS::assignTtoS()
{
    for (auto e : _editors.values()) e->assignTtoS();
}

void ValuesEditorTS::assignStoT()
{
    for (auto e : _editors.values()) e->assignStoT();
}

void ValuesEditorTS::linkValues()
{
    for (auto e : _editors.values()) e->linkValues();
}
