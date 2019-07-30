#include "PlotFuncWindow.h"

#include "../AppSettings.h"
#include "../core/Protocol.h"
#include "../funcs/InfoFunctions.h"
#include "../widgets/Appearance.h"
#include "../widgets/Plot.h"
#include "../widgets/FrozenStateButton.h"
#include "../widgets/GraphDataGrid.h"
#include "../widgets/CursorPanel.h"
#include "../widgets/PlotParamsPanel.h"
#include "../widgets/UnitWidgets.h"
#include "helpers/OriWidgets.h"
#include "widgets/OriFlatToolBar.h"
#include "widgets/OriLabels.h"
#include "widgets/OriStatusBar.h"
#include "../../libs/qcustomplot/qcpcursor.h"

using namespace Ori::Gui;

enum PlotWindowStatusPanels
{
    STATUS_UNIT_X,
    STATUS_UNIT_Y,
    STATUS_POINTS,
    STATUS_INFO,

    STATUS_PANELS_COUNT,
};

QMap<QString, int> PlotFuncWindow::_windowIndeces;

PlotFuncWindow::PlotFuncWindow(PlotFunction *func) : SchemaMdiChild(func->schema()), _function(func)
{
    _windowIndex = _windowIndeces[function()->name()];
    _windowIndeces[function()->name()] = _windowIndex+1;

    setTitleAndIcon(displayWindowTitle(), function()->iconPath());

    createContent();
    createActions();
    createMenuBar();
    createToolBar();
    createStatusBar();
}

PlotFuncWindow::~PlotFuncWindow()
{
    delete _function;
}

void PlotFuncWindow::createActions()
{
    actnUpdate = action(tr("Update"), this, SLOT(update()), ":/toolbar/update", Qt::Key_F5);
    actnUpdateParams = action(tr("Update With Params..."), this,
        SLOT(updateWithParams()), ":/toolbar/update_params", Qt::CTRL | Qt::Key_F5);

    actnShowT = action(tr("Show &T-plane"), this, SLOT(showT()), ":/toolbar/plot_t");
    actnShowS = action(tr("Show &S-plane"), this, SLOT(showS()), ":/toolbar/plot_s");
    actnShowTS = action(tr("TS-flipped Mode"), this, SLOT(showTS()), ":/toolbar/plot_ts");
    actnShowT->setCheckable(true);
    actnShowS->setCheckable(true);
    actnShowTS->setCheckable(true);
    actnShowT->setChecked(true);
    actnShowS->setChecked(true);

    actnShowRoundTrip = action(tr("Show Round-trip"), this, SLOT(showRoundTrip()));
    actnShowRoundTrip->setVisible(false);

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

    actnSetLimits = action(tr("Set Limits..."), _plot, SLOT(setLimitsDlg()));
    actnSetLimitsX = action(tr("Set X-axis Limits..."), _plot, SLOT(setLimitsDlgX()));
    actnSetLimitsY = action(tr("Set Y-axis Limits..."), _plot, SLOT(setLimitsDlgY()));
}

