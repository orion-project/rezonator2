#ifndef PLOT_H
#define PLOT_H

#include "../../libs/qcustomplot/qcustomplot.h"

typedef QCPGraph Graph;

struct PlotLimits
{
    double minX, maxX;
    double minY, maxY;
};

class Plot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit Plot();

    QVector<Graph*>& serviceGraphs() { return _serviceGraphs; }

    void changeLimitsX(double factor) { changeLimits(xAxis, factor); }
    void changeLimitsY(double factor) { changeLimits(yAxis, factor); }
    void changeLimits(const PlotLimits& limits);
    PlotLimits limits() const;

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

    void changeLimits(QCPAxis* axis, double factor);
};

#endif // PLOT_H
