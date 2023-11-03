#include "PlotFuncWindow.h"

#include "InfoFuncWindow.h"
#include "FuncWindowHelpers.h"
#include "../AppSettings.h"
#include "../CustomPrefs.h"
#include "../core/Format.h"
#include "../core/Protocol.h"
#include "../funcs/InfoFunctions.h"
#include "../funcs/PlotFuncRoundTripFunction.h"
#include "../funcs/FunctionGraph.h"
#include "../widgets/PlotHelpers.h"
#include "../widgets/FrozenStateButton.h"
#include "../widgets/PlotParamsPanel.h"
#include "../widgets/UnitWidgets.h"

#include "helpers/OriDialogs.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriFlatToolBar.h"
#include "widgets/OriStatusBar.h"

#include "qcpl_cursor.h"
#include "qcpl_cursor_panel.h"
#include "qcpl_format.h"
#include "qcpl_graph_grid.h"
#include "qcpl_io_json.h"
#include "qcpl_plot.h"

using namespace Ori::Gui;

enum PlotWindowStatusPanels
{
    STATUS_UNIT_X,
    STATUS_UNIT_Y,
    STATUS_POINTS,
    STATUS_INFO,

    STATUS_PANELS_COUNT,
};

PlotFuncWindow::PlotFuncWindow(PlotFunction *func) : SchemaMdiChild(func->schema()), _function(func)
{
    setTitleAndIcon(FuncWindowHelpers::makeWindowTitle(func), function()->iconPath());

    createContent();
    createActions();
    createMenuBar();
    createToolBar();
    createContextMenus();
    createStatusBar();

    updatePlotItemToggleActions();
}

PlotFuncWindow::~PlotFuncWindow()
{
    delete _function;
    delete _graphs;
}

void PlotFuncWindow::createActions()
{
    actnUpdate = action(tr("Update"), this, SLOT(update()), ":/toolbar/update", Qt::Key_F5);
    actnUpdateParams = action(tr("Update With Params..."), this,
        SLOT(updateWithParams()), ":/toolbar/update_params", Qt::CTRL | Qt::Key_F5);

    actnShowT = action(tr("Show T-plane"), this, SLOT(activateModeT()), ":/toolbar/plot_t");
    actnShowS = action(tr("Show S-plane"), this, SLOT(activateModeS()), ":/toolbar/plot_s");
    actnShowFlippedTS = action(tr("TS-flipped Mode"), this, SLOT(activateModeFlippedTS()), ":/toolbar/plot_ts");
    actnShowT->setCheckable(true);
    actnShowS->setCheckable(true);
    actnShowFlippedTS->setCheckable(true);
    actnShowT->setChecked(true);
    actnShowS->setChecked(true);

    actnShowRoundTrip = action(tr("Show Round-trip"), this, SLOT(showRoundTrip()), ":/toolbar/func_round_trip");

    actnFreeze = toggledAction(tr("Freeze"), this, SLOT(freeze(bool)), ":/toolbar/freeze", Qt::CTRL | Qt::Key_F);

    actnAutolimits = action(tr("Fit to Graphs"), _plot, SLOT(autolimits()), ":/toolbar/limits_auto");
    actnAutolimitsX = action(tr("Fit to Graphs Over X"), _plot, SLOT(autolimitsX()), ":/toolbar/limits_auto_x");
    actnAutolimitsY = action(tr("Fit to Graphs Over Y"), _plot, SLOT(autolimitsY()), ":/toolbar/limits_auto_y");

    actnZoomIn = action(tr("Zoom-in"), _plot, SLOT(zoomIn()), ":/toolbar/limits_zoom_in");
    actnZoomOut = action(tr("Zoom-out"), _plot, SLOT(zoomOut()), ":/toolbar/limits_zoom_out");
    actnZoomInX = action(tr("Zoom-in Over X"), _plot, SLOT(zoomInX()), ":/toolbar/limits_zoom_in_x");
    actnZoomOutX = action(tr("Zoom-out Over X"), _plot, SLOT(zoomOutX()), ":/toolbar/limits_zoom_out_x");
    actnZoomInY = action(tr("Zoom-in Over Y"), _plot, SLOT(zoomInY()), ":/toolbar/limits_zoom_in_y");
    actnZoomOutY = action(tr("Zoom-out Over Y"), _plot, SLOT(zoomOutY()), ":/toolbar/limits_zoom_out_y");

    actnSetLimitsX = action(tr("X-axis Limits..."), _plot, SLOT(limitsDlgX()));
    actnSetLimitsY = action(tr("Y-axis Limits..."), _plot, SLOT(limitsDlgY()));
    actnSetTextX = action(tr("X-axis Text..."), _plot, SLOT(axisTextDlgX()));
    actnSetTextY = action(tr("Y-axis Text..."), _plot, SLOT(axisTextDlgY()));
    actnSetTextT = action(tr("Title Text..."), _plot, SLOT(titleTextDlg()));

    actnFormatTitle = action(tr("Title Format..."), _plot, SLOT(titleFormatDlg()));
    actnFormatLegend = action(tr("Legend Format..."), _plot, SLOT(legendFormatDlg()));
    actnFormatX = action(tr("X-axis Format..."), _plot, SLOT(axisFormatDlgX()));
    actnFormatY = action(tr("Y-axis Format..."), _plot, SLOT(axisFormatDlgY()));
    actnSavePlotFormat = action(tr("Save Plot Format..."), this, SLOT(savePlotFormat()));
    actnLoadPlotFormat = action(tr("Load Plot Format..."), this, SLOT(loadPlotFormat()));

    actnCopyGraphData = action(tr("Copy Graph Data"), this, SLOT(copyGraphData()), ":/toolbar/copy_table");
    actnCopyGraphDataCurSegment = action(tr("Copy Graph Data (this segment)"), this, SLOT(copyGraphData()), ":/toolbar/copy_table");
    actnCopyGraphDataAllSegments = action(tr("Copy Graph Data (all segments)"), this, SLOT(copyGraphDataAllSegments()), ":/toolbar/copy_table");
    actnCopyGraphDataWithParams = action(tr("Copy Graph Data..."), this, SLOT(copyGraphDataWithParams()), ":/toolbar/copy_table");
    actnCopyPlotImage = action(tr("Copy Plot Image"), this, SLOT(copyPlotImage()), ":/toolbar/copy_img");
    actnCopyFormatFromSelection = action(tr("Copy Format"), this, SLOT(copyFormatFromSelection()), ":/toolbar/copy_fmt");
    actnPasteFormatToSelection = action(tr("Paste Format"), this, SLOT(pasteFormatToSelection()), ":/toolbar/paste_fmt");
    actnCopyPlotFormat = action(tr("Copy Plot Format"), this, SLOT(copyPlotFormat()), ":/toolbar/copy_fmt");
    actnPastePlotFormat = action(tr("Paste Plot Format"), this, SLOT(pastePlotFormat()), ":/toolbar/paste_fmt");

    actnToggleTitle = action(tr("Plot Title"), this, SLOT(toggleTitle()));
    actnToggleLegend = action(tr("Plot Legend"), this, SLOT(toggleLegend()));
    actnToggleTitle->setCheckable(true);
    actnToggleLegend->setCheckable(true);
}

