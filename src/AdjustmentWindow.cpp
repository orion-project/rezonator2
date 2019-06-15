#include "AdjustmentWindow.h"

#include "HelpSystem.h"
#include "widgets/Appearance.h"

#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriValueEdit.h"

#include <QApplication>
#include <QDateTime>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTimer>

using namespace Ori::Layouts;

namespace {
AdjustmentWindow* __instance = nullptr;
const int __changeValueIntervalMs = 250;
}

//------------------------------------------------------------------------------
//                              AdjusterButton
//------------------------------------------------------------------------------

AdjusterButton::AdjusterButton(const char *iconPath) : QToolButton()
{
    setIcon(QIcon(iconPath));
}

void AdjusterButton::focusInEvent(QFocusEvent *e)
{
    QToolButton::focusInEvent(e);
    emit focused(true);
}

void AdjusterButton::focusOutEvent(QFocusEvent *e)
{
    QToolButton::focusOutEvent(e);
    emit focused(false);
}

//------------------------------------------------------------------------------
//                             AdjusterWidget
//------------------------------------------------------------------------------

AdjusterWidget::AdjusterWidget(Z::Parameter *param, QWidget *parent) : QWidget(parent), _param(param)
{
    _param->addListener(this);
    _sourceValue = _param->value();

    _valueEditor = new Ori::Widgets::ValueEdit;
    Z::Gui::setValueFont(_valueEditor);
    _valueEditor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    connect(_valueEditor, &Ori::Widgets::ValueEdit::focused, this, &AdjusterWidget::editorFocused);
    connect(_valueEditor, &Ori::Widgets::ValueEdit::keyPressed, this, &AdjusterWidget::editorKeyPressed);
    connect(_valueEditor, &Ori::Widgets::ValueEdit::valueEdited, this, &AdjusterWidget::valueEdited);

    auto paramLabel = _param->label();
    if (paramLabel.isEmpty())
        paramLabel = _param->alias();
    _labelLabel = Z::Gui::symbolLabel(paramLabel);

    _labelUnit = new QLabel;

    _buttonPlus = new AdjusterButton(":/toolbar10/plus");
    _buttonMinus = new AdjusterButton(":/toolbar10/minus");
    _buttonMult = new AdjusterButton(":/toolbar10/multiply");
    _buttonDivide = new AdjusterButton(":/toolbar10/divide");
    connect(_buttonPlus, &QToolButton::clicked, this, &AdjusterWidget::adjustPlus);
    connect(_buttonMinus, &QToolButton::clicked, this, &AdjusterWidget::adjustMinus);
    connect(_buttonMult, &QToolButton::clicked, this, &AdjusterWidget::adjustMult);
    connect(_buttonDivide, &QToolButton::clicked, this, &AdjusterWidget::adjustDivide);
    connect(_buttonPlus, &AdjusterButton::focused, this, &AdjusterWidget::editorFocused);
    connect(_buttonMinus, &AdjusterButton::focused, this, &AdjusterWidget::editorFocused);
    connect(_buttonMult, &AdjusterButton::focused, this, &AdjusterWidget::editorFocused);
    connect(_buttonDivide, &AdjusterButton::focused, this, &AdjusterWidget::editorFocused);

    // TODO: move menu to parent and add shortcuts
    auto menu = new QMenu(this);
    menu->addAction(QIcon(":/toolbar/restore"), tr("Restore Value..."), this, &AdjusterWidget::restoreValue);
    menu->addAction(QIcon(":/toolbar/settings"), tr("Settings..."), this, &AdjusterWidget::setupAdjuster);
    menu->addAction(QIcon(":/toolbar/delete"), tr("Delete"), this, &AdjusterWidget::deleteRequsted);
    menu->addSeparator();
    menu->addAction(QIcon(":/toolbar/help"), tr("Help"), this, &AdjusterWidget::help);
    connect(menu, &QMenu::aboutToHide, this, &AdjusterWidget::focus);

    auto optionsButton = new QPushButton;
    optionsButton->setFlat(true);
    optionsButton->setIcon(QIcon(":/toolbar16/menu"));
    optionsButton->setFixedWidth(24);
    connect(optionsButton, &QPushButton::clicked, [this, menu, optionsButton](){
        this->editorFocused(true);
        // button->setMenu() crashes the app on MacOS when button is clicked, so show manually
        menu->popup(optionsButton->mapToGlobal(optionsButton->rect().bottomLeft()));
    });

    LayoutH({
        _labelLabel,
        Space(3),
        _labelUnit,
        Space(12),
        _buttonDivide,
        _buttonMinus,
        _valueEditor,
        _buttonPlus,
        _buttonMult,
        Space(6),
        optionsButton,
    }).setMargin(6).setSpacing(1).useFor(this);

    populate();
}