void PlotFuncWindow::createMenuBar()
{
    _unitsMenuX = new UnitsMenu(this);
    _unitsMenuY = new UnitsMenu(this);
    connect(_unitsMenuX, &UnitsMenu::unitChanged, this, &PlotFuncWindow::setUnitX);
    connect(_unitsMenuY, &UnitsMenu::unitChanged, this, &PlotFuncWindow::setUnitY);

    menuPlot = menu(tr("&Plot", "Menu title"), this, {
        actnUpdate, actnUpdateParams, actnFreeze, nullptr, actnShowTS, actnShowT, actnShowS, nullptr,
        _unitsMenuX->menu(), _unitsMenuY->menu(), nullptr, actnShowRoundTrip
    });
    connect(menuPlot, &QMenu::aboutToShow, [this](){
        _unitsMenuX->menu()->setTitle("X-axis Unit");
        _unitsMenuY->menu()->setTitle("Y-axis Unit");
        _unitsMenuX->setUnit(getUnitX());
        _unitsMenuY->setUnit(getUnitY());
    });

    menuLimits = menu(tr("&Limits", "Menu title"), this, {
        actnSetLimits, actnAutolimits, actnZoomIn, actnZoomOut, nullptr,
        actnSetLimitsX, actnAutolimitsX, actnZoomInX, actnZoomOutX, nullptr,
        actnSetLimitsY, actnAutolimitsY, actnZoomInY, actnZoomOutY
    });

    menuFormat = menu(tr("Fo&rmat", "Menu title"), this, {
        // TODO
    });

    auto menuX = new QMenu;
    auto titleX = new QWidgetAction(this);
    auto labelX = new QLabel(tr("Axis X"));
    labelX->setMargin(6);
    Z::Gui::setFontStyle(labelX, true);
    titleX->setDefaultWidget(labelX);
    menuX->addAction(titleX);
    menuX->addMenu(_unitsMenuX->menu());
    menuX->addSeparator();
    menuX->addAction(tr("Limits..."), _plot, SLOT(setLimitsDlgX()));
    menuX->addAction(QIcon(":/toolbar/limits_auto_x"), tr("Fit to Graphs"), _plot, SLOT(autolimitsX()));
    connect(menuX, &QMenu::aboutToShow, [this](){
        _unitsMenuX->menu()->setTitle(tr("Unit"));
        _unitsMenuX->setUnit(getUnitX());
    });

    auto menuY = new QMenu;
    auto titleY = new QWidgetAction(this);
    auto labelY = new QLabel(tr("Axis Y"));
    labelY->setMargin(6);
    Z::Gui::setFontStyle(labelY, true);
    titleY->setDefaultWidget(labelY);
    menuY->addAction(titleY);
    menuY->addMenu(_unitsMenuY->menu());
    menuY->addSeparator();
    menuY->addAction(tr("Limits..."), _plot, SLOT(setLimitsDlgY()));
    menuY->addAction(QIcon(":/toolbar/limits_auto_y"), tr("Fit to Graphs"), _plot, SLOT(autolimitsY()));
    connect(menuY, &QMenu::aboutToShow, [this](){
        _unitsMenuY->menu()->setTitle(tr("Unit"));
        _unitsMenuY->setUnit(getUnitY());
    });

    _plot->menuAxisX = menuX;
    _plot->menuAxisY = menuY;
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
    t->addAction(actnShowTS);
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

    _plot = new Plot;
    _plot->legend->setVisible(false);
    _plot->setAutoAddPlottableToLegend(false);
    connect(_plot, &Plot::graphSelected, this, &PlotFuncWindow::graphSelected);


    _cursor = new QCPCursor(_plot);
    connect(_cursor, &QCPCursor::positionChanged, this, &PlotFuncWindow::updateCursorInfo);
    _plot->serviceGraphs().append(_cursor);
    auto axesLayer = _plot->layer(QStringLiteral("axes"));
    if (axesLayer) _cursor->setLayer(axesLayer);

    _cursorPanel = new CursorPanel(_function, _cursor);
    _cursorPanel->setAutoUpdateInfo(false);
    _cursorPanel->placeIn(toolbar);

    _splitter->addWidget(_leftPanel);
    _splitter->addWidget(_plot);
    _splitter->setChildrenCollapsible(false);

    setContent(toolbar);
    setContent(_splitter);
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

QCPGraph *PlotFuncWindow::selectedGraph() const
{
    auto graphs = _plot->selectedGraphs();
    return graphs.isEmpty()? nullptr: graphs.first();
}

void PlotFuncWindow::showT()
{
    Z_NOTE("showT" << actnShowT->isChecked());

    if (_exclusiveModeTS)
        actnShowS->setChecked(!actnShowT->isChecked());
    else if (!actnShowT->isChecked() && !actnShowS->isChecked())
        actnShowS->setChecked(true);
    updateGraphs();
    afterUpdate();
    _plot->replot();
    schema()->markModified();
}

void PlotFuncWindow::showS()
{
    Z_NOTE("showS" << actnShowS->isChecked());

    if (_exclusiveModeTS)
        actnShowT->setChecked(!actnShowS->isChecked());
    else if (!actnShowS->isChecked() && !actnShowT->isChecked())
        actnShowT->setChecked(true);
    updateGraphs();
    afterUpdate();
    _plot->replot();
    schema()->markModified();
}

void PlotFuncWindow::showTS()
{
    Z_NOTE("showTS" << actnShowTS->isChecked());

    updateTSModeActions();
    updateGraphs();
    afterUpdate();
    _plot->replot();
    schema()->markModified();
}

void PlotFuncWindow::updateTSModeActions()
{
    bool ts = !actnShowTS->isChecked();
    actnShowT->setEnabled(ts);
    actnShowT->setVisible(ts);
    actnShowS->setEnabled(ts);
    actnShowS->setVisible(ts);
}

void PlotFuncWindow::updateNotables()
{
    if (_frozen)
    {
        _needRecalc = true;
        return;
    }
    if (_leftPanel->infoPanel() && _leftPanel->infoPanel()->isVisible())
        _leftPanel->infoPanel()->setHtml(_function->calculateNotables());
}

void PlotFuncWindow::updateTitles()
{
    updateTitle();
    updateTitleX();
    updateTitleY();
}

void PlotFuncWindow::updateTitle()
{
    QString title = _title;
    title.replace(TitlePlaceholder::defaultTitle(), getDefaultTitle());
    title = formatTitleSpecial(title);
    _plot->title()->setText(title);
}

void PlotFuncWindow::updateTitleX()
{
    QString title = _titleX;
    title.replace(TitlePlaceholder::defaultTitle(), getDefaultTitleX());
    title = formatTitleSpecial(title);
    _plot->xAxis->setLabel(title);
}

void PlotFuncWindow::updateTitleY()
{
    QString title = _titleY;
    title.replace(TitlePlaceholder::defaultTitle(), getDefaultTitleY());
    title = formatTitleSpecial(title);
    _plot->yAxis->setLabel(title);
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
            _leftPanel->dataGrid()->setData(graph->data());
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
    _cursorPanel->update(_function->calculatePoint(_cursor->position().x()));
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

    updateTitles();
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
        //debug_LogGraphsCount();
        _statusBar->setText(STATUS_INFO, _function->errorText());
        _statusBar->highlightError(STATUS_INFO);
        for (auto g : _graphsT) _plot->removePlottable(g);
        for (auto g : _graphsS) _plot->removePlottable(g);
        _graphsT.clear();
        _graphsS.clear();
    }
    else
    {
        _statusBar->clear(STATUS_INFO);
        updateGraphs();
    }
}

