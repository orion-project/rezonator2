#include "AdjustmentWindow.h"

#include "AppSettings.h"
#include "HelpSystem.h"
#include "core/ElementFilter.h"
#include "widgets/Appearance.h"
#include "widgets/ParamsTreeWidget.h"
#include "funcs/FormatInfo.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriLayouts.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriValueEdit.h"
#include "widgets/OriFlatToolBar.h"

#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QVBoxLayout>
#include <QFormLayout>
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
//                          AdjusterSettingsWidget
//------------------------------------------------------------------------------

AdjusterSettingsWidget::AdjusterSettingsWidget(const AdjusterSettings &settings, QWidget *parent) : QWidget(parent)
{
    _settings = settings;

    _increment = new Ori::Widgets::ValueEdit;
    _multiplier = new Ori::Widgets::ValueEdit;
    Z::Gui::setValueFont(_increment);
    Z::Gui::setValueFont(_multiplier);
    _increment->setValue(_settings.increment);
    _multiplier->setValue(_settings.multiplier);

    _flagSetDefault = new QCheckBox(tr("Set as default values"));
    _flagUseForAll = new QCheckBox(tr("Apply for all adjusters"));

    auto layout = new QFormLayout(this);
    layout->setMargin(0);
    layout->addRow(tr("Increment"), _increment);
    layout->addRow(tr("Multiplier"), _multiplier);
    layout->addRow(_flagSetDefault);
    layout->addRow(_flagUseForAll);
}

AdjusterSettings AdjusterSettingsWidget::settings() const
{
    auto settings = _settings;
    if (_increment->ok()) settings.increment = _increment->value();
    if (_multiplier->ok()) settings.multiplier = _multiplier->value();
    return settings;
}

bool AdjusterSettingsWidget::shouldSetDefault() const { return _flagSetDefault->isChecked(); }
bool AdjusterSettingsWidget::shouldUseForAll() const { return _flagUseForAll->isChecked(); }

//------------------------------------------------------------------------------
//                             AdjusterWidget
//------------------------------------------------------------------------------

AdjusterWidget::AdjusterWidget(Schema* schema, Z::Parameter *param, QWidget *parent) : QWidget(parent)
{
    _schema = schema;
    _param = param;
    _param->addListener(this);
    _sourceValue = _param->value();
    _currentValue = _param->value();
    _elem = Z::Utils::findElemByParam(_schema, _param);

    _settings.increment = Settings::instance().adjusterIncrement;
    _settings.multiplier = Settings::instance().adjusterMultiplier;

    _valueEditor = new Ori::Widgets::ValueEdit;
    _valueEditor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    connect(_valueEditor, &Ori::Widgets::ValueEdit::focused, this, &AdjusterWidget::editorFocused);
    connect(_valueEditor, &Ori::Widgets::ValueEdit::keyPressed, this, &AdjusterWidget::editorKeyPressed);
    connect(_valueEditor, &Ori::Widgets::ValueEdit::valueEdited, this, &AdjusterWidget::valueEdited);
    Z::Gui::setValueFont(_valueEditor);

    _labelLabel = new QLabel;
    _labelLabel->setFont(Z::Gui::getElemLabelFont());
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

    case Qt::Key_Equal:
        if (qApp->keyboardModifiers().testFlag(Qt::ControlModifier))
            adjustMult();
        else
            adjustPlus();
        break;
    case Qt::Key_Minus:
        if (qApp->keyboardModifiers().testFlag(Qt::ControlModifier))
            adjustDivide();
        else
            adjustMinus();
        break;
    case Qt::Key_Plus:
        adjustPlus();
        break;
    case Qt::Key_Slash:
        adjustDivide();
        break;
    case Qt::Key_Asterisk:
    case Qt::Key_8:
        adjustMult();
        break;

    // TODO: process action hotkeys
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

    Z::Format::FormatParam f;
    f.schema = _schema;
    f.isElement = _elem;
    f.smallName = _elem;

    auto labelStr = f.format(_param);
    if (_elem)
        labelStr = QStringLiteral("%1<span style='font-weight:normal'>, </span>%2").arg(_elem->displayLabel(), labelStr);

    _labelLabel->setText(labelStr);
    _isReadOnly = f.isReadOnly;

    _buttonMult->setEnabled(not _isReadOnly);
    _buttonPlus->setEnabled(not _isReadOnly);
    _buttonMinus->setEnabled(not _isReadOnly);
    _buttonDivide->setEnabled(not _isReadOnly);
    _valueEditor->setReadOnly(_isReadOnly);
    Z::Gui::setFontStyle(_valueEditor, false, _isReadOnly);
}

double AdjusterWidget::currentValue() const
{
    return (_changeValueTimer and _changeValueTimer->isActive() ? _currentValue : _param->value()).value();
}

void AdjusterWidget::setCurrentValue(double value)
{
    if (_isReadOnly) return;
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
    setCurrentValue(currentValue() + _settings.increment);
}

void AdjusterWidget::adjustMinus()
{
    if (not _isFocused) focus();
    setCurrentValue(currentValue() - _settings.increment);
}

void AdjusterWidget::adjustMult()
{
    if (not _isFocused) focus();
    setCurrentValue(currentValue() * _settings.multiplier);
}

void AdjusterWidget::adjustDivide()
{
    if (not _isFocused) focus();
    setCurrentValue(currentValue() / _settings.multiplier);
}

void AdjusterWidget::restoreValue()
{
    if (_isReadOnly) return;
    if (_currentValue == _sourceValue) return;
    _currentValue = _sourceValue;
    changeValue();
}