AdjusterWidget::~AdjusterWidget()
{
    _param->removeListener(this);
}

void AdjusterWidget::parameterChanged(Z::ParameterBase*)
{
    if (not _isValueChanging)
        _sourceValue = _param->value();

    populate();
}

void AdjusterWidget::focus()
{
    _valueEditor->setFocus();
    _valueEditor->selectAll();
}

void AdjusterWidget::editorFocused(bool focus)
{
    _isFocused = focus;
    Z::Gui::setFocusedBackground(this, focus);
    if (focus) emit focused();
}

void AdjusterWidget::editorKeyPressed(int key)
{
    switch (key)
    {
    case Qt::Key_Up: emit goingFocusPrev(); break;
    case Qt::Key_Down: emit goingFocusNext(); break;
    default:;
    }
}

void AdjusterWidget::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    focus();
}

void AdjusterWidget::populate()
{
    auto value = _param->value();

    _valueEditor->setValue(value.value());

    auto unit = value.unit();
    if (unit != Z::Units::none())
        _labelUnit->setText(QString("(%1)").arg(unit->name()));
    else _labelUnit->clear();
}

double AdjusterWidget::currentValue() const
{
    return (_changeValueTimer and _changeValueTimer->isActive() ? _currentValue : _param->value()).value();
}

void AdjusterWidget::setCurrentValue(double value)
{
    _valueEditor->setValue(value);
    if (!_changeValueTimer)
    {
        _changeValueTimer = new QTimer(this);
        _changeValueTimer->setInterval(__changeValueIntervalMs);
        connect(_changeValueTimer, &QTimer::timeout, this, &AdjusterWidget::changeValue);
    }
    _currentValue = Z::Value(value, _param->value().unit());
    _changeValueTimer->start();
}

void AdjusterWidget::adjustPlus()
{
    if (not _isFocused) focus();
    setCurrentValue(currentValue() + _increment);
}

void AdjusterWidget::adjustMinus()
{
    if (not _isFocused) focus();
    setCurrentValue(currentValue() - _increment);
}

void AdjusterWidget::adjustMult()
{
    if (not _isFocused) focus();
    setCurrentValue(currentValue() * _multiplier);
}

void AdjusterWidget::adjustDivide()
{
    if (not _isFocused) focus();
    setCurrentValue(currentValue() / _multiplier);
}

void AdjusterWidget::setupAdjuster()
{
    qDebug() << "setup";
}

void AdjusterWidget::restoreValue()
{
    _currentValue = _sourceValue;
    changeValue();
}

void AdjusterWidget::help()
{
    Z::HelpSystem::instance()->showTopic("adjustment.html");
}

void AdjusterWidget::changeValue()
{
    _changeValueTimer->stop();
    auto res = _param->verify(_currentValue);
    if (res.isEmpty())
    {
        _isValueChanging = true;
        _param->setValue(_currentValue);
        _isValueChanging = false;

        // TODO: raise events
        //schema()->events().raise(SchemaEvents::ElemChanged, elem);
        // or
        //schema()->events().raise(SchemaEvents::CustomParamChanged, _param);
        // and
        //schema()->events().raise(SchemaEvents::RecalRequred);
    }
    else
    {
        // TODO: show error
    }
}

//------------------------------------------------------------------------------
//                            AdjusterListWidget
//------------------------------------------------------------------------------

AdjusterListWidget::AdjusterListWidget(QWidget *parent) : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
}

void AdjusterListWidget::add(AdjusterWidget* w)
{
    if (_items.contains(w)) return;
    qobject_cast<QVBoxLayout*>(layout())->addWidget(w);
    _items.append(w);
    connect(w, &AdjusterWidget::goingFocusNext, this, &AdjusterListWidget::focusNextParam);
    connect(w, &AdjusterWidget::goingFocusPrev, this, &AdjusterListWidget::focusPrevParam);
    QTimer::singleShot(0, w, &AdjusterWidget::focus);
}

