#ifndef PLOT_H
#define PLOT_H

#include "../../libs/qcustomplot/qcustomplot.h"
#include "PlotUtils.h"

class Plot : public QCustomPlot
{
    Q_OBJECT

public:
    explicit Plot();

    QVector<QCPGraph*>& serviceGraphs() { return _serviceGraphs; }

    AxisLimits limitsX() const { return limits(xAxis); }
    AxisLimits limitsY() const { return limits(yAxis); }
    void setLimitsX(const AxisLimits& p, bool replot) { setLimitsX(p.min, p.max, replot); }
    void setLimitsY(const AxisLimits& p, bool replot) { setLimitsY(p.min, p.max, replot); }
    void setLimitsX(double min, double max, bool replot = true) { setLimits(xAxis, min, max, replot); }
    void setLimitsY(double min, double max, bool replot = true) { setLimits(yAxis, min, max, replot); }
    void extendLimits(double factor, bool replot = true);
    void extendLimitsX(double factor, bool replot = true) { extendLimits(xAxis, factor, replot); }
    void extendLimitsY(double factor, bool replot = true) { extendLimits(yAxis, factor, replot); }

//    bool isTitleVisible() const { return _title; }
//    void setTitleVisible(bool on);
//    QString title() const;
//    void setTitle(const QString& title);

    QCPTextElement* title() { return _title; }

    QMenu *menuAxisX = nullptr;
    QMenu *menuAxisY = nullptr;

    bool useSafeMargins = true;
    bool excludeServiceGraphsFromAutolimiting = true;

    // TODO: should be a row below the title, it can be 0 or 1, depending on if title is visible
    int axisRectRow() const { return 1; }
    int axisRectCol() const { return 0; }

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
    void graphSelected(QCPGraph*);
    void emptySpaceDoubleClicked(QMouseEvent *event);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    
private slots:
    void plotSelectionChanged();
    void graphClicked(QCPAbstractPlottable*);
    bool setLimitsDlg(QCPAxis* axis);

private:
    enum class PlotPart
    {
        None,
        AxisX,
        AxisY
    };

    QCPTextElement* _title;
    QString _titleText;
    QVector<QCPGraph*> _serviceGraphs;
    const double _safeMarginsX;
    const double _safeMarginsY;
    const double _zoomStepX;
    const double _zoomStepY;
    const int _numberPrecision;

    AxisLimits limits(QCPAxis* axis) const;
    void extendLimits(QCPAxis* axis, double factor, bool replot);
    void setLimits(QCPAxis* axis, double min, double max, bool replot);
    bool setLimitsDlg(QCPRange& range, const QString &title);
    bool sanitizeAxisRange(QCPAxis* axis);
    bool sanitizeRange(QCPRange& range, double safeMargin);
    void setAxisRange(QCPAxis* axis, const QCPRange &range);
    double safeMargins(QCPAxis* axis);
    PlotPart selectedPart() const;
    QString getAxisTitle(QCPAxis* axis) const;
};

#endif // PLOT_H
