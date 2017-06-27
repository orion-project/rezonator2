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

typedef QWidget* (*MakePanel)();
typedef void (*ActivatePanel)(class PlotParamsPanel*);

class PlotParamsPanel : public QStackedWidget
{
    Q_OBJECT

public:
    explicit PlotParamsPanel(QSplitter *splitter, MakePanel makeParamsPanel, QWidget *parent = 0);

    void placeIn(QToolBar* toolbar);

    QTextBrowser* infoPanel() const { return (QTextBrowser*)_panels.at(0).widget; }
    GraphDataGrid* dataGrid() const { return (GraphDataGrid*)_panels.at(1).widget; }

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
        MakePanel makeWidget;
        ActivatePanel activate;
    };
    QVector<PanelInfo> _panels;
    QSplitter* _splitter;

    void initPanel(const QString &title, const char *icon, MakePanel make, ActivatePanel activate);
    void saveActiveSize();
};

#endif // PLOTPARAMSPANEL_H
