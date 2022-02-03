#ifndef BEAM_SHAPE_EXTENSION_H
#define BEAM_SHAPE_EXTENSION_H

#include <QWidget>

#include "../core/Values.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPainter;
class QWidget;
QT_END_NAMESPACE

class PlotFuncWindow;

class BeamShapeWidget : public QWidget
{
    Q_OBJECT

public:
    BeamShapeWidget(QWidget *parent);

    void parentSizeChanged();
    void setInitialGeometry(const QRect& r);

    void setShape(const Z::PointTS& shape);

protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void enterEvent(QEvent*) override;
    void leaveEvent(QEvent*) override;
    void paintEvent(QPaintEvent*) override;

private:
    QMenu *_menu;
    enum {
        CENTER, TOP_LEFT, TOP, TOP_RIGHT, RIGHT,
        BOTTOM_RIGHT, BOTTOM, BOTTOM_LEFT, LEFT
    } _site;
    int x0, y0, X, Y, W, H;
    int _textW, _textH;
    bool _drawResizeBorder = false;
    double _ratio = -1;

    void updateSite(const QPoint &pos);
    void paint(QPaintDevice *target, bool showBackground) const;
    void copyImage() const;
};

class BeamShapeExtension : public QObject
{
    Q_OBJECT

public:
    BeamShapeExtension(PlotFuncWindow* parent);

    void setShape(const Z::PointTS& shape);

private:
    PlotFuncWindow* _parent;
    QAction* _actnShowBeamShape;
    BeamShapeWidget *_beamShape = nullptr;
    QRect _beamShapeGeom;

    void showBeamShape();
};

#endif // BEAM_SHAPE_EXTENSION_H