void PlotFuncWindow::updateGraphs()
{
    updateGraphs(Z::Plane_T);
    updateGraphs(Z::Plane_S);
}

void PlotFuncWindow::updateGraphs(Z::WorkPlane plane)
{
    QVector<QCPGraph*>& graphs = plane == Z::Plane_T? _graphsT: _graphsS;
    bool isVisible = actnShowTS->isChecked()
        or (plane == Z::Plane_T and actnShowT->isChecked())
        or (plane == Z::Plane_S and actnShowS->isChecked());
    int resultCount = _function->resultCount(plane);
    for (int i = 0; i < resultCount; i++)
    {
        QCPGraph *g;
        if (i >= graphs.size())
        {
            g = _plot->addGraph();
            g->setPen(getLineSettings(plane));
            graphs.append(g);
        }
        else g = graphs[i];
        g->setVisible(isVisible);
        if (isVisible)
            fillGraphWithFunctionResults(plane, g, i);
    }
    while (graphs.size() > resultCount)
    {
        _plot->removePlottable(graphs.last());
        graphs.removeLast();
    }
}

void PlotFuncWindow::fillGraphWithFunctionResults(Z::WorkPlane plane, QCPGraph *graph, int resultIndex)
{
    bool flipped = plane == Z::WorkPlane::Plane_S and actnShowTS->isChecked();
    auto result = _function->result(plane, resultIndex);
    int count = result.pointsCount();
    auto xs = result.x();
    auto ys = result.y();
    auto unitX = getUnitX();
    auto unitY = getUnitY();
    QSharedPointer<QCPGraphDataContainer> data(new QCPGraphDataContainer);
    for (int i = 0; i < count; i++)
    {
        // TODO: possible optimization: extract unit's SI factor before loop
        // and replace the call of virtual method with simple multiplication
        double x = unitX->fromSi(xs.at(i));
        double y = unitY->fromSi(ys.at(i)) * (flipped ? -1 : 1);
        data->add(QCPGraphData(x, y));
    }
    graph->setData(data);
}

QPen PlotFuncWindow::getLineSettings(Z::WorkPlane plane)
{
    return plane == Z::Plane_T? QPen(Qt::darkGreen): QPen(Qt::red);
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
    // TODO:NEXT-VER
}

