#include "CustomPlotFuncWindow.h"

#include "CustomPlotCodeWindow.h"
#include "../app/MessageBus.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"

#include <QCloseEvent>
#include <QJsonObject>
#include <QMenu>
#include <QToolBar>

#include <qcpl_plot.h>

#define A_ Ori::Gui::action

CustomPlotFuncWindow::CustomPlotFuncWindow(Schema* schema): PlotFuncWindowV2(new CustomPlotFunction(schema))
{
    _defaultWindowTitle = windowTitle();

    _actnShowCode = A_(tr("Show Code"), this, &CustomPlotFuncWindow::showCode, ":/toolbar/python_framed");
    
    menuPlot->addSeparator();
    menuPlot->addAction(_actnShowCode);
    
    toolbar()->addSeparator();
    toolbar()->addAction(_actnShowCode);
    
    _plot->putTextVarX("{default_title}", tr("Default axis title"), [this]{ return function()->titleX(); });
    _plot->putTextVarY("{default_title}", tr("Default axis title"), [this]{ return function()->titleY(); });

    _plot->setDefaultTextX("{default_title}, {(unit)}");
    _plot->setFormatterTextX(_plot->defaultTextX());
    _plot->setDefaultTextY("{default_title}, {(unit)}");
    _plot->setFormatterTextY(_plot->defaultTextY());
}

void CustomPlotFuncWindow::closeEvent(QCloseEvent* ce)
{
    if (function()->code().trimmed().isEmpty() ||
        Ori::Dlg::ok(tr("Custom function code will be lost if you close the window")))
    {
        if (_codeWindow)
            _codeWindow->close();
        SchemaMdiChild::closeEvent(ce);
    }
    else
        ce->ignore();
}

bool CustomPlotFuncWindow::configureInternal()
{
    function()->setCode(CodeUtils::loadCodeTemplate("_plot_empty"));

    return true;
}

QString CustomPlotFuncWindow::readFunction(const QJsonObject& root)
{
    function()->setCode(root["code"].toString());
    
    return {};
}

QString CustomPlotFuncWindow::writeFunction(QJsonObject& root)
{
    if (_codeWindow)
        function()->setCode(_codeWindow->code());
    
    root["code"] = function()->code();
    
    return {};
}

void CustomPlotFuncWindow::beforeUpdate()
{
    if (_codeWindow) {
        _codeWindow->clearLog();
        function()->setCode(_codeWindow->code());
    }
}

void CustomPlotFuncWindow::afterUpdate()
{
    QString customTitle = function()->customTitle();
    setWindowTitle(customTitle.isEmpty() ? _defaultWindowTitle : customTitle);
    if (_codeWindow) {
        _codeWindow->showResult();
        _codeWindow->setWindowTitle(windowTitle());
    }
}

void CustomPlotFuncWindow::showCode()
{
    if (!_codeWindow) {
        _codeWindow = new CustomPlotCodeWindow(function(), [this]{ update(); });
        _codeWindow->setWindowTitle(windowTitle());
        connect(_codeWindow, &CodeEditorWindow::closing, this, [this]{ update(); });
    }
    MessageBus::instance().send(MBE_MDI_CHILD_REQUESTED, {{ "wnd", QVariant::fromValue(_codeWindow.data()) }});
}

static Z::Unit getDefaultUnit(Z::Dim dim)
{
    if (dim == Z::Dims::linear())
        return AppSettings::instance().defaultUnitLinear;
    if (dim == Z::Dims::angular())
        return AppSettings::instance().defaultUnitAngle;
    return Z::Units::none();
}

Z::Unit CustomPlotFuncWindow::getDefaultUnitX() const
{
    return getDefaultUnit(function()->dimX());
}

Z::Unit CustomPlotFuncWindow::getDefaultUnitY() const
{
    return getDefaultUnit(function()->dimY());
}
