#ifndef CAUSTIC_WINDOW_H
#define CAUSTIC_WINDOW_H

#include "PlotFuncWindowStorable.h"
#include "../funcs_meat/CausticFunction.h"

class CausticWindow;

class CausticParamsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CausticParamsPanel(CausticWindow*);

    void prepareModes();
    void showMode();

private:
    CausticWindow* _window;
    QMap<int, FunctionModeButton*> _modeButtons;

    QWidget* makeModeButton(const QString& icon, const QString& text, int mode);
    void prepareModes_ray();
    void prepareModes_gauss();

private slots:
    void modeClicked();
};

//------------------------------------------------------------------------------

class CausticWindow : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit CausticWindow(Schema*);

    bool configure(QWidget* parent) override;

    void schemaParamsChanged(Schema*) override;

    CausticFunction* function() const { return (CausticFunction*)_function; }

protected:
    //QWidget* makeParamsPanel() override;

private:
    CausticParamsPanel* _paramsPanel = nullptr;
};

#endif // CAUSTIC_WINDOW_H