void AdjusterWidget::changeValue()
{
    if (_changeValueTimer)
        _changeValueTimer->stop();

    auto res = _param->verify(_currentValue);
    if (res.isEmpty())
    {
        _isValueChanging = true;
        _param->setValue(_currentValue);
        _isValueChanging = false;

        if (_elem)
            _schema->events().raise(SchemaEvents::ElemChanged, _elem);
        else
            _schema->events().raise(SchemaEvents::CustomParamChanged, _param);
        _schema->events().raise(SchemaEvents::RecalRequred);
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
    if (param)
        __instance->addAdjuster(param);
}

AdjustmentWindow::AdjustmentWindow(Schema *schema, QWidget *parent)
    : QWidget(parent, Qt::Tool), SchemaToolWindow(schema), _schema(schema)
{
    __instance = this;

    setWindowTitle(tr("Adjustment"));
    setAttribute(Qt::WA_DeleteOnClose);

    _adjustersWidget = new AdjusterListWidget;

    auto actnAdd = Ori::Gui::action(tr("Add Adjuster"), this, SLOT(addAdjuster()), ":/toolbar/plus");
    _actnRestore = Ori::Gui::action(tr("Restore Value"), this, SLOT(restoreValue()), ":/toolbar/restore");
    _actnSettings = Ori::Gui::action(tr("Settings..."), this, SLOT(setupAdjuster()), ":/toolbar/settings");
    _actnDelete = Ori::Gui::action(tr("Delete"), this, SLOT(deleteAdjuster()), ":/toolbar/delete");
    auto actnHelp = Ori::Gui::action(tr("Help"), this, SLOT(help()), ":/toolbar/help");

    auto toolbar = Ori::Gui::toolbar({
        Ori::Gui::textToolButton(actnAdd), nullptr, _actnRestore, _actnSettings, _actnDelete, nullptr, actnHelp
    });
    toolbar->setStyleSheet("border-bottom: 1px solid silver");
    toolbar->setIconSize(QSize(16, 16));

    LayoutV({
        LayoutV({toolbar, _adjustersWidget, Stretch()}).setMargin(0).setSpacing(0)
    }).setMargin(0).setSpacing(0).useFor(this);

    updateActions();
}

AdjustmentWindow::~AdjustmentWindow()
{
    __instance = nullptr;
    // TODO: store adjusters
}

AdjusterWidget* AdjustmentWindow::focusedAdjuster()
{
    for (auto adjuster : _adjusters)
        if (adjuster.widget->isFocused())
            return adjuster.widget;
    return nullptr;
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
    for (auto adj : _adjusters)
        if (adj.param == param)
        {
            adj.widget->focus();
            return;
        }
    AdjusterItem adjuster;
    adjuster.param = param;
    adjuster.widget = new AdjusterWidget(_schema, param);
    connect(adjuster.widget, &AdjusterWidget::focused, this, &AdjustmentWindow::updateActions);
    _adjustersWidget->add(adjuster.widget);
    _adjusters.append(adjuster);
}

void AdjustmentWindow::addAdjuster()
{
    Z::Parameters existedParams;
    for (auto adj : _adjusters)
        existedParams << adj.param;

    ParamsTreeWidget::Options opts;
    opts.schema = _schema;
    opts.dialogTitle = tr("Parameter Selector");
    opts.dialogPrompt = tr("Select a parameter to adjust");
    opts.ignoreList = existedParams;
    opts.elemFilter = ElementFilter::elemsWithVisibleParams();
    opts.paramFilter = Z::Utils::defaultParamFilter();
    auto param = ParamsTreeWidget::selectParamDlg(opts);
    if (param)
        addAdjuster(param);
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
        {
            deletingWidget->deleteLater();
            _adjusters.first().widget->focus();
        }
    }
}

void AdjustmentWindow::deleteAdjuster()
{
    auto deletingWidget = focusedAdjuster();
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
    {
        deletingWidget->deleteLater();
        _adjusters.first().widget->focus();
    }
}

void AdjustmentWindow::setupAdjuster()
{
    auto adjuster = focusedAdjuster();
    if (!adjuster) return;

    AdjusterSettingsWidget w(adjuster->settings());

    if (Ori::Dlg::Dialog(&w)
            .withTitle(tr("Adjuster Settings"))
            .withContentToButtonsSpacingFactor(2)
            .exec())
    {
        auto settings = w.settings();
        adjuster->setSettings(settings);

        if (w.shouldSetDefault())
        {
            Settings::instance().adjusterIncrement = settings.increment;
            Settings::instance().adjusterMultiplier = settings.multiplier;
            Settings::instance().save();
        }
        if (w.shouldUseForAll())
        {
            for (auto adj : _adjusters)
                if (adj.widget != adjuster)
                    adj.widget->setSettings(settings);
        }
    }
}

void AdjustmentWindow::restoreValue()
{
    auto adjuster = focusedAdjuster();
    if (!adjuster) return;

    adjuster->restoreValue();
}

void AdjustmentWindow::updateActions()
{
    _actnSettings->setEnabled(_adjusters.count() > 0);
    _actnDelete->setEnabled(_adjusters.count() > 0);

    auto adjuster = focusedAdjuster();
    _actnRestore->setEnabled(adjuster and not adjuster->isReadOnly());
}

void AdjustmentWindow::help()
{
    Z::HelpSystem::instance()->showTopic("adjustment.html");
}
