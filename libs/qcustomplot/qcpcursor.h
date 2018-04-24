#ifndef QCPCURSOR_H
#define QCPCURSOR_H

#include "qcustomplot.h"

class QCPCursor : public QCPGraph
{
    Q_OBJECT
public:
    enum CursorShape { VerticalLine, HorizontalLine, CrossLines };
public:
    explicit QCPCursor(QCustomPlot *plot);
    QPointF position() const;
    void setPosition(const double& x, const double& y, bool replot = true);
    void setPosition(const QPointF& pos, bool replot = true) { setPosition(pos.x(), pos.y(), replot); }
    void setPositionX(const double& x, bool replot = true) { setPosition(x, position().y(), replot); }
    void setPositionY(const double& y, bool replot = true) { setPosition(position().x(), y, replot); }
    void moveToCenter(bool replot = true);
    inline void pixelPosition(double& x, double& y) const;
    inline void setPixelPosition(const QPointF& p, bool replot = true) { setPixelPosition(p.x(), p.y(), replot); }
    inline void setPixelPosition(const QPoint& p, bool replot = true) { setPixelPosition(p.x(), p.y(), replot); }
    void setPixelPosition(const double& x, const double& y, bool replot = true);
    bool followMouse() const { return _followMouse; }
    CursorShape shape() const { return _shape; }
    void setShape(CursorShape value);
public slots:
    void setFollowMouse(bool value);
    void setVisible(bool on); // make slot
signals:
    void positionChanged();
protected:
    void drawLinePlot(QCPPainter *painter, QVector<QPointF>*) const override;
private:
    bool _followMouse;
    bool _canDragX, _canDragY;
    bool _dragX, _dragY;
    CursorShape _shape;
private slots:
    void mouseDoubleClick(QMouseEvent*);
    void mouseMove(QMouseEvent*);
    void mousePress(QMouseEvent*);
    void mouseRelease(QMouseEvent*);
};

#endif // QCPCURSOR_H
