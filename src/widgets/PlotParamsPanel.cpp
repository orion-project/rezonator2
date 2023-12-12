#include "PlotParamsPanel.h"

#include "PlotHelpers.h"
#include "../app/Appearance.h"

#include "qcpl_graph_grid.h"

#include <QAction>
#include <QDebug>
#include <QSplitter>
#include <QTextBrowser>
#include <QToolBar>

#define DEFAULT_PANEL_W 150

static QWidget* makeGraphDataGrid(PlotParamsPanel*)
{
    auto grid = new QCPL::GraphDataGrid;
    grid->setNumberPrecision(AppSettings::instance().numberPrecisionData);
    grid->getExportSettings = PlotHelpers::makeExportSettings;
    return grid;
}

static QWidget* makeSpecPointsView(PlotParamsPanel*)
{
    auto browser = new QTextBrowser;
    browser->setOpenLinks(false);
    browser->setOpenExternalLinks(false);
    browser->setFont(Z::Gui::CodeEditorFont().get());
    Z::Gui::applyTextBrowserStyleSheet(browser, ":/style/spec-points");
    return browser;
}

PlotParamsPanel::PlotParamsPanel(PlotParamsPanelCtorOptions options, QWidget *parent) :
    QStackedWidget(parent), _splitter(options.splitter)
{
    if (options.hasInfoPanel)
        _infoPanelIndex = initPanel(tr("Special Points"), ":/toolbar/points",
            /* makeWidget: */ makeSpecPointsView,
            /* onActivate: */ [](PlotParamsPanel* self){ emit self->updateSpecPoints(); });

    if (options.hasDataGrid)
        _dataGridIndex = initPanel(tr("Data Table"), ":/toolbar/table",
            /* makeWidget: */ makeGraphDataGrid,
            /* onActivate: */ [](PlotParamsPanel* self){ emit self->updateDataGrid(); });

    if (options.hasOptionsPanel)
        _optionsPanelIndex = initPanel(tr("Function Params"), ":/toolbar/params",
            /* makeWidget: */ [](PlotParamsPanel* self)->QWidget*{ return emit self->optionsPanelRequired(); },
            /* onActivate: */ nullptr);

    setVisible(false); // all actions unchecked

    AppSettings::instance().registerListener(this);
}

PlotParamsPanel::~PlotParamsPanel()
{
    AppSettings::instance().unregisterListener(this);
}

int PlotParamsPanel::initPanel(const QString& title, const char* icon, MakePanelFunc makeWidget, ActivatePanelFunc onActivate)
{
    auto action = new QAction(QIcon(icon), title, this);
    connect(action, SIGNAL(triggered()), this, SLOT(showPanel()));
    action->setCheckable(true);
    _panels.append({action, nullptr, DEFAULT_PANEL_W, makeWidget, onActivate});
    return _panels.size()-1;
}

void PlotParamsPanel::placeIn(QToolBar* toolbar)
{
    for (const PanelInfo& panel: _panels)
        toolbar->addAction(panel.action);
}

QList<QAction*> PlotParamsPanel::panelToogleActions() const
{
    QList<QAction*> actions;
    for (const PanelInfo& panel: _panels)
        actions << panel.action;
    return actions;
}

void PlotParamsPanel::showPanel()
{
    saveActiveSize();

    auto action = qobject_cast<QAction*>(sender());
    if (action->isChecked())
    {
        for (PanelInfo& panel: _panels)
            if (panel.action == action)
            {
                if (!panel.widget)
                {
                    panel.widget = panel.makeWidget(this);
                    Q_ASSERT(panel.widget != nullptr);
                    addWidget(panel.widget);
                }
                _splitter->setSizes(QList<int>() << panel.size <<
                    _splitter->width() - _splitter->handleWidth() - panel.size);
                setCurrentWidget(panel.widget);
                show();
                if (panel.onActivate)
                    panel.onActivate(this);

                // options panel can be disabled if function is frozen
                if (panel.widget == optionsPanel())
                    panel.widget->setEnabled(_optionsPanelEnabled);

                break;
            }
    }
    else hide();

    for (const PanelInfo& panel: _panels)
        if (panel.action != action)
            panel.action->setChecked(false);
}

void PlotParamsPanel::saveActiveSize()
{
    if (!isVisible()) return;

    auto activePanel = currentWidget();
    if (!activePanel) return;

    for (PanelInfo& panel: _panels)
        if (panel.widget == activePanel)
        {
            panel.size = _splitter->sizes().constFirst();
            break;
        }
}

QTextBrowser* PlotParamsPanel::infoPanel() const
{
    return _infoPanelIndex < 0? nullptr: qobject_cast<QTextBrowser*>(_panels.at(_infoPanelIndex).widget);
}

QCPL::GraphDataGrid* PlotParamsPanel::dataGrid() const
{
    return _dataGridIndex < 0? nullptr: qobject_cast<QCPL::GraphDataGrid*>(_panels.at(_dataGridIndex).widget);
}

QWidget* PlotParamsPanel::optionsPanel() const
{
    return _optionsPanelIndex < 0? nullptr: _panels.at(_optionsPanelIndex).widget;
}

void PlotParamsPanel::setOptionsPanelEnabled(bool on)
{
    _optionsPanelEnabled = on;
    auto panel = optionsPanel();
    if (panel)
        panel->setEnabled(on);
}

void PlotParamsPanel::optionChanged(AppSettingsOption option)
{
    if (option == AppSettingsOption::NumberPrecisionData)
    {
        auto grid = dataGrid();
        if (grid)
            grid->setNumberPrecision(AppSettings::instance().numberPrecisionData);
    }
}
