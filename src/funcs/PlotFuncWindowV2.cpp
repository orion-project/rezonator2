#include "PlotFuncWindowV2.h"

#include "../app/AppSettings.h"
#include "../core/Report.h"
#include "../funcs/InfoFuncWindow.h"
#include "../funcs/FuncWindowHelpers.h"
#include "../io/CommonUtils.h"
#include "../math/FunctionGraph.h"
#include "../math/InfoFunctions.h"
#include "../widgets/PlotHelpers.h"
#include "../widgets/FrozenStateButton.h"
#include "../widgets/PlotParamsPanel.h"
#include "../widgets/UnitWidgets.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriStatusBar.h"
#include "widgets/OriLabels.h"

#include "qcpl_cursor.h"
#include "qcpl_cursor_panel.h"
#include "qcpl_io_json.h"
#include "qcpl_plot.h"

enum PlotWindowStatusPanels
{
    STATUS_UNIT_X,
    STATUS_UNIT_Y,
    STATUS_POINTS,
    STATUS_INFO,

    STATUS_PANELS_COUNT,
};

PlotWindowConfig defaultPlotFuncWindowConfig(PlotFunctionV2 *func)
{
    return {
        .statusPanelCount = STATUS_PANELS_COUNT,
        .statusPanelPoints = STATUS_POINTS,
    };
}

PlotFuncWindowV2::PlotFuncWindowV2(PlotFunctionV2 *func) :
    PlotBaseWindow(func->schema(), defaultPlotFuncWindowConfig(func)), _function(func)
{
    setTitleAndIcon(FuncWindowHelpers::makeWindowTitle(func), function()->iconPath());

    createContent();
    createActions();
    createMenuBar();
    createToolBar();
    createStatusBar();
}

PlotFuncWindowV2::~PlotFuncWindowV2()
{
    delete _function;
}

void PlotFuncWindowV2::createActions()
{
    actnUpdate = Ori::Gui::V0::action(tr("Update"), this, SLOT(update()), ":/toolbar/update", Qt::Key_F5);
    actnFreeze = Ori::Gui::V0::toggledAction(tr("Freeze"), this, SLOT(freeze(bool)), ":/toolbar/freeze", Qt::CTRL | Qt::Key_F);
}

void PlotFuncWindowV2::createMenuBar()
{
    _unitsMenuX = new UnitsMenu(this);
    _unitsMenuY = new UnitsMenu(this);
    connect(_unitsMenuX, &UnitsMenu::unitChanged, this, &PlotFuncWindowV2::setUnitX);
    connect(_unitsMenuY, &UnitsMenu::unitChanged, this, &PlotFuncWindowV2::setUnitY);
    connect(menuPlot, &QMenu::aboutToShow, this, [this](){
        _unitsMenuX->menu()->setTitle("X-axis Unit");
        _unitsMenuY->menu()->setTitle("Y-axis Unit");
        _unitsMenuX->setUnit(getUnitX());
        _unitsMenuY->setUnit(getUnitY());
    });
    connect(_plot->menuAxisX, &QMenu::aboutToShow, this, [this](){
        _unitsMenuX->menu()->setTitle(tr("Unit"));
        _unitsMenuX->setUnit(getUnitX());
    });
    connect(_plot->menuAxisY, &QMenu::aboutToShow, this, [this](){
        _unitsMenuY->menu()->setTitle(tr("Unit"));
        _unitsMenuY->setUnit(getUnitY());
    });    _plot->menuAxisX->insertMenu(_contextMenuBeginX, _unitsMenuX->menu());
    _plot->menuAxisY->insertMenu(_contextMenuBeginY, _unitsMenuY->menu());

    // Begin of the Plot menu
    auto pos = menuPlot->actions().at(0);
    menuPlot->insertAction(pos, actnUpdate);
    menuPlot->insertAction(pos, actnFreeze);
    menuPlot->insertSeparator(pos);
    
    // End of the Plot menu
    menuPlot->addSeparator();
    menuPlot->addMenu(_unitsMenuX->menu());
    menuPlot->addMenu(_unitsMenuY->menu());
}

void PlotFuncWindowV2::createToolBar()
{
    _buttonFrozenInfo = new FrozenStateButton(tr("Frozen info"), "frozen_info");

    auto t = toolbar();
    auto pos = t->actions().at(0); 
    t->insertAction(pos, actnUpdate);
    t->insertAction(pos, actnFreeze);
    actnFrozenInfo = t->insertWidget(pos, _buttonFrozenInfo);
    t->insertSeparator(pos);
}

