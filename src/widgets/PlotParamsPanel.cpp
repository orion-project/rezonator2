#include <QAction>
#include <QSplitter>
#include <QTextBrowser>
#include <QToolBar>

#include "PlotParamsPanel.h"
#include "GraphDataGrid.h"

#define DEFAULT_PANEL_W 200

PlotParamsPanel::PlotParamsPanel(PlotParamsPanelCtorOptions options, QWidget *parent) :
    QStackedWidget(parent), _splitter(options.splitter)
{
    if (options.hasInfoPanel)
        _infoPanelIndex = initPanel(tr("Show Special Points"), ":/misc16/points",
            /* makeWidget: */ []()->QWidget*{ return new QTextBrowser; },
            /* onActivate: */ [](PlotParamsPanel* self){ emit self->updateNotables(); });

    if (options.hasDataGrid)
        _dataGridIndex = initPanel(tr("Show Data Table"), ":/misc16/table",
            /* makeWidget: */ []()->QWidget*{ return new GraphDataGrid; },
            /* onActivate: */ [](PlotParamsPanel* self){ emit self->updateDataGrid(); });

    if (options.makeParamsPanel)
        initPanel(tr("Additional Parameters"), ":/misc16/params", options.makeParamsPanel, nullptr);

    setVisible(false); // all actions unchecked
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
                    addWidget(panel.widget = panel.makeWidget());
                _splitter->setSizes(QList<int>() << panel.size <<
                    _splitter->width() - _splitter->handleWidth() - panel.size);
                setCurrentWidget(panel.widget);
                show();
                if (panel.onActivate)
                    panel.onActivate(this);
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

GraphDataGrid* PlotParamsPanel::dataGrid() const
{
    return _dataGridIndex < 0? nullptr: qobject_cast<GraphDataGrid*>(_panels.at(_dataGridIndex).widget);
}
