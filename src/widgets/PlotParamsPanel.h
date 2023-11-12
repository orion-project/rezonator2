#ifndef PLOT_PARAMS_PANEL_H
#define PLOT_PARAMS_PANEL_H

#include "../app/AppSettings.h"

#include <QStackedWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QSplitter;
class QTextBrowser;
class QToolBar;
QT_END_NAMESPACE

namespace QCPL {
class GraphDataGrid;
}

typedef QWidget* (*MakePanelFunc)(class PlotParamsPanel*);
typedef void (*ActivatePanelFunc)(class PlotParamsPanel*);

struct PlotParamsPanelCtorOptions
{
    QSplitter *splitter;
    bool hasInfoPanel;
    bool hasDataGrid;
    bool hasOptionsPanel;
};

class PlotParamsPanel : public QStackedWidget, public IAppSettingsListener
{
    Q_OBJECT

public:
    explicit PlotParamsPanel(PlotParamsPanelCtorOptions options, QWidget *parent = nullptr);
    ~PlotParamsPanel() override;

    void placeIn(QToolBar* toolbar);
    QList<QAction*> panelToogleActions() const;

    QTextBrowser* infoPanel() const;
    QCPL::GraphDataGrid* dataGrid() const;
    QWidget* optionsPanel() const;

    void setOptionsPanelEnabled(bool on);

    void optionChanged(AppSettingsOptions option) override;

signals:
    void updateDataGrid();
    void updateSpecPoints();
    QWidget* optionsPanelRequired();

private slots:
    void showPanel();

private:
    struct PanelInfo
    {
        QAction* action;
        QWidget* widget;
        int size;
        MakePanelFunc makeWidget;
        ActivatePanelFunc onActivate;
    };
    QVector<PanelInfo> _panels;
    int _infoPanelIndex = -1;
    int _dataGridIndex = -1;
    int _optionsPanelIndex = -1;
    QSplitter* _splitter;
    bool _optionsPanelEnabled = true;

    int initPanel(const QString &title, const char *icon, MakePanelFunc makeWidget, ActivatePanelFunc onActivate);
    void saveActiveSize();
};

#endif // PLOT_PARAMS_PANEL_H