void PlotFuncWindowV2::createContent()
{
    _plot->getAxisUnitString = [this](QCPAxis* axis) {
        if (axis == _plot->xAxis) return getUnitX()->name();
        if (axis == _plot->yAxis) return getUnitY()->name();
        return QString();
    };

    _plot->addTextVar("{func_name}", tr("Function name"), [this]{ return function()->name(); });

    _plot->addTextVarX("{unit}", tr("Unit of measurement"), [this]{ return getUnitX()->name(); });
    _plot->addTextVarX("{(unit)}", tr("Unit of measurement (in brackets)"), [this]{
        auto unit = getUnitX(); return unit == Z::Units::none() ? QString() : QStringLiteral("(%1)").arg(unit->name()); });

    _plot->addTextVarY("{unit}", tr("Unit of measurement"), [this]{ return getUnitY()->name(); });
    _plot->addTextVarY("{(unit)}", tr("Unit of measurement (in brackets)"), [this]{
        auto unit = getUnitY(); return unit == Z::Units::none() ? QString() : QStringLiteral("(%1)").arg(unit->name()); });

    _plot->setDefaultTextT(QStringLiteral("{func_name}"));
    _plot->setFormatterTextT(_plot->defaultTextT());
}

void PlotFuncWindowV2::createStatusBar()
{
    _statusBar->connect(STATUS_UNIT_X, &QWidget::customContextMenuRequested, [this](const QPoint& p){
        _unitsMenuX->setUnit(getUnitX());
        _unitsMenuX->menu()->popup(_statusBar->mapToGlobal(STATUS_UNIT_X, p));
    });
    _statusBar->connect(STATUS_UNIT_Y, &QWidget::customContextMenuRequested, [this](const QPoint& p){
        _unitsMenuY->setUnit(getUnitY());
        _unitsMenuY->menu()->popup(_statusBar->mapToGlobal(STATUS_UNIT_Y, p));
    });
}

bool PlotFuncWindowV2::configure()
{
    bool ok = configureInternal();
    if (ok)
        schema()->events().raise(SchemaEvents::Changed, "PlotFuncWindowV2: configure");
    return ok;
}

void PlotFuncWindowV2::update()
{
    beforeUpdate();

    if (_frozen)
    {
        _needRecalc = true;
        return;
    }

    clearGraphs();

    _function->calculate();
    if (!_function->ok())
    {
        showStatusError(_function->errorText());
    }
    else
    {
        clearStatusInfo();
        updateGraphs();
    }

    if (_autolimitsRequest)
    {
        _autolimitsRequest = false;
        _plot->autolimits(false);
    }

    if (_centerCursorRequested)
    {
        _centerCursorRequested = false;
        _cursor->moveToCenter();
        // Cursor info will be updated by positionChanged signal
    }
    else updateCursorInfo();

    updateStatusUnits();
    updateDataGrid();
    afterUpdate();
    
    _plot->updateTexts();
    _plot->replot();
}

void PlotFuncWindowV2::showStatusError(const QString& message)
{
    _statusBar->setText(STATUS_INFO, message);
    _statusBar->highlightError(STATUS_INFO);
}

void PlotFuncWindowV2::clearStatusInfo()
{
    _statusBar->clear(STATUS_INFO);
}

void PlotFuncWindowV2::updateStatusUnits()
{
    auto unitX = getUnitX();
    auto unitY = getUnitY();
    _statusBar->setText(STATUS_UNIT_X, QStringLiteral("X: ") + (
        unitX == Z::Units::none() ? QStringLiteral("n/a") : unitX->name()));
    _statusBar->setText(STATUS_UNIT_Y, QStringLiteral("Y: ") + (
        unitY == Z::Units::none() ? QStringLiteral("n/a") : unitY->name()));
}

void PlotFuncWindowV2::clearGraphs()
{
    for (auto g : std::as_const(_graphs))
        _plot->removeGraph(g);
    _graphs.clear();
}

void PlotFuncWindowV2::updateGraphs()
{
    auto unitX = getUnitX();
    auto unitY = getUnitY();
    QSet<QString> legend;
    for (const auto &line : function()->lines())
    {
        const auto id = line.id();
        const auto &xs = line.x();
        const auto &ys = line.y();
        if (xs.size() != ys.size()) {
            qWarning() << "Line" << id << "X len does not match Y";
            continue;
        }
        auto g = _plot->addGraph();
        for (int i = 0; i < xs.size(); i++)
            g->addData(unitX->fromSi(xs[i]), unitY->fromSi(ys[i]));
        g->setName(id);
        g->setPen(graphPen(id));
        if (!legend.contains(id)) {
            g->addToLegend();
            legend << id;
        }
        _graphs << g;
    }
}

