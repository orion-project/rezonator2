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

    QPair<double, double> limitsX() const { return limits(xAxis); }
    QPair<double, double> limitsY() const { return limits(yAxis); }
    void setLimitsX(const QPair<double, double>& p, bool replot) { setLimitsX(p.first, p.second, replot); }
    void setLimitsY(const QPair<double, double>& p, bool replot) { setLimitsY(p.first, p.second, replot); }
    void setLimitsX(double min, double max, bool replot = true) { setLimits(xAxis, min, max, replot); }
    void setLimitsY(double min, double max, bool replot = true) { setLimits(yAxis, min, max, replot); }
    void extendLimitsX(double factor, bool replot = true) { extendLimits(xAxis, factor, replot); }
    void extendLimitsY(double factor, bool replot = true) { extendLimits(yAxis, factor, replot); }

public slots:
    void autolimits(bool replot = true);

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

    QPair<double, double> limits(QCPAxis* axis) const;
    void extendLimits(QCPAxis* axis, double factor, bool replot);
    void setLimits(QCPAxis* axis, double min, double max, bool replot);
};

#endif // PLOT_H
