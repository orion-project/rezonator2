#include "FuncOptionsPanel.h"

#include "PlotFuncWindow.h"

#include <QLabel>

//------------------------------------------------------------------------------
//                              FunctionModeButton
//------------------------------------------------------------------------------

FunctionModeButton::FunctionModeButton(const QString& icon, const QString& text, int mode) : QToolButton(), _mode(mode)
{
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    setCheckable(true);
    setText(text);
    setIconSize(QSize(24, 24));
    setIcon(QIcon(icon));
    auto c = palette().color(QPalette::Highlight);
    setStyleSheet(QString::fromLatin1(
        "QToolButton{background-color:rgba(0,0,0,0);border:none;padding:4}"
        "QToolButton:hover{background-color:rgba(%1,%2,%3,30);border:none}"
        "QToolButton:pressed{background-color:rgba(%1,%2,%3,100);border:none;padding-left:5;padding-top:5}"
        "QToolButton:checked{background-color:rgba(%1,%2,%3,50);border:none}"
                ).arg(c.red()).arg(c.green()).arg(c.blue()));
}

//------------------------------------------------------------------------------
//                                FunctionParamsPanel
//------------------------------------------------------------------------------

FuncOptionsPanel::FuncOptionsPanel(PlotFuncWindow *window) : QWidget(), _window(window)
{
}

QWidget* FuncOptionsPanel::makeSectionHeader(const QString& title)
{
    auto header = new QLabel(QString("<b>%1:</b>").arg(title));
    header->setContentsMargins(6, 6, 6, 6);
    return header;
}

QWidget* FuncOptionsPanel::makeModeButton(const QString& icon, const QString& text, int mode)
{
    auto button = new FunctionModeButton(icon, text, mode);
    connect(button, &FunctionModeButton::clicked, this, &FuncOptionsPanel::modeButtonClicked);
    _modeButtons.insert(mode, button);
    return button;
}

void FuncOptionsPanel::modeButtonClicked()
{
    auto button = qobject_cast<FunctionModeButton*>(sender());
    if (button && currentFunctionMode() != button->mode())
    {
        _window->storeView(currentFunctionMode());
        functionModeChanged(button->mode());
        _window->restoreView(button->mode());
        _window->update();
        showCurrentMode();
    }
}

void FuncOptionsPanel::showCurrentMode()
{
    auto mode = currentFunctionMode();
    foreach (auto button, _modeButtons)
        button->setChecked(button->mode() == mode);
}
