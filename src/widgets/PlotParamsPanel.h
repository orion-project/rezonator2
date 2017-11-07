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

typedef QWidget* (*MakePanelFunc)();
typedef void (*ActivatePanelFunc)(class PlotParamsPanel*);

struct PlotParamsPanelCtorOptions
{
    QSplitter *splitter;
    bool hasInfoPanel;
    bool hasDataGrid;
    MakePanelFunc makeParamsPanel;
};

class PlotParamsPanel : public QStackedWidget
{
    Q_OBJECT

public:
    explicit PlotParamsPanel(PlotParamsPanelCtorOptions options, QWidget *parent = 0);

    void placeIn(QToolBar* toolbar);

    QTextBrowser* infoPanel() const;
    GraphDataGrid* dataGrid() const;

signals:
    void updateDataGrid();
    void updateNotables();

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
    QSplitter* _splitter;

    int initPanel(const QString &title, const char *icon, MakePanelFunc makeWidget, ActivatePanelFunc onActivate);
    void saveActiveSize();
};

#endif // PLOTPARAMSPANEL_H