void PlotFuncWindow::createMenuBar()
{
    _unitsMenuX = new UnitsMenu(this);
    _unitsMenuY = new UnitsMenu(this);
    connect(_unitsMenuX, &UnitsMenu::unitChanged, this, &PlotFuncWindow::setUnitX);
    connect(_unitsMenuY, &UnitsMenu::unitChanged, this, &PlotFuncWindow::setUnitY);

    menuPlot = menu(tr("Plot", "Menu title"), this, {
        actnUpdate, actnUpdateParams, actnFreeze, nullptr, actnShowFlippedTS, actnShowT, actnShowS, nullptr,
        actnSetTextT, actnSetTextX, actnSetTextY, _unitsMenuX->menu(), _unitsMenuY->menu(), nullptr, actnShowRoundTrip,
    });
    connect(menuPlot, &QMenu::aboutToShow, this, [this](){
        _unitsMenuX->menu()->setTitle("X-axis Unit");
        _unitsMenuY->menu()->setTitle("Y-axis Unit");
        _unitsMenuX->setUnit(getUnitX());
        _unitsMenuY->setUnit(getUnitY());
    });

    menuLimits = menu(tr("Limits", "Menu title"), this, {
        actnAutolimits, actnZoomIn, actnZoomOut, nullptr,
        actnSetLimitsX, actnAutolimitsX, actnZoomInX, actnZoomOutX, nullptr,
        actnSetLimitsY, actnAutolimitsY, actnZoomInY, actnZoomOutY
    });

    menuFormat = new QMenu(tr("Format", "Menu title"), this);
}

QList<QMenu*> PlotFuncWindow::menus()
{
    menuFormat->clear();
    menuFormat->addAction(actnFormatTitle);
    menuFormat->addAction(actnFormatLegend);
    menuFormat->addAction(actnFormatX);
    menuFormat->addAction(actnFormatY);
    for (auto& item : formatMenuItems())
        item.addTo(menuFormat);
    menuFormat->addSeparator();
    menuFormat->addAction(actnSavePlotFormat);
    menuFormat->addAction(actnLoadPlotFormat);
    return {menuPlot, menuLimits, menuFormat};
}

