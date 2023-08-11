#include "ValuesEditorTS.h"

#include "UnitWidgets.h"
#include "../Appearance.h"
#include "widgets/OriValueEdit.h"
#include "helpers/OriLayouts.h"

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QStyle>
#include <QToolButton>

using namespace Ori::Layouts;

namespace {
QToolButton* makeButton(const QString& tooltip, const QString& icon, QObject* reciever, const char* slot)
{
    auto b = new QToolButton;
    b->setFixedWidth(18);
    b->setToolTip(tooltip);
    b->setFocusPolicy(Qt::NoFocus);
    b->setIcon(QIcon(icon));
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

ValueEditorTS::ValueEditorTS(const QString& label, const QString& symbol, const Z::ValueTS& value) : QWidget()
{
    _symbol = new QLabel(symbol);
    _symbol->setAlignment(Qt::AlignVCenter);
    _symbol->setFont(Z::Gui::ParamLabelFont().get());

    _label = new QLabel(label);
    _label->setAlignment(Qt::AlignVCenter);

    _editorT = new Ori::Widgets::ValueEdit;
    _editorS = new Ori::Widgets::ValueEdit;
    _editorT->setFont(Z::Gui::ValueFont().get());
    _editorS->setFont(Z::Gui::ValueFont().get());

    _unitsSelector = new UnitComboBox;

    connect(_editorT, SIGNAL(focused(bool)), this, SLOT(editorFocused(bool)));
    connect(_editorT, SIGNAL(keyPressed(int)), this, SLOT(editorKeyPressed(int)));
    connect(_editorS, SIGNAL(focused(bool)), this, SLOT(editorFocused(bool)));
    connect(_editorS, SIGNAL(keyPressed(int)), this, SLOT(editorKeyPressed(int)));
    connect(_unitsSelector, SIGNAL(focused(bool)), this, SLOT(editorFocused(bool)));

    LayoutH({
        _label,
        Stretch(),
        Space(6),
        _symbol,
        Space(6),
        _editorT,
        makeButton(tr("Assign T value to S"), ":/toolbar/equ_right", this, SLOT(assignTtoS())),
        makeButton(tr("Swap values"), ":/toolbar/equ_swap", this, SLOT(swapValues())),
        makeButton(tr("Assign S value to T"), ":/toolbar/equ_left", this, SLOT(assignStoT())),
        _editorS,
        Space(2),
        _unitsSelector,
    }).setMargin(0).setSpacing(0).useFor(this);

    int s = style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
    int hs = s / 2 + 1;
    setContentsMargins(s, hs, hs, hs);

    setValue(value);
}

void ValueEditorTS::setValue(const Z::ValueTS& value)
{
    _editorT->setValue(value.rawValueT());
    _editorS->setValue(value.rawValueS());
    _unitsSelector->populate(Z::Units::guessDim(value.unit()));
    _unitsSelector->setSelectedUnit(value.unit());
}

Z::ValueTS ValueEditorTS::value() const
{
    return Z::ValueTS(_editorT->value(), _editorS->value(), _unitsSelector->selectedUnit());
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

void ValueEditorTS::editorFocused(bool focus)
{
    Z::Gui::setFocusedBackground(this, focus);
}

void ValueEditorTS::editorKeyPressed(int key)
{
    switch (key)
    {
    case Qt::Key_Up: emit goingFocusPrev(sender() == _editorT ? Z::Plane_T : Z::Plane_S); break;
    case Qt::Key_Down: emit goingFocusNext(sender() == _editorT ? Z::Plane_T : Z::Plane_S); break;
    default:;
    }
}

void ValueEditorTS::setFocus(Z::WorkPlane plane)
{
    auto editor = plane == Z::Plane_T ? _editorT : _editorS;
    editor->setFocus();
    editor->selectAll();
}

//------------------------------------------------------------------------------
//                                ValueEditorsTS
//------------------------------------------------------------------------------

ValuesEditorTS::ValuesEditorTS() : ValuesEditorTS(QVector<ValueEditorTS*>())
{
}

ValuesEditorTS::ValuesEditorTS(const QVector<ValueEditorTS*>& editors) : QVBoxLayout()
{
    setContentsMargins(0, 0, 0, 0);
    setSpacing(0);

    _unitSpacer = new QLabel;
    _headerT = new QLabel("T");
    _headerS = new QLabel("S");
    _headerT->setFont(Z::Gui::ValueFont().bold().get());
    _headerS->setFont(Z::Gui::ValueFont().bold().get());
    _headerT->setAlignment(Qt::AlignHCenter);
    _headerS->setAlignment(Qt::AlignHCenter);

    auto header = new QHBoxLayout;
    header->setContentsMargins(0, 0, 0, 0);
    header->setSpacing(0);
    header->addStretch();
    header->addWidget(_headerT);
    header->addWidget(makeButton(tr("Assign all T values to S"), ":/toolbar/equ_right", this, SLOT(assignTtoS())));
    header->addWidget(makeButton(tr("Swap all values"), ":/toolbar/equ_swap", this, SLOT(swapValues())));
    header->addWidget(makeButton(tr("Assign all S values to T"), ":/toolbar/equ_left", this, SLOT(assignStoT())));
    header->addWidget(_headerS);
    header->addWidget(_unitSpacer);
    addLayout(header);

    int s = qApp->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
    int hs = s / 2 + 1; // should be same as margins of ValueEditorTS
    header->setContentsMargins(s, hs, hs, 0);

    for (ValueEditorTS* e : editors)
        addEditor(e);
}

void ValuesEditorTS::addEditor(ValueEditorTS *editor)
{
    _editors << editor;
    addWidget(editor);
    connect(editor, &ValueEditorTS::goingFocusNext, this, &ValuesEditorTS::goingFocusNext);
    connect(editor, &ValueEditorTS::goingFocusPrev, this, &ValuesEditorTS::goingFocusPrev);
}

void ValuesEditorTS::adjustSymbolsWidth()
{
    int maxSymbolW = 0;
    int maxEditorW = 0;
    int maxUnitW = 0;
    for (ValueEditorTS* e : _editors)
    {
        maxSymbolW = qMax(maxSymbolW, e->_symbol->width());
        maxEditorW = qMax(maxEditorW, e->_editorT->width());
        maxEditorW = qMax(maxEditorW, e->_editorS->width());
        maxUnitW = qMax(maxUnitW, e->_unitsSelector->width());
    }
    for (ValueEditorTS* e : _editors)
    {
        e->_symbol->setFixedWidth(maxSymbolW);
        e->_editorT->setFixedWidth(maxEditorW);
        e->_editorS->setFixedWidth(maxEditorW);
        e->_unitsSelector->setFixedWidth(maxUnitW);
    }
    _unitSpacer->setFixedWidth(maxUnitW+2);
    _headerS->setFixedWidth(maxEditorW);
    _headerT->setFixedWidth(maxEditorW);
}

void ValuesEditorTS::swapValues()
{
    for (auto e : _editors) e->swapValues();
}

void ValuesEditorTS::assignTtoS()
{
    for (auto e : _editors) e->assignTtoS();
}

void ValuesEditorTS::assignStoT()
{
    for (auto e : _editors) e->assignStoT();
}

void ValuesEditorTS::goingFocusNext(Z::WorkPlane plane)
{
    for (int i = 0; i < _editors.size(); i++)
        if (_editors.at(i) == sender())
            return _editors.at(i < _editors.size()-1 ? i+1 : 0)->setFocus(plane);
}

void ValuesEditorTS::goingFocusPrev(Z::WorkPlane plane)
{
    for (int i = 0; i < _editors.size(); i++)
        if (_editors.at(i) == sender())
            return _editors.at(i > 0 ? i-1 : _editors.size()-1)->setFocus(plane);
}