void PlotFuncWindowV2::freeze(bool frozen)
{
    _frozen = frozen;
    actnUpdate->setEnabled(!_frozen);
    actnFrozenInfo->setVisible(_frozen);
    _unitsMenuX->setEnabled(!_frozen);
    _unitsMenuY->setEnabled(!_frozen);
    _leftPanel->setOptionsPanelEnabled(!_frozen);
    if (_frozen)
    {
        InfoFuncSummary summary(schema());
        summary.calculate();
        _buttonFrozenInfo->setInfo(summary.result());
    }
    if (!_frozen && _needRecalc)
        update();
}

void PlotFuncWindowV2::optionChanged(AppSettingsOption option)
{
    PlotBaseWindow::optionChanged(option);
    if (option == AppSettingsOption::DefaultPenFormat)
    {
        int changed = 0; // update only if format is not user-overridden
        if (auto id = Z::planeName(Z::T); !_graphPens.contains(id))
            changed += PlotHelpers::applyGraphPen(_plot, id, graphPen(id));
        if (auto id = Z::planeName(Z::S); !_graphPens.contains(id))
            changed += PlotHelpers::applyGraphPen(_plot, id, graphPen(id));
        if (changed)
            _plot->replot();
    }
}

void PlotFuncWindowV2::graphFormatted(QCPGraph *g)
{
    if (auto id = Z::planeName(Z::T); g->name() == id)
    {
        if (g->pen() == AppSettings::instance().pen(AppSettings::PenGraphT))
            _graphPens.remove(id);
        else _graphPens[id] = g->pen();
    }
    else if (auto id = Z::planeName(Z::S); g->name() == id)
    {
        if (g->pen() == AppSettings::instance().pen(AppSettings::PenGraphS))
            _graphPens.remove(id);
        else _graphPens[id] = g->pen();
    }
    else
        _graphPens[g->name()] = g->pen();
}

bool PlotFuncWindowV2::shouldCloseIfDelete(const Elements& elems)
{
    return function()->dependsOn().check(elems);
}

void PlotFuncWindowV2::elementDeleting(Schema*, Element* elem)
{
    // TODO:NEXT-VER: see InfoFuncWindow::elementDeleting() and implement similar behaviour.
    // Current behaviour is ok for now as full FunctionBase::Dead processing
    // requires an ability of saving of frozen data.
    if (function()->dependsOn().check(elem))
        disableAndClose();
}

void PlotFuncWindowV2::globalParamDeleting(Schema*, Z::Parameter *param)
{
    if (function()->dependsOn().check(param))
        disableAndClose();
}

void PlotFuncWindowV2::customParamDeleting(Z::Parameter *param)
{
    if (function()->dependsOn().check(param))
        disableAndClose();
}

void PlotFuncWindowV2::disableAndClose()
{
    _frozen = true; // disable updates
    QTimer::singleShot(0, this, [this]{close();});
}

Z::Unit PlotFuncWindowV2::getUnitX() const
{
    auto defUnit = getDefaultUnitX();
    auto thisDim = Z::Units::guessDim(_unitX);
    auto funcDim = Z::Units::guessDim(defUnit);
    return thisDim == funcDim ? _unitX : defUnit;
}

Z::Unit PlotFuncWindowV2::getUnitY() const
{
    auto defUnit = getDefaultUnitY();
    auto thisDim = Z::Units::guessDim(_unitY);
    auto funcDim = Z::Units::guessDim(defUnit);
    return thisDim == funcDim ? _unitY : defUnit;
}

void PlotFuncWindowV2::setUnitX(Z::Unit unit)
{
    auto oldUnit = getUnitX();
    if (oldUnit == unit) return;
    _unitX = unit;
    _plot->updateTextX();
    updateStatusUnits();
    schema()->markModified("PlotFuncWindowV2::setUnitX");
    PlotHelpers::rescaleLimits(_plot, PlotAxis::X, oldUnit, unit);
    PlotHelpers::rescaleCursor(_cursor, PlotAxis::X, oldUnit, unit);
    update();
}

void PlotFuncWindowV2::setUnitY(Z::Unit unit)
{
    auto oldUnit = getUnitY();
    if (oldUnit == unit) return;
    _unitY = unit;
    _plot->updateTextY();
    updateStatusUnits();
    schema()->markModified("PlotFuncWindowV2::setUnitY");
    PlotHelpers::rescaleLimits(_plot, PlotAxis::Y, oldUnit, unit);
    PlotHelpers::rescaleCursor(_cursor, PlotAxis::Y, oldUnit, unit);
    update();
}

