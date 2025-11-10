#ifndef CUSTOM_PLOT_FUNCTION_H
#define CUSTOM_PLOT_FUNCTION_H

#include "PlotFunctionV2.h"

class PyRunner;

class CustomPlotFunction : public PlotFunctionV2
{
public:
    FUNC_ALIAS("CustomPlot")
    QString name() const override;
    FUNC_ICON(":/toolbar/plot_py")
    FUNC_HELP("func_custom_plot.html")
    
    CustomPlotFunction(Schema *schema);

    QString code() const { return _code; }
    void setCode(const QString &code) { _code = code; }
    
    QString customTitle() const { return _customTitle; }
    QString moduleName() const { return _moduleName; }
    QStringList errorLog() const { return _errorLog; }
    int errorLine() const { return _errorLine; }
    
    void setPrintFunc(std::function<void(const QString&)> printFunc) { _printFunc = printFunc; }

    Z::Dim dimX() const { return _dimX; }
    Z::Dim dimY() const { return _dimY; }
    QString titleX() const { return _titleX; }
    QString titleY() const { return _titleY; }

private:
    QString _code;
    QString _customTitle;
    QString _moduleName;
    QStringList _errorLog;
    int _errorLine;
    std::function<void(const QString&)> _printFunc;
    std::shared_ptr<PyRunner> _runner;
    Z::Dim _dimX, _dimY;
    QString _titleX, _titleY;

    // Inherited from PlotFunctionV2
    bool prepare() override;
    void unprepare() override;
    void calculateInternal() override;
    
    void showError(PyRunner *py);
    void showError(const QString &err);
};

#endif // CUSTOM_PLOT_FUNCTION_H