void PlotFuncWindow::createContextMenus()
{
    auto menuX = new QMenu(this);
    auto titleX = new QWidgetAction(this);
    auto labelX = new QLabel(tr("<b>Axis X</b>"));
    labelX->setMargin(6);
    titleX->setDefaultWidget(labelX);
    menuX->addAction(titleX);
    menuX->addMenu(_unitsMenuX->menu());
    menuX->addAction(tr("Limits..."), _plot, &QCPL::Plot::limitsDlgX);
    menuX->addAction(tr("Text..."), _plot, &QCPL::Plot::axisTextDlgX);
    menuX->addAction(tr("Format..."), _plot, &QCPL::Plot::axisFormatDlgX);
    menuX->addAction(QIcon(":/toolbar/limits_auto_x"), tr("Fit to Graphs"), _plot, SLOT(autolimitsX()));
    menuX->addSeparator();
    menuX->addAction(QIcon(":/toolbar/copy_fmt"), tr("Copy Format"), this, [this](){ QCPL::copyAxisFormat(_plot->xAxis); });
    menuX->addAction(QIcon(":/toolbar/paste_fmt"), tr("Paste Format"), this, [this](){ pasteAxisFormat(_plot->xAxis); });
    connect(menuX, &QMenu::aboutToShow, this, [this](){
        _unitsMenuX->menu()->setTitle(tr("Unit"));
        _unitsMenuX->setUnit(getUnitX());
    });

    auto menuY = new QMenu(this);
    auto titleY = new QWidgetAction(this);
    auto labelY = new QLabel(tr("<b>Axis Y</b>"));
    labelY->setMargin(6);
    titleY->setDefaultWidget(labelY);
    menuY->addAction(titleY);
    menuY->addMenu(_unitsMenuY->menu());
    menuY->addAction(tr("Limits..."), _plot, &QCPL::Plot::limitsDlgY);
    menuY->addAction(tr("Text..."), _plot, &QCPL::Plot::axisTextDlgY);
    menuY->addAction(tr("Format..."), _plot, &QCPL::Plot::axisFormatDlgY);
    menuY->addAction(QIcon(":/toolbar/limits_auto_y"), tr("Fit to Graphs"), _plot, SLOT(autolimitsY()));
    menuY->addSeparator();
    menuY->addAction(QIcon(":/toolbar/copy_fmt"), tr("Copy Format"), this, [this](){ QCPL::copyAxisFormat(_plot->yAxis); });
    menuY->addAction(QIcon(":/toolbar/paste_fmt"), tr("Paste Format"), this, [this](){ pasteAxisFormat(_plot->yAxis); });
    connect(menuY, &QMenu::aboutToShow, this, [this](){
        _unitsMenuY->menu()->setTitle(tr("Unit"));
        _unitsMenuY->setUnit(getUnitY());
    });

    auto menuLegend = new QMenu(this);
    menuLegend->addAction(actnFormatLegend);
    menuLegend->addSeparator();
    menuLegend->addAction(QIcon(":/toolbar/copy_fmt"), tr("Copy Format"), this, [this](){ QCPL::copyLegendFormat(_plot->legend); });
    menuLegend->addAction(QIcon(":/toolbar/paste_fmt"), tr("Paste Format"), this, [this](){ pasteLegendFormat(); });

    auto menuTitle = new QMenu(this);
    menuTitle->addAction(tr("Title Text..."), _plot, &QCPL::Plot::titleTextDlg);
    menuTitle->addAction(actnFormatTitle);
    menuTitle->addSeparator();
    menuTitle->addAction(QIcon(":/toolbar/copy_fmt"), tr("Copy Title Format"), this, [this](){ QCPL::copyTitleFormat(_plot->title()); });
    menuTitle->addAction(QIcon(":/toolbar/paste_fmt"), tr("Paste Title Format"), this, [this](){ pasteTitleFormat(); });

    auto menuGraph = new QMenu(this);
    menuGraph->addAction(actnCopyGraphData);
    menuGraph->addAction(actnCopyGraphDataCurSegment);
    menuGraph->addAction(actnCopyGraphDataAllSegments);
    menuGraph->addAction(actnCopyPlotImage);
    connect(menuGraph, &QMenu::aboutToShow, this, &PlotFuncWindow::graphsMenuAboutToShow);

    auto menuPlot = new QMenu(this);
    menuPlot->addAction(actnCopyPlotImage);
    menuPlot->addSeparator();
    menuPlot->addAction(actnCopyPlotFormat);
    menuPlot->addAction(actnPastePlotFormat);

    _plot->menuAxisX = menuX;
    _plot->menuAxisY = menuY;
    _plot->menuGraph = menuGraph;
    _plot->menuPlot = menuPlot;
    _plot->menuLegend = menuLegend;
    _plot->menuTitle = menuTitle;
}