QPen PlotFuncWindowV2::graphPen(const QString &name) const
{
    if (!_graphPens.contains(name))
    {
        if (name == Z::planeName(Z::T))
            return AppSettings::instance().pen(AppSettings::PenGraphT);
        if (name == Z::planeName(Z::S))
            return AppSettings::instance().pen(AppSettings::PenGraphS);
        return QPen(Qt::black);
    }
    return _graphPens[name];
}

bool PlotFuncWindowV2::storableRead(const QJsonObject &root, Z::Report *report)
{
    auto funcJson = root["function"].toObject();
    QString res = readFunction(funcJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    auto wndJson = root["window"].toObject();
    res = readWindowGeneral(wndJson, report);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    res = readWindowSpecific(wndJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    updatePlotItemToggleActions();
    return true;
}

bool PlotFuncWindowV2::storableWrite(QJsonObject &root, Z::Report *report)
{
    QJsonObject funcJson;
    QString res = writeFunction(funcJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    QJsonObject wndJson;
    res = writeWindowGeneral(wndJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    res = writeWindowSpecific(wndJson);
    if (!res.isEmpty())
    {
        report->warning(res);
        return false;
    }

    root["function"] = funcJson;
    root["window"] = wndJson;

    return true;
}

QString PlotFuncWindowV2::readWindowGeneral(const QJsonObject& root, Z::Report *report)
{
    // Restore cursor state
    _cursorPanel->setEnabled(root["cursor_enabled"].toBool(true));
    _cursorPanel->setMode(Z::IO::Utils::enumFromStr(root["cursor_mode"].toString(), QCPL::CursorPanel::Both));
    if (root.contains("cursor_pen"))
    {
        _cursorPen = QCPL::readPen(root["cursor_pen"].toObject(), _cursor->pen());
        _cursor->setPen(*_cursorPen);
    }
    requestCenterCursor();

    // Restore plot limits
    QCPL::AxisLimits limitsX { root["x_min"].toDouble(Double::nan()),
                               root["x_max"].toDouble(Double::nan()) };
    QCPL::AxisLimits limitsY { root["y_min"].toDouble(Double::nan()),
                               root["y_max"].toDouble(Double::nan()) };
    if (limitsX.isInvalid() || limitsY.isInvalid())
        requestAutolimits();
    else
    {
        _plot->setLimitsX(limitsX, false);
        _plot->setLimitsY(limitsY, false);
    }
    _unitX = Z::Units::findByAlias(root["x_unit"].toString(), Z::Units::none());
    _unitY = Z::Units::findByAlias(root["y_unit"].toString(), Z::Units::none());
    _plot->setFormatterTextX(root["x_title"].toString());
    _plot->setFormatterTextY(root["y_title"].toString());
    _plot->setFormatterTextT(root["t_title"].toString());

    // Restore plot format
    QCPL::JsonReport qcpl_report;
    QCPL::readPlot(root["format"].toObject(), _plot, &qcpl_report);
    for (auto& msg : qcpl_report)
        if (!msg.ok() && msg.code != QCPL::JsonError::NoData)
            report->warning(msg.message);

    auto jsonPens = root["graphs"].toObject();
    for (auto it = jsonPens.constBegin(); it != jsonPens.constEnd(); it++) {
        qDebug() << "Read pen" << it.key() << QCPL::readPen(it.value().toObject(), QPen(Qt::black));
        _graphPens[it.key()] = QCPL::readPen(it.value().toObject(), QPen(Qt::black));
        }

    return QString();
}

QString PlotFuncWindowV2::writeWindowGeneral(QJsonObject& root) const
{
    // Store cursor state
    root["cursor_enabled"] = _cursorPanel->enabled();
    root["cursor_mode"] = Z::IO::Utils::enumToStr(_cursorPanel->mode());
    if (_cursorPen) root["cursor_pen"] = QCPL::writePen(*_cursorPen);

    // Store plot limits
    auto limitsX = _plot->limitsX();
    auto limitsY = _plot->limitsY();
    root["x_min"] = limitsX.min;
    root["x_max"] = limitsX.max;
    root["x_unit"] = getUnitX()->alias();
    root["x_title"] = _plot->formatterTextX();
    root["y_min"] = limitsY.min;
    root["y_max"] = limitsY.max;
    root["y_unit"] = getUnitY()->alias();
    root["y_title"] = _plot->formatterTextY();
    root["t_title"] = _plot->formatterTextT();

    // Store plot format
    root["format"] = QCPL::writePlot(_plot);
    QJsonObject jsonPens;
    for (auto it = _graphPens.cbegin(); it != _graphPens.cend(); it++)
        jsonPens[it.key()] = QCPL::writePen(it.value());
    root["graphs"] = jsonPens;

    return QString();
}
