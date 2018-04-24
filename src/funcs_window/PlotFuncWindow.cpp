#include "PlotFuncWindow.h"
#include "../core/Protocol.h"
#include "../funcs/InfoFunctions.h"
#include "../widgets/Plot.h"
#include "../widgets/FrozenStateButton.h"
#include "../widgets/GraphDataGrid.h"
#include "../widgets/CursorPanel.h"
#include "../widgets/PlotParamsPanel.h"
#include "widgets/OriFlatToolBar.h"
#include "widgets/OriLabels.h"
#include "widgets/OriStatusBar.h"
#include "../../libs/qcustomplot/qcpcursor.h"

enum PlotWindowStatusPanels
{
    STATUS_POINTS,
    STATUS_INFO,

    STATUS_PANELS_COUNT,
};

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
    for (auto button : _modeButtons)
        button->setChecked(button->mode() == mode);
}

//------------------------------------------------------------------------------
//                                PlotFuncWindow
//------------------------------------------------------------------------------

PlotFuncWindow::PlotFuncWindow(PlotFunction *func) :
    SchemaMdiChild(func->schema()), _function(func)
{
    createActions();
    createMenuBar();
    createToolBar();
    createContent();
    createStatusBar();
}

PlotFuncWindow::~PlotFuncWindow()
{
    delete _function;
}

void PlotFuncWindow::createActions()
{
    //---------------- Plot
    actnUpdate = new QAction(tr("Update", "Plot action"), this);
    actnUpdate->setShortcut(Qt::Key_F5);
    actnUpdate->setIcon(QIcon(":/toolbar/update"));
    connect(actnUpdate, SIGNAL(triggered()), this, SLOT(update()));

    actnUpdateParams = new QAction(tr("Update With Params...", "Plot action"), this);
    actnUpdateParams->setShortcut(Qt::CTRL | Qt::Key_F5);
    actnUpdateParams->setIcon(QIcon(":/toolbar/update_params"));
    connect(actnUpdateParams, SIGNAL(triggered()), this, SLOT(updateWithParams()));

    actnShowT = new QAction(tr("Show &T-plane", "Plot action"), this);
    actnShowS = new QAction(tr("Show &S-plane", "Plot action"), this);
    actnShowTS = new QAction(tr("TS-flipped Mode", "Plot action"), this);
    actnShowT->setCheckable(true);
    actnShowS->setCheckable(true);
    actnShowTS->setCheckable(true);
    actnShowT->setChecked(true);
    actnShowS->setChecked(true);
    actnShowS->setIcon(QIcon(":/toolbar/plot_s"));
    actnShowT->setIcon(QIcon(":/toolbar/plot_t"));
    actnShowTS->setIcon(QIcon(":/toolbar/plot_ts"));
    connect(actnShowT, SIGNAL(triggered()), this, SLOT(showT()));
    connect(actnShowS, SIGNAL(triggered()), this, SLOT(showS()));
    connect(actnShowTS, SIGNAL(triggered()), this, SLOT(showTS()));

    actnShowTS->setVisible(false); //< TODO:NEXT_VER

    actnShowRoundTrip = new QAction(tr("Show Round-trip", "Plot action"), this);
    connect(actnShowRoundTrip, SIGNAL(triggered()), this, SLOT(showRoundTrip()));

    actnFreeze = new QAction(tr("Freeze", "Function actions"), this);
    actnFreeze->setShortcut(Qt::CTRL | Qt::Key_F);
    actnFreeze->setCheckable(true);
    actnFreeze->setIcon(QIcon(":/toolbar/freeze"));
    connect(actnFreeze, SIGNAL(toggled(bool)), this, SLOT(freeze(bool)));

    //---------------- Limits
    actnAutolimits = new QAction(QIcon(":/toolbar/limits_auto"), tr("Fit to Graphs"), this);
    connect(actnAutolimits, &QAction::triggered, [&](){ _plot->autolimits(); });
    actnAutolimitsX = new QAction(QIcon(":/toolbar/limits_auto_x"), tr("Fit to Graphs Over X"), this);
    connect(actnAutolimitsX, &QAction::triggered, [&](){ _plot->autolimitsX(); });
    actnAutolimitsY = new QAction(QIcon(":/toolbar/limits_auto_y"), tr("Fit to Graphs Over Y"), this);
    connect(actnAutolimitsY, &QAction::triggered, [&](){ _plot->autolimitsY(); });

    actnZoomInX = new QAction(QIcon(":/toolbar/limits_zoom_in_x"), tr("Zoom-in Over X"), this);
    connect(actnZoomInX, &QAction::triggered, [&](){ _plot->extendLimitsX(-0.1); });
    actnZoomOutX = new QAction(QIcon(":/toolbar/limits_zoom_out_x"), tr("Zoom-out Over X"), this);
    connect(actnZoomOutX, &QAction::triggered, [&](){ _plot->extendLimitsX(0.1); });
    actnZoomInY = new QAction(QIcon(":/toolbar/limits_zoom_in_y"), tr("Zoom-in Over Y"), this);
    connect(actnZoomInY, &QAction::triggered, [&](){ _plot->extendLimitsY(-0.1); });
    actnZoomOutY = new QAction(QIcon(":/toolbar/limits_zoom_out_y"), tr("Zoom-out Over Y"), this);
    connect(actnZoomOutY, &QAction::triggered, [&](){ _plot->extendLimitsY(0.1); });

    actnSetLimitsX = new QAction(tr("Set X-axis Limits..."), this);
    connect(actnSetLimitsX, &QAction::triggered, [&](){ _plot->setLimitsDlgX(); });
    actnSetLimitsY = new QAction(tr("Set Y-axis Limits..."), this);
    connect(actnSetLimitsY, &QAction::triggered, [&](){ _plot->setLimitsDlgY(); });
}