void PlotFuncWindow::createToolBar()
{
    _buttonFrozenInfo = new FrozenStateButton(tr("Frozen info"), "frozen_info");

    auto t = toolbar();
    t->addAction(actnUpdate);
    t->addAction(actnUpdateParams);
    t->addSeparator();
    t->addAction(actnFreeze);
    actnFrozenInfo = t->addWidget(_buttonFrozenInfo);
    t->addSeparator();
    t->addAction(actnShowFlippedTS);
    t->addAction(actnShowT);
    t->addAction(actnShowS);
    t->addSeparator();
    t->addAction(actnAutolimits);
    t->addAction(actnZoomIn);
    t->addAction(actnZoomOut);
    t->addSeparator();
    t->addAction(actnAutolimitsX);
    t->addAction(actnZoomInX);
    t->addAction(actnZoomOutX);
    t->addSeparator();
    t->addAction(actnAutolimitsY);
    t->addAction(actnZoomInY);
    t->addAction(actnZoomOutY);
}

void PlotFuncWindow::createContent()
{
    _splitter = new QSplitter(Qt::Horizontal);

    PlotParamsPanelCtorOptions opts;
    opts.splitter = _splitter;
    opts.hasInfoPanel = function()->hasNotables();
    opts.hasDataGrid = function()->hasDataTable();
    opts.hasOptionsPanel = function()->hasOptions();
    _leftPanel = new PlotParamsPanel(opts);
    connect(_leftPanel, &PlotParamsPanel::updateNotables, this, &PlotFuncWindow::updateNotables);
    connect(_leftPanel, &PlotParamsPanel::updateDataGrid, this, &PlotFuncWindow::updateDataGrid);
    connect(_leftPanel, &PlotParamsPanel::optionsPanelRequired, this, &PlotFuncWindow::optionsPanelRequired);

    auto toolbar = new Ori::Widgets::FlatToolBar;
    toolbar->setIconSize(QSize(16, 16));
    _leftPanel->placeIn(toolbar);
    toolbar->addSeparator();

    _plot = new QCPL::Plot;
    _plot->legend->setVisible(false);
    _plot->legend->setSelectableParts(QCPLegend::spLegendBox);
    _plot->setAutoAddPlottableToLegend(false);
    connect(_plot, &QCPL::Plot::graphClicked, this, &PlotFuncWindow::graphSelected);
    connect(_plot, &QCPL::Plot::modified, this, [this](auto reason){
        schema()->markModified(reason.toLatin1().data());
        updatePlotItemToggleActions();
    });

    _plot->getAxisUnitString = [this](QCPAxis* axis) {
        if (axis == _plot->xAxis) return getUnitX()->name();
        if (axis == _plot->yAxis) return getUnitY()->name();
        return QString();
    };

    addTextVar("{func_name}", tr("Function name"), [this]{ return function()->name(); });

    _plot->addTextVarX("{unit}", tr("Unit of measurement"), [this]{ return getUnitX()->name(); });
    _plot->addTextVarX("{(unit)}", tr("Unit of measurement (in brackets)"), [this]{
        auto unit = getUnitX(); return unit == Z::Units::none() ? QString() : QStringLiteral("(%1)").arg(unit->name()); });

    _plot->addTextVarY("{unit}", tr("Unit of measurement"), [this]{ return getUnitY()->name(); });
    _plot->addTextVarY("{(unit)}", tr("Unit of measurement (in brackets)"), [this]{
        auto unit = getUnitY(); return unit == Z::Units::none() ? QString() : QStringLiteral("(%1)").arg(unit->name()); });

    _plot->setDefaultTextT(QStringLiteral("{func_name}"));
    _plot->setFormatterTextT(_plot->defaultTextT());

    _cursor = new QCPL::Cursor(_plot);
    connect(_cursor, &QCPL::Cursor::positionChanged, this, &PlotFuncWindow::updateCursorInfo);
    _plot->serviceGraphs().append(_cursor);
    auto axesLayer = _plot->layer("axes");
    if (axesLayer) _cursor->setLayer(axesLayer);

    _cursorMenu = new QMenu(tr("Cursor"), this);
    _cursorPanel = new QCPL::CursorPanel(_cursor);
    _cursorPanel->setAutoUpdateInfo(false);
    _cursorPanel->setNumberPrecision(AppSettings::instance().numberPrecisionData, false);
    _cursorPanel->placeIn(toolbar);
    _cursorPanel->fillMenu(_cursorMenu);

    _splitter->addWidget(_leftPanel);
    _splitter->addWidget(_plot);
    _splitter->setChildrenCollapsible(false);

    setContent(toolbar);
    setContent(_splitter);

    _graphs = new FunctionGraphSet(_plot, [this]{ return GraphUnits {getUnitX(), getUnitY()}; });
}

