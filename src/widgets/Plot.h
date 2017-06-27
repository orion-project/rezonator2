#ifndef PLOT_H
#define PLOT_H

#include "../../libs/qcustomplot/qcustomplot.h"

typedef QCPGraph Graph;

class Plot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit Plot();

    QVector<Graph*>& serviceGraphs() { return _serviceGraphs; }

public slots:
    void autolimits(bool autoReplot = true);

signals:
    void graphSelected(Graph*);

protected:
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    
private slots:
    void plotSelectionChanged();
    void graphClicked(QCPAbstractPlottable*);

private:
    QVector<Graph*> _serviceGraphs;
};

#endif // PLOT_H
