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
    void setPosition(const double& x, const double& y);
    void setPositionX(const double& x) { setPosition(x, position().y()); }
    void setPositionY(const double& y) { setPosition(position().x(), y); }
    void moveToCenter();
    inline void pixelPosition(double& x, double& y) const;
    inline void setPixelPosition(const QPointF& p) { setPixelPosition(p.x(), p.y()); }
    inline void setPixelPosition(const QPoint& p) { setPixelPosition(p.x(), p.y()); }
    void setPixelPosition(const double& x, const double& y);
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
