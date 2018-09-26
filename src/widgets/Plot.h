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
    void extendLimits(double factor, bool replot = true);
    void extendLimitsX(double factor, bool replot = true) { extendLimits(xAxis, factor, replot); }
    void extendLimitsY(double factor, bool replot = true) { extendLimits(yAxis, factor, replot); }

public slots:
    void autolimits(bool replot = true);
    void autolimitsX(bool replot = true);
    void autolimitsY(bool replot = true);
    bool setLimitsDlg();
    bool setLimitsDlgX() { return setLimitsDlg(xAxis); }
    bool setLimitsDlgY() { return setLimitsDlg(yAxis); }
    void zoomIn() { extendLimits(-(_zoomStepX+_zoomStepY)/2.0); }
    void zoomOut() { extendLimits((_zoomStepX+_zoomStepY)/2.0); }
    void zoomInX() { extendLimitsX(-_zoomStepX); }
    void zoomOutX() { extendLimitsX(_zoomStepX); }
    void zoomInY() { extendLimitsY(-_zoomStepY); }
    void zoomOutY() { extendLimitsY(_zoomStepY); }

signals:
    void graphSelected(Graph*);
    void emptySpaceDoubleClicked(QMouseEvent *event);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    
private slots:
    void plotSelectionChanged();
    void graphClicked(QCPAbstractPlottable*);
    bool setLimitsDlg(QCPAxis* axis);

private:
    QVector<Graph*> _serviceGraphs;
    const double _safeMarginsX;
    const double _safeMarginsY;
    const double _zoomStepX;
    const double _zoomStepY;
    const int _numberPrecision;

    QPair<double, double> limits(QCPAxis* axis) const;
    void extendLimits(QCPAxis* axis, double factor, bool replot);
    void setLimits(QCPAxis* axis, double min, double max, bool replot);
    bool setLimitsDlg(QCPRange& range, const QString &title);
    bool sanitizeAxisRange(QCPAxis* axis);
    bool sanitizeRange(QCPRange& range, double safeMargin);
    void setAxisRange(QCPAxis* axis, const QCPRange &range);
    double safeMargins(QCPAxis* axis);
};

#endif // PLOT_H
