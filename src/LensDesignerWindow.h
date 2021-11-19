#ifndef LENS_DESIGNER_WINDOW_H
#define LENS_DESIGNER_WINDOW_H

#include "core/Parameters.h"

#include <QSplitter>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsView;
QT_END_NAMESPACE

namespace LensDesignerItems {
class OpticalAxisItem;
class LensShapeItem;
} // namespace LensDesignerItems

class LensDesignerWidget : public QSplitter, public Z::ParameterListener
{
    Q_OBJECT

public:
    explicit LensDesignerWidget(QWidget *parent = nullptr);
    ~LensDesignerWidget();

    void parameterChanged(Z::ParameterBase*) override;

private:
    QGraphicsScene* _scene;
    QGraphicsView* _view;
    Z::Parameters _params;
    Z::Parameter *_D, *_R1, *_R2, *_IOR, *_T;
    LensDesignerItems::OpticalAxisItem* _axis;
    LensDesignerItems::LensShapeItem* _shape;

    void redraw();
};


class LensDesignerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LensDesignerWindow(QWidget *parent = nullptr);

    static void showWindow();

private:
    LensDesignerWidget* _designer;
};

#endif // LENS_DESIGNER_WINDOW_H
