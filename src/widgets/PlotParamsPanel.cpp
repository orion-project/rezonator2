#include <QAction>
#include <QSplitter>
#include <QTextBrowser>
#include <QToolBar>

#include "PlotParamsPanel.h"
#include "GraphDataGrid.h"

#define DEFAULT_PANEL_W 200

PlotParamsPanel::PlotParamsPanel(QSplitter *splitter, MakePanel makeParamsPanel, QWidget *parent) :
    QStackedWidget(parent), _splitter(splitter)
{
    initPanel(tr("Show Special Points"), ":/misc16/points",
        []()->QWidget*{ return new QTextBrowser; },
        [](PlotParamsPanel* self){ emit self->updateNotables(); });

    initPanel(tr("Show Data Table"), ":/misc16/table",
        []()->QWidget*{ return new GraphDataGrid; },
        [](PlotParamsPanel* self){ emit self->updateDataGrid(); });

    if (makeParamsPanel)
        initPanel(tr("Additional Parameters"), ":/misc16/params", makeParamsPanel, nullptr);

    setVisible(false); // all actions unchecked
}

void PlotParamsPanel::initPanel(const QString& title, const char* icon, MakePanel make, ActivatePanel activate)
{
    auto action = new QAction(QIcon(icon), title, this);
    connect(action, SIGNAL(triggered()), this, SLOT(showPanel()));
    action->setCheckable(true);
    _panels.append({action, nullptr, DEFAULT_PANEL_W, make, activate});
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
                if (panel.activate)
                    panel.activate(this);
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
