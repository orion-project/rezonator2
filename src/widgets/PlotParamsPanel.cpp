#include <QAction>
#include <QDebug>
#include <QSplitter>
#include <QTextBrowser>
#include <QToolBar>

#include "PlotParamsPanel.h"
#include "../AppSettings.h"

#include "qcpl_graph_grid.h"

#define DEFAULT_PANEL_W 150

static QWidget* makeGraphDataGrid(PlotParamsPanel*)
{
    auto grid = new QCPL::GraphDataGrid;
    grid->setNumberPrecision(AppSettings::instance().numberPrecisionData);
    grid->getExportSettings = [](){
        QCPL::GraphDataExportSettings es;
        auto as = AppSettings::instance();
        es.csv = as.exportAsCsv;
        es.systemLocale = as.exportSystemLocale;
        es.numberPrecision = as.exportNumberPrecision;
        es.transposed = as.exportTransposed;
        return es;
    };
    return grid;
}

PlotParamsPanel::PlotParamsPanel(PlotParamsPanelCtorOptions options, QWidget *parent) :
    QStackedWidget(parent), _splitter(options.splitter)
{
    if (options.hasInfoPanel)
        _infoPanelIndex = initPanel(tr("Show Special Points"), ":/toolbar16/points",
            /* makeWidget: */ [](PlotParamsPanel*)->QWidget*{ return new QTextBrowser; },
            /* onActivate: */ [](PlotParamsPanel* self){ emit self->updateNotables(); });

    if (options.hasDataGrid)
        _dataGridIndex = initPanel(tr("Show Data Table"), ":/toolbar16/table",
            /* makeWidget: */ makeGraphDataGrid,
            /* onActivate: */ [](PlotParamsPanel* self){ emit self->updateDataGrid(); });

    if (options.hasOptionsPanel)
        _optionsPanelIndex = initPanel(tr("Show Options"), ":/toolbar16/params",
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

void PlotParamsPanel::fillActions(QList<QAction*>& actions)
{
    for (const PanelInfo& panel: _panels)
        actions << panel.action;
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
            panel.size = _splitter->sizes().first();
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

void PlotParamsPanel::optionChanged(AppSettingsOptions option)
{
    qDebug() << "option changed";
    if (option == AppSettingsOptions::numberPrecisionData)
    {
        auto grid = dataGrid();
        if (grid)
            grid->setNumberPrecision(AppSettings::instance().numberPrecisionData);
    }
}
