#ifndef CUSTOM_PLOT_FUNC_WINDOW_H
#define CUSTOM_PLOT_FUNC_WINDOW_H

#include "../funcs/PlotFuncWindowStorable.h"
#include "../math/CustomPlotFunction.h"

class CustomPlotCodeWindow;

class CustomPlotFuncWindow final : public PlotFuncWindowStorable
{
    Q_OBJECT

public:
    explicit CustomPlotFuncWindow(Schema*);
    
    CustomPlotFunction* function() const { return dynamic_cast<CustomPlotFunction*>(_function); }

    // Implementation of PlotFuncWindow
    bool configureInternal() override;
    Z::Unit getDefaultUnitX() const override;
    Z::Unit getDefaultUnitY() const override;

    // Implementation of PlotFuncWindowStorable
    QString readFunction(const QJsonObject& root) override;
    QString writeFunction(QJsonObject& root) override;
    
protected:
    void closeEvent(QCloseEvent* ce) override;

    void beforeUpdate() override;
    void afterUpdate() override;
    
private:
    QAction *_actnShowCode;
    QPointer<CustomPlotCodeWindow> _codeWindow;
    QString _defaultWindowTitle;

    void showCode();
};

#endif // CUSTOM_PLOT_FUNC_WINDOW_H