void PlotFuncWindow::createMenuBar()
{
    menuPlot = new QMenu(tr("&Plot", "Menu title"), this);
    menuPlot->addAction(actnUpdate);
    menuPlot->addAction(actnUpdateParams);
    menuPlot->addSeparator();
    menuPlot->addAction(actnShowT);
    menuPlot->addAction(actnShowS);
    menuPlot->addAction(actnShowTS);
    menuPlot->addSeparator();
    menuPlot->addAction(actnShowRoundTrip);

    menuLimits = new QMenu(tr("&Limits", "Menu title"), this);
    menuLimits->addAction(actnSetLimitsX);
    menuLimits->addAction(actnSetLimitsY);
    menuLimits->addSeparator();
    menuLimits->addAction(actnAutolimits);
    menuLimits->addAction(actnAutolimitsX);
    menuLimits->addAction(actnAutolimitsY);
    menuLimits->addSeparator();
    menuLimits->addAction(actnZoomInX);
    menuLimits->addAction(actnZoomOutX);
    menuLimits->addAction(actnZoomInY);
    menuLimits->addAction(actnZoomOutY);

    menuFormat = new QMenu(tr("Fo&rmat", "Menu title"), this);
}

void PlotFuncWindow::createToolBar()
{
    toolbar()->addAction(actnUpdate);
    toolbar()->addAction(actnUpdateParams);
    toolbar()->addSeparator();
    toolbar()->addAction(actnFreeze);

    _buttonFrozenInfo = new FrozenStateButton;
    _buttonFrozenInfo->setToolTip(tr("Frozen info", "Function actions"));
    _buttonFrozenInfo->setIcon(QIcon(":/toolbar/frozen_info"));
    actnFrozenInfo = toolbar()->addWidget(_buttonFrozenInfo);

    toolbar()->addSeparator();
    toolbar()->addAction(actnShowT);
    toolbar()->addAction(actnShowS);
    toolbar()->addAction(actnShowTS);
    toolbar()->addSeparator();
    toolbar()->addAction(actnAutolimits);
    toolbar()->addAction(actnAutolimitsX);
    toolbar()->addAction(actnAutolimitsY);
    toolbar()->addSeparator();
    toolbar()->addAction(actnZoomInX);
    toolbar()->addAction(actnZoomOutX);
    toolbar()->addAction(actnZoomInY);
    toolbar()->addAction(actnZoomOutY);
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
    connect(_plot, SIGNAL(graphSelected(Graph*)), this, SLOT(graphSelected(Graph*)));

    _cursor = new QCPCursor(_plot);
    connect(_cursor, SIGNAL(positionChanged()), this, SLOT(updateCursorInfo()));
    _plot->serviceGraphs().append(_cursor);

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
    setContent(_statusBar);
}

Graph* PlotFuncWindow::selectedGraph() const
{
    QList<Graph*> graphs = _plot->selectedGraphs();
    return graphs.isEmpty()? nullptr: graphs.first();
}

void PlotFuncWindow::showT()
{
    Z_NOTE("showT" << actnShowT->isChecked());

    if (!actnShowT->isChecked() && !actnShowS->isChecked())
        actnShowS->setChecked(true);
    updateVisibilityTS();
    // TODO:NEXT-VER Schema.ModifiedForms := True;
}

void PlotFuncWindow::showS()
{
    Z_NOTE("showS" << actnShowS->isChecked());

    if (!actnShowS->isChecked() && !actnShowT->isChecked())
        actnShowT->setChecked(true);
    updateVisibilityTS();
    // TODO:NEXT-VER Schema.ModifiedForms := True;
}