void AdjusterListWidget::remove(AdjusterWidget* w)
{
    if (!_items.contains(w)) return;
    layout()->removeWidget(w);
    _items.removeOne(w);
    disconnect(w, &AdjusterWidget::goingFocusNext, this, &AdjusterListWidget::focusNextParam);
    disconnect(w, &AdjusterWidget::goingFocusPrev, this, &AdjusterListWidget::focusPrevParam);
}

void AdjusterListWidget::focusNextParam()
{
    int count = _items.size();
    if (count < 2) return;
    auto item = sender();
    for (int i = 0; i < count; i++)
        if (_items.at(i) == item)
        {
            int next = i+1;
            if (next > count-1) next = 0;
            _items.at(next)->focus();
            return;
        }
}

void AdjusterListWidget::focusPrevParam()
{
    int count = _items.size();
    if (count < 2) return;
    auto item = sender();
    for (int i = 0; i < count; i++)
        if (_items.at(i) == item)
        {
            int prev = i-1;
            if (prev < 0) prev = count-1;
            _items.at(prev)->focus();
            return;
        }
}

//------------------------------------------------------------------------------
//                             AdjustmentWindow
//------------------------------------------------------------------------------

void AdjustmentWindow::adjust(Schema* schema, Z::Parameter* param)
{
    if (!__instance)
    {
        __instance = new AdjustmentWindow(schema, qApp->activeWindow());
        // TODO: restore adjusters
    }
    __instance->show();
    __instance->activateWindow();
    __instance->addAdjuster(param);
}

AdjustmentWindow::AdjustmentWindow(Schema *schema, QWidget *parent)
    : QWidget(parent, Qt::Tool), SchemaToolWindow(schema)
{
    __instance = this;

    setWindowTitle(tr("Adjustment"));
    setAttribute(Qt::WA_DeleteOnClose);

    _adjustersWidget = new AdjusterListWidget;

    LayoutV({
        LayoutV({_adjustersWidget, Stretch()}).setMargin(0).setSpacing(0)
    }).setMargin(0).setSpacing(0).useFor(this);
}

AdjustmentWindow::~AdjustmentWindow()
{
    __instance = nullptr;
    // TODO: store adjusters
}

void AdjustmentWindow::elementDeleting(Schema*, Element* elem)
{
    for (auto param : elem->params())
        deleteAdjuster(param);
}

void AdjustmentWindow::customParamDeleting(Schema*, Z::Parameter* param)
{
    deleteAdjuster(param);
}

void AdjustmentWindow::addAdjuster(Z::Parameter* param)
{
    if (!param)
    {
        // TODO: check if empty and show stub
        return;
    }

    for (auto adj : _adjusters)
        if (adj.param == param)
        {
            adj.widget->focus();
            return;
        }
    AdjusterItem adjuster;
    adjuster.param = param;
    adjuster.widget = new AdjusterWidget(param);
    connect(adjuster.widget, &AdjusterWidget::deleteRequsted, this, &AdjustmentWindow::deleteCurrentAdjuster);
    _adjustersWidget->add(adjuster.widget);
    _adjusters.append(adjuster);
}

void AdjustmentWindow::deleteAdjuster(Z::Parameter* param)
{
    AdjusterWidget *deletingWidget = nullptr;

    for (int i = 0; i < _adjusters.size(); i++)
        if (_adjusters.at(i).param == param)
        {
            deletingWidget = _adjusters.at(i).widget;
            _adjusters.removeAt(i);
            break;
        }

    if (deletingWidget)
    {
        if (_adjusters.isEmpty())
            close();
        else
            deletingWidget->deleteLater();
    }
}

void AdjustmentWindow::deleteCurrentAdjuster()
{
    auto deletingWidget = qobject_cast<AdjusterWidget*>(sender());
    if (!deletingWidget) return;

    for (int i = 0; i < _adjusters.size(); i++)
        if (_adjusters.at(i).widget == deletingWidget)
        {
            _adjusters.removeAt(i);
            break;
        }

    if (_adjusters.isEmpty())
        close();
    else
        deletingWidget->deleteLater();
}