void PlotFuncWindow::createStatusBar()
{
    _statusBar = new Ori::Widgets::StatusBar(STATUS_PANELS_COUNT);
    _statusBar->connect(STATUS_UNIT_X, &QWidget::customContextMenuRequested, [this](const QPoint& p){
        _unitsMenuX->setUnit(getUnitX());
        _unitsMenuX->menu()->popup(_statusBar->mapToGlobal(STATUS_UNIT_X, p));
    });
    _statusBar->connect(STATUS_UNIT_Y, &QWidget::customContextMenuRequested, [this](const QPoint& p){
        _unitsMenuY->setUnit(getUnitY());
        _unitsMenuY->menu()->popup(_statusBar->mapToGlobal(STATUS_UNIT_Y, p));
    });
    setContent(_statusBar);
}

QList<BasicMdiChild::MenuItem> PlotFuncWindow::menuItems_View()
{
    QList<BasicMdiChild::MenuItem> menuItems;
    if (auto actions = _leftPanel->panelToogleActions(); !actions.isEmpty())
    {
        foreach (auto a, actions)
            menuItems << a;
        menuItems << BasicMdiChild::MenuItem();
    }
    return menuItems
            << _cursorMenu
            << actnToggleTitle
            << actnToggleLegend
            << viewMenuItems();
}

QList<BasicMdiChild::MenuItem> PlotFuncWindow::menuItems_Edit()
{
    return editMenuItems()
            << actnCopyGraphDataWithParams
            << actnCopyPlotImage
            << BasicMdiChild::MenuItem()
            // << actnCopyFormatFromSelection
            // << actnPasteFormatToSelection
            // TODO: "Copy/Paste plot format" should be just modes of
            // "Copy/Paste selected format" when nothing is selected
            << actnCopyPlotFormat
            << actnPastePlotFormat;
}

QCPL::Graph* PlotFuncWindow::selectedGraph() const
{
    auto graphs = _plot->selectedGraphs();
    return graphs.isEmpty()? nullptr: graphs.first();
}

void PlotFuncWindow::activateModeT()
{
    if (_exclusiveModeTS)
        actnShowS->setChecked(!actnShowT->isChecked());
    else if (!actnShowT->isChecked() && !actnShowS->isChecked())
        actnShowS->setChecked(true);

    updateModeTS();
}

void PlotFuncWindow::activateModeS()
{
    if (_exclusiveModeTS)
        actnShowT->setChecked(!actnShowS->isChecked());
    else if (!actnShowS->isChecked() && !actnShowT->isChecked())
        actnShowT->setChecked(true);

    updateModeTS();
}

void PlotFuncWindow::activateModeFlippedTS()
{
    updateModeTS();
}

void PlotFuncWindow::updateModeTS()
{
    showModeTS();

    if (_recalcWhenChangeModeTS)
    {
        update();
    }
    else
    {
        updateGraphs();
        afterUpdate();
        _plot->replot();
    }

    schema()->markModified("PlotFuncWindow::updateModeTS");
}

void PlotFuncWindow::showModeTS()
{
    bool flipped = actnShowFlippedTS->isChecked();
    _graphs->T()->setVisible(flipped || actnShowT->isChecked());
    _graphs->S()->setVisible(flipped || actnShowS->isChecked());
    _graphs->S()->setFlipped(flipped);

    actnShowT->setEnabled(!flipped);
    actnShowT->setVisible(!flipped);
    actnShowS->setEnabled(!flipped);
    actnShowS->setVisible(!flipped);
}

void PlotFuncWindow::updateNotables()
{
    if (_frozen)
    {
        _needRecalc = true;
        return;
    }
    if (_leftPanel->infoPanel() && _leftPanel->infoPanel()->isVisible())
        _leftPanel->infoPanel()->setHtml(_function->calculateNotables(getUnitX(), getUnitY()));
}

void PlotFuncWindow::updateStatusUnits()
{
    auto unitX = getUnitX();
    auto unitY = getUnitY();
    _statusBar->setText(STATUS_UNIT_X, QStringLiteral("X: ") + (
        unitX == Z::Units::none() ? QStringLiteral("n/a") : unitX->name()));
    _statusBar->setText(STATUS_UNIT_Y, QStringLiteral("Y: ") + (
        unitY == Z::Units::none() ? QStringLiteral("n/a") : unitY->name()));
}

void PlotFuncWindow::updateDataGrid()
{
    if (_leftPanel->dataGrid() && _leftPanel->dataGrid()->isVisible())
    {
        auto graph = selectedGraph();
        if (graph)
            _leftPanel->dataGrid()->setData(graph);
    }
}