void PlotFuncWindow::recalcRequired(Schema*)
{
    update();
}

void PlotFuncWindow::debug_LogGraphsCount()
{
    Z_INFO("Graphs on plot:" << _plot->graphCount());
    for (int i = 0; i < _plot->graphCount(); i++)
        Z_INFO("  graph:" << qintptr(_plot->graph(i)));
    Z_INFO("Graphs in T array:" << _graphsT.size());
    for (int i = 0; i < _graphsT.size(); i++)
        Z_INFO("  graph:" << qintptr(_graphsT.at(i)));
    Z_INFO("Graphs in S array:" << _graphsS.size());
    for (int i = 0; i < _graphsS.size(); i++)
        Z_INFO("  graph:" << qintptr(_graphsS.at(i)));
}

void PlotFuncWindow::freeze(bool frozen)
{
    _frozen = frozen;
    actnUpdate->setEnabled(!_frozen);
    actnUpdateParams->setEnabled(!_frozen);
    actnFrozenInfo->setVisible(_frozen);
    _buttonFrozenInfo->setInfo(InfoFuncSummary(schema()).calculate());
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
        schema()->events().raise(SchemaEvents::Changed);
    return ok;
}

void PlotFuncWindow::storeView(int key)
{
    ViewState view;
    view.limitsX = _plot->limitsX();
    view.limitsY = _plot->limitsY();
    view.cursorPos = _cursor->position();
    view.unitX = _unitX;
    view.unitY = _unitY;
    _storedView[key] = view;
    storeViewSpecific(key);
}

void PlotFuncWindow::restoreView(int key)
{
    if (_storedView.contains(key))
    {
        const ViewState& view = _storedView[key];
        _unitX = view.unitX;
        _unitY = view.unitY;
        _plot->setLimitsX(view.limitsX, false);
        _plot->setLimitsY(view.limitsY, false);
        _cursor->setPosition(view.cursorPos, false);
    }
    else
    {
        // Units for the view will be guessed from dim on first request
        _unitX = Z::Units::none();
        _unitY = Z::Units::none();
        _autolimitsRequest = true;
        _centerCursorRequested = true;
    }
    restoreViewSpecific(key);
}

QString PlotFuncWindow::displayWindowTitle() const
{
    if (_windowIndex > 0)
        return QString("%1 (%2)").arg(function()->name()).arg(_windowIndex);
    return function()->name();
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
    QTimer::singleShot(0, [this]{this->close();});
}

Z::Unit PlotFuncWindow::getUnitX() const
{
    auto defUnit = function()->defaultUnitX();
    auto thisDim = Z::Units::guessDim(_unitX);
    auto funcDim = Z::Units::guessDim(defUnit);
    return thisDim == funcDim ? _unitX : defUnit;
}

Z::Unit PlotFuncWindow::getUnitY() const
{
    auto defUnit = function()->defaultUnitY();
    auto thisDim = Z::Units::guessDim(_unitY);
    auto funcDim = Z::Units::guessDim(defUnit);
    return thisDim == funcDim ? _unitY : defUnit;
}

void PlotFuncWindow::setUnitX(Z::Unit unit)
{
    auto oldUnit = getUnitX();
    if (oldUnit == unit) return;
    _unitX = unit;
    if (function()->ok())
    {
        auto limits = _plot->limitsX();
        limits.min = unit->fromSi(oldUnit->toSi(limits.min));
        limits.max = unit->fromSi(oldUnit->toSi(limits.max));
        updateGraphs();
        _plot->setLimitsX(limits, false);
        updateTitleX();
        updateStatusUnits();
        afterSetUnitsX(oldUnit, unit);
        schema()->markModified();
        _plot->replot();
    }
}

void PlotFuncWindow::setUnitY(Z::Unit unit)
{
    auto oldUnit = getUnitY();
    if (oldUnit == unit) return;
    _unitY = unit;
    if (function()->ok())
    {
        auto limits = _plot->limitsY();
        limits.min = unit->fromSi(oldUnit->toSi(limits.min));
        limits.max = unit->fromSi(oldUnit->toSi(limits.max));
        updateGraphs();
        _plot->setLimitsY(limits, false);
        updateTitleY();
        updateStatusUnits();
        afterSetUnitsY(oldUnit, unit);
        schema()->markModified();
        _plot->replot();
    }
}
