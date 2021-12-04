#ifndef BEAM_SHAPE_WIDGET_H
#define BEAM_SHAPE_WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE

class BeamShapeWidget : public QWidget
{
    Q_OBJECT

public:
    BeamShapeWidget(QWidget *parent);

    void parentSizeChanged();
    void setInitialGeometry(const QRect& r);

    void setSizes(double t, double s);

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

#endif // BEAM_SHAPE_WIDGET_H
