#ifndef GRAPH_DATA_GRID_H
#define GRAPH_DATA_GRID_H

#include <QTableWidget>
#include "../../libs/qcustomplot/qcustomplot.h"

class GraphDataGrid : public QTableWidget
{
    Q_OBJECT
public:
    explicit GraphDataGrid();
    void setData(const QList<QCPData>& data);
public slots:
    void copy();
protected:
    void contextMenuEvent(QContextMenuEvent* event);
private:
    QMenu* _contextMenu = nullptr;
};

#endif // GRAPH_DATA_GRID_H
