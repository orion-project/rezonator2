#ifndef PLOTPARAMSPANEL_H
#define PLOTPARAMSPANEL_H

#include <QStackedWidget>

QT_BEGIN_NAMESPACE
class QAction;
class QSplitter;
class QTextBrowser;
class QToolBar;
QT_END_NAMESPACE

class GraphDataGrid;

typedef QWidget* (*MakePanelFunc)(class PlotParamsPanel*);
typedef void (*ActivatePanelFunc)(class PlotParamsPanel*);

struct PlotParamsPanelCtorOptions
{
    QSplitter *splitter;
    bool hasInfoPanel;
    bool hasDataGrid;
    bool hasOptionsPanel;
};

class PlotParamsPanel : public QStackedWidget
{
    Q_OBJECT

public:
    explicit PlotParamsPanel(PlotParamsPanelCtorOptions options, QWidget *parent = 0);

    void placeIn(QToolBar* toolbar);

    QTextBrowser* infoPanel() const;
    GraphDataGrid* dataGrid() const;
    QWidget* optionsPanel() const;

    void setOptionsPanelEnabled(bool on);

signals:
    void updateDataGrid();
    void updateNotables();
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

#endif // PLOTPARAMSPANEL_H