void PlotFuncWindow::updateCursorInfo()
{
    if (_frozen)
    {
        _cursorPanel->update();
        // TODO:NEXT-VER calculate by interpolating between existing graph points
        return;
    }

    auto unitX = getUnitX();
    auto unitY = getUnitY();
    CursorInfoValues values;
    auto p = _cursor->position();
    values << CursorInfoValue(QStringLiteral("X"), unitX->toSi(p.x()));
    values << CursorInfoValue(QStringLiteral("Y"), unitY->toSi(p.y()));
    getCursorInfo({Z::Value(p.x(), unitX), Z::Value(p.y(), unitY)}, values);
    QStringList info;
    foreach (const CursorInfoValue& v, values)
    {
        QString line;
        if (v.isX())
            line = _cursorPanel->formatLinkX(Z::format(p.x()));
        else if (v.isY())
            line = _cursorPanel->formatLinkY(Z::format(p.y()));
        else
            line = QStringLiteral("%1 = %2").arg(v.name, Z::format(unitY->fromSi(v.value)));
        if (!v.note.isEmpty())
            line += ' ' + v.note;
        info << line;
    }
    _cursorPanel->setText(info.join(QStringLiteral("; ")));
}

void PlotFuncWindow::updateWithParams()
{
    if (configure()) update();
}

void PlotFuncWindow::update()
{
    if (_frozen)
    {
        _needRecalc = true;
        return;
    }

    calculate();

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

    _plot->updateTexts();
    updateStatusUnits();
    updateNotables();
    afterUpdate();

    _plot->replot();
}

void PlotFuncWindow::calculate()
{
    _function->calculate();
    if (!_function->ok())
    {
        showStatusError(_function->errorText());
        _graphs->clear();
    }
    else
    {
        clearStatusInfo();
        updateGraphs();
    }
}

void PlotFuncWindow::showStatusError(const QString& message)
{
    _statusBar->setText(STATUS_INFO, message);
    _statusBar->highlightError(STATUS_INFO);
}

void PlotFuncWindow::clearStatusInfo()
{
    _statusBar->clear(STATUS_INFO);
}

void PlotFuncWindow::updateGraphs()
{
    _graphs->update(_function);
}

void PlotFuncWindow::graphSelected(QCPGraph *graph)
{
    updateDataGrid();

    if (graph)
        _statusBar->setText(STATUS_POINTS, tr("Points: %1").arg(graph->data()->size()));
    else
        _statusBar->clear(STATUS_POINTS);
}

void PlotFuncWindow::showRoundTrip()
{
    InfoFuncWindow::open(new PlotFuncRoundTripFunction(windowTitle(), _function), this);
}

void PlotFuncWindow::freeze(bool frozen)
{
    _frozen = frozen;
    actnUpdate->setEnabled(!_frozen);
    actnUpdateParams->setEnabled(!_frozen);
    actnFrozenInfo->setVisible(_frozen);
    if (_frozen)
    {
        InfoFuncSummary summary(schema());
        summary.calculate();
        _buttonFrozenInfo->setInfo(summary.result());
    }
    _leftPanel->setOptionsPanelEnabled(!_frozen);
    if (!_frozen and _needRecalc)
        update();
}

QWidget* PlotFuncWindow::optionsPanelRequired()
{
    auto panel = makeOptionsPanel();
    if (!panel)
        qCritical() << "Function" << function()->alias() << "declared with "
            "hasOptions=true but its window does not provide options panel";
    return panel;
}

bool PlotFuncWindow::configure()
{
    bool ok = configureInternal();
    if (ok)
        schema()->events().raise(SchemaEvents::Changed, "PlotFuncWindow: configure");
    return ok;
}

void PlotFuncWindow::storeViewParts(ViewSettings& vs, ViewParts parts)
{
    if (parts.testFlag(VP_LIMITS_Y))
    {
        auto limits = _plot->limitsY();
        vs["y_min"] = limits.min;
        vs["y_max"] = limits.max;
    }
    if (parts.testFlag(VP_TITLE_Y))
    {
        vs["y_title"] = _plot->formatterTextY();
    }
    if (parts.testFlag(VP_UNIT_Y))
    {
        vs["y_unit"] = getUnitY()->alias();
    }
    if (parts.testFlag(VP_CUSRSOR_POS))
    {
        auto pos = _cursor->position();
        vs["cursor_x"] = pos.x();
        vs["cursor_y"] = pos.y();
    }
}

