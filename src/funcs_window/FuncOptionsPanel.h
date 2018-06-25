#ifndef FUNC_OPTIONS_PANEL_H
#define FUNC_OPTIONS_PANEL_H

#include <QMap>
#include <QToolButton>

class PlotFuncWindow;

class FunctionModeButton : public QToolButton
{
    Q_OBJECT

public:
    FunctionModeButton(const QString& icon, const QString& text, int mode);
    int mode() const { return _mode; }

private:
    int _mode;
};


class FuncOptionsPanel : public QWidget
{
    Q_OBJECT

public:
    FuncOptionsPanel(PlotFuncWindow *window);

protected:
    QMap<int, FunctionModeButton*> _modeButtons;

    virtual int currentFunctionMode() const { return 0; }
    virtual void functionModeChanged(int mode) { Q_UNUSED(mode); }
    void showCurrentMode();

    QWidget* makeSectionHeader(const QString& title);
    QWidget* makeModeButton(const QString& icon, const QString& text, int mode);

private:
    PlotFuncWindow *_window;
    void modeButtonClicked();
};

#endif // FUNC_OPTIONS_PANEL_H