void PlotFuncWindow::showTS()
{
    Z_NOTE("showTS" << actnShowTS->isChecked());

    updateTSModeActions();
    updateVisibilityTS();
    // TODO:NEXT-VER Schema.ModifiedForms := True;
}

void PlotFuncWindow::updateVisibilityTS()
{
    bool t = actnShowT->isChecked() || actnShowTS->isChecked();
    bool s = actnShowS->isChecked() || actnShowTS->isChecked();
    foreach (Graph *g, _graphsT) g->setVisible(t);
    foreach (Graph *g, _graphsS) g->setVisible(s);
    _plot->replot();
}

void PlotFuncWindow::updateTSModeActions()
{
    actnShowT->setEnabled(!actnShowTS->isChecked());
    actnShowS->setEnabled(!actnShowTS->isChecked());
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

void PlotFuncWindow::updateAxesTitles()
{
    // TODO
}

void PlotFuncWindow::updateDataGrid()
{
    if (_leftPanel->dataGrid() && _leftPanel->dataGrid()->isVisible())
    {
        Graph *graph = selectedGraph();
        if (graph)
            _leftPanel->dataGrid()->setData(graph->data()->values());
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
    if (configure(this))
    {
        // TODO:NEXT-VER Schema.ModifiedForms := True;
        update();
    }
}

void PlotFuncWindow::update()
{
    if (_frozen)
    {
        _needRecalc = true;
        return;
    }

    bool replot = not _autolimitsRequest;

    calculate(replot);

    if (_autolimitsRequest)
    {
        _autolimitsRequest = false;
        _plot->autolimits(true);
    }

    if (_centerCursorRequested)
    {
        _centerCursorRequested = false;
        _cursor->moveToCenter();
        // Cursor info will be updated by positionChanged signal
    }
    else updateCursorInfo();

    updateAxesTitles();
    updateNotables();
}

void PlotFuncWindow::calculate(bool replot)
{
    _function->calculate();
    if (!_function->ok())
    {
        debug_LogGraphsCount();
        _statusBar->setText(STATUS_INFO, _function->errorText());
        _statusBar->highlightError(STATUS_INFO);
        for (Graph* g : _graphsT) if (g->parentPlot() == _plot) _plot->removePlottable(g);
        for (Graph* g : _graphsS) if (g->parentPlot() == _plot) _plot->removePlottable(g);
        _graphsT.clear();
        _graphsS.clear();
    }
    else
    {
        _statusBar->clear(STATUS_INFO);
        updateGraphs(Z::Plane_T);
        updateGraphs(Z::Plane_S);
    }
    if (replot)
        _plot->replot();
}

void PlotFuncWindow::updateGraphs(Z::WorkPlane plane)
{
    QVector<Graph*>& graphs = plane == Z::Plane_T? _graphsT: _graphsS;
    int resultCount = _function->resultCount(plane);
    for (int i = 0; i < resultCount; i++)
    {
        Graph *g;
        if (i >= graphs.size())
        {
            g = _plot->addGraph();
            g->setPen(getLineSettings(plane));
            graphs.append(g);
        }
        else g = graphs[i];
        auto result = _function->result(plane, i);
        g->setData(result.x, result.y);
        if (g->parentPlot() != _plot)
            _plot->addPlottable(g);
    }
    while (graphs.size() > resultCount)
    {
        _plot->removePlottable(graphs.last());
        graphs.removeLast();
    }
}

QPen PlotFuncWindow::getLineSettings(Z::WorkPlane plane)
{
    return plane == Z::Plane_T? QPen(Qt::darkGreen): QPen(Qt::red);
}

void PlotFuncWindow::graphSelected(Graph *graph)
{
    updateDataGrid();

    if (graph)
        _statusBar->setText(STATUS_POINTS, tr("Points: %1").arg(graph->data()->count()));
    else
        _statusBar->clear(STATUS_POINTS);
}

void PlotFuncWindow::showRoundTrip()
{
    // TODO:NEXT-VER
}

void PlotFuncWindow::schemaChanged(Schema*)
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

bool PlotFuncWindow::configure(QWidget* parent)
{
    bool ok = configureInternal(parent);
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
    _storedView[key] = view;
}

void PlotFuncWindow::restoreView(int key)
{
    if (_storedView.contains(key))
    {
        const ViewState& view = _storedView[key];
        _plot->setLimitsX(view.limitsX, false);
        _plot->setLimitsY(view.limitsY, false);
        _cursor->setPosition(view.cursorPos, false);
    }
    else
    {
        _autolimitsRequest = true;
        _centerCursorRequested = true;
    }
}