void PlotFuncWindow::restoreViewParts(const ViewSettings &vs, ViewParts parts)
{
    if (parts.testFlag(VP_LIMITS_Y))
    {
        bool minOk, maxOk;
        double min = vs["y_min"].toDouble(&minOk);
        double max = vs["y_max"].toDouble(&maxOk);
        if (minOk && maxOk)
            _plot->setLimitsY(min, max, false);
        else _autolimitsRequest = true;
    }
    if (parts.testFlag(VP_TITLE_Y))
    {
        if (vs.contains("y_title"))
            _plot->setFormatterTextY(vs["y_title"].toString());
        else _plot->setFormatterTextY(_plot->defaultTextY());
    }
    if (parts.testFlag(VP_UNIT_Y))
    {
        auto unit = Z::Units::findByAlias(vs["y_unit"].toString());
        _unitY = unit ? unit : getDefaultUnitY();
    }
    if (parts.testFlag(VP_CUSRSOR_POS))
    {
        bool xOk, yOk;
        double x = vs["cursor_x"].toDouble(&xOk);
        double y = vs["cursor_y"].toDouble(&yOk);
        if (xOk && yOk)
            _cursor->setPosition(x, y, false);
        else _centerCursorRequested = true;
    }
}

ElemDeletionReaction PlotFuncWindow::reactElemDeletion(const Elements& elems)
{
    return elems.contains(function()->arg()->element)
            ? ElemDeletionReaction::Close
            : ElemDeletionReaction::None;
}

void PlotFuncWindow::elementDeleting(Schema*, Element* elem)
{
    // TODO:NEXT-VER: see InfoFuncWindow::elementDeleting() and implement similar behaviour.
    // Current behaviour is ok for now as full FunctionBase::Dead processing
    // requires an ability of saving of frozen data.
    if (function()->arg()->element == elem)
        disableAndClose();
}

void PlotFuncWindow::disableAndClose()
{
    _frozen = true; // disable updates
    QTimer::singleShot(0, this, [this]{close();});
}

Z::Unit PlotFuncWindow::getUnitX() const
{
    auto defUnit = getDefaultUnitX();
    auto thisDim = Z::Units::guessDim(_unitX);
    auto funcDim = Z::Units::guessDim(defUnit);
    return thisDim == funcDim ? _unitX : defUnit;
}

Z::Unit PlotFuncWindow::getUnitY() const
{
    auto defUnit = getDefaultUnitY();
    auto thisDim = Z::Units::guessDim(_unitY);
    auto funcDim = Z::Units::guessDim(defUnit);
    return thisDim == funcDim ? _unitY : defUnit;
}

void PlotFuncWindow::setUnitX(Z::Unit unit)
{
    auto oldUnit = getUnitX();
    if (oldUnit == unit) return;
    _unitX = unit;
    _plot->updateTextX();
    updateStatusUnits();
    schema()->markModified("PlotFuncWindow::setUnitX");
    PlotHelpers::rescaleLimits(_plot, PlotAxis::X, oldUnit, unit);
    PlotHelpers::rescaleCursor(_cursor, PlotAxis::X, oldUnit, unit);
    update();
}

void PlotFuncWindow::setUnitY(Z::Unit unit)
{
    auto oldUnit = getUnitY();
    if (oldUnit == unit) return;
    _unitY = unit;
    _plot->updateTextY();
    updateStatusUnits();
    schema()->markModified("PlotFuncWindow::setUnitY");
    PlotHelpers::rescaleLimits(_plot, PlotAxis::Y, oldUnit, unit);
    PlotHelpers::rescaleCursor(_cursor, PlotAxis::Y, oldUnit, unit);
    update();
}

void PlotFuncWindow::optionChanged(AppSettingsOptions option)
{
    if (option == AppSettingsOptions::numberPrecisionData)
    {
        _cursorPanel->setNumberPrecision(AppSettings::instance().numberPrecisionData, false);
        updateCursorInfo();
    }
}

void PlotFuncWindow::copyGraphData()
{
    PlotHelpers::toClipboard(_plot->selectedGraph());
}

void PlotFuncWindow::copyGraphDataAllSegments()
{
    auto ts = _graphs->findBy(_plot->selectedGraph());
    if (ts) PlotHelpers::toClipboard(ts->segments());
}

void PlotFuncWindow::copyPlotImage()
{
    bool oldVisible = _cursor->visible();
    if (AppSettings::instance().exportHideCursor)
        _cursor->setVisible(false);

    QImage image(_plot->width(), _plot->height(), QImage::Format_RGB32);
    QCPPainter painter(&image);
    _plot->toPainter(&painter);
    emit finishImageBeforeCopy(&painter);
    qApp->clipboard()->setImage(image);

    if (oldVisible != _cursor->visible())
        _cursor->setVisible(oldVisible);
}

void PlotFuncWindow::copyGraphDataWithParams()
{
    PlotHelpers::exportGraphsData(_graphs, _plot->selectedGraph());
}

void PlotFuncWindow::graphsMenuAboutToShow()
{
    auto ts = _graphs->findBy(_plot->selectedGraph());
    bool manySegments = ts && ts->segmentsCount() > 1;
    actnCopyGraphData->setVisible(!manySegments);
    actnCopyGraphDataCurSegment->setVisible(manySegments);
    actnCopyGraphDataAllSegments->setVisible(manySegments);
}

void PlotFuncWindow::legendFormatDlg()
{
    QCPL::LegendFormatDlgProps props;
    props.title = tr("Legend Format");
    props.sampleText = tr("• Line T\n• Line S");
    if (QCPL::legendFormatDlg(_plot->legend, props))
    {
        schema()->markModified("PlotFuncWindow::formatLegend");
        updatePlotItemToggleActions();
    }
}

void PlotFuncWindow::addTextVar(const QString& name, const QString& descr, std::function<QString()> getter)
{
    _plot->addTextVarT(name, descr, getter);
    _plot->addTextVarX(name, descr, getter);
    _plot->addTextVarY(name, descr, getter);
}

void PlotFuncWindow::copyPlotFormat()
{
    QCPL::copyPlotFormat(_plot);
}

void PlotFuncWindow::pastePlotFormat()
{
    auto err = QCPL::pastePlotFormat(_plot);
    if (err.isEmpty())
    {
        _plot->updateTitleVisibility();
        _plot->replot();
        schema()->markModified("PlotFuncWindow::pastePlotFormat");
        updatePlotItemToggleActions();
    }
    else Ori::Dlg::info(err);
}

void PlotFuncWindow::pasteLegendFormat()
{
    auto err = QCPL::pasteLegendFormat(_plot->legend);
    if (err.isEmpty())
    {
        _plot->replot();
        schema()->markModified("PlotFuncWindow::pasteLegendFormat");
    }
    else Ori::Dlg::info(err);
}

void PlotFuncWindow::pasteTitleFormat()
{
    auto err = QCPL::pasteTitleFormat(_plot->title());
    if (err.isEmpty())
    {
        _plot->replot();
        schema()->markModified("PlotFuncWindow::pasteTitleFormat");
    }
    else Ori::Dlg::info(err);
}

void PlotFuncWindow::pasteAxisFormat(QCPAxis *axis)
{
    auto err = QCPL::pasteAxisFormat(axis);
    if (err.isEmpty())
    {
        _plot->replot();
        schema()->markModified("PlotFuncWindow::pasteAxisFormat");
    }
    else Ori::Dlg::info(err);
}

void PlotFuncWindow::toggleTitle()
{
    _plot->title()->setVisible(!_plot->title()->visible());
    _plot->updateTitleVisibility();
    _plot->replot();
    schema()->markModified("PlotFuncWindow::toggleTitle");
    updatePlotItemToggleActions();
}

void PlotFuncWindow::toggleLegend()
{
    _plot->legend->setVisible(!_plot->legend->visible());
    _plot->replot();
    schema()->markModified("PlotFuncWindow::toggleLegend");
    updatePlotItemToggleActions();
}

void PlotFuncWindow::updatePlotItemToggleActions()
{
    actnToggleTitle->setChecked(_plot->title()->visible());
    actnToggleLegend->setChecked(_plot->legend->visible());
}

void PlotFuncWindow::copyFormatFromSelection()
{
    qDebug() << "TODO: PlotFuncWindow::copyFormatFromSelection()";
}

void PlotFuncWindow::pasteFormatToSelection()
{
    qDebug() << "TODO: PlotFuncWindow::pasteFormatToSelection()";
}

void PlotFuncWindow::savePlotFormat()
{
    QString recentPath = CustomPrefs::recentDir("plot_format_path");
    auto fileName = QFileDialog::getSaveFileName(
        this, tr("Save Plot Format"), recentPath, tr("JSON files (*.json)\nAll files (*.*)"));
    if (fileName.isEmpty())
        return;
    CustomPrefs::setRecentDir("plot_format_path", fileName);
    QString err = QCPL::saveFormatToFile(fileName, _plot);
    if (!err.isEmpty())
        Ori::Dlg::error(err);
}

void PlotFuncWindow::loadPlotFormat()
{
    QString recentPath = CustomPrefs::recentDir("plot_format_path");
    auto fileName = QFileDialog::getOpenFileName(
        this, tr("Load Plot Format"), recentPath, tr("JSON files (*.json)\nAll files (*.*)"));
    if (fileName.isEmpty())
        return;
    CustomPrefs::setRecentDir("plot_format_path", fileName);
    QCPL::JsonReport report;
    auto err = QCPL::loadFormatFromFile(fileName, _plot, &report);
    if (!err.isEmpty())
    {
        Ori::Dlg::error(err);
        return;
    }
    _plot->replot();
    if (!report.isEmpty() && Z::Protocol::isEnabled)
    {
        foreach (auto err, report)
            Z::Protocol(Z::Protocol::Warning) << err.message;
    }
}
